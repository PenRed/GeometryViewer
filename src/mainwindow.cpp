#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      penRedViewer(nullptr),
      constructViewer(nullptr),
      destroyViewer(nullptr),
      width3D(400), height3D(400), pixelSize3D(0.1)
{

    ui->setupUi(this);
    nViewers = 0;
    activeViewer = 0;

    //Set perspective box items
    ui->perspectiveSelector->addItem("X");   //0
    ui->perspectiveSelector->addItem("Y");   //1
    ui->perspectiveSelector->addItem("Z");   //2
    ui->perspectiveSelector->addItem("3D");  //3

    //Configure save dialog
    QList<QUrl> urls;
    urls << QUrl::fromLocalFile(QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).first())
         << QUrl::fromLocalFile(QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).first())
         << QUrl::fromLocalFile(QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first());

    saveDialog.setSidebarUrls(urls);
    saveDialog.setFileMode(QFileDialog::AnyFile);
    saveDialog.setAcceptMode(QFileDialog::AcceptSave);
    connect(&saveDialog, &QFileDialog::fileSelected, this, &MainWindow::on_saveImage);

    //Configure load dialogs

    loadConfigDialog.setSidebarUrls(urls);
    loadConfigDialog.setFileMode(QFileDialog::ExistingFile);
    loadConfigDialog.setAcceptMode(QFileDialog::AcceptOpen);
    connect(&loadConfigDialog, &QFileDialog::fileSelected, this, &MainWindow::on_loadConfig);

    loadQuadricDialog.setSidebarUrls(urls);
    loadQuadricDialog.setFileMode(QFileDialog::ExistingFile);
    loadQuadricDialog.setAcceptMode(QFileDialog::AcceptOpen);
    connect(&loadQuadricDialog, &QFileDialog::fileSelected, this, &MainWindow::on_loadQuadric);

    loadMeshDialog.setSidebarUrls(urls);
    loadMeshDialog.setFileMode(QFileDialog::ExistingFile);
    loadMeshDialog.setAcceptMode(QFileDialog::AcceptOpen);
    connect(&loadMeshDialog, &QFileDialog::fileSelected, this, &MainWindow::on_loadMesh);

    //Initialize buffers
    for(auto& buffer : buffers)
        buffer.resize(viewer::maxPixels*3);
    for(auto& matImage : matImages)
        matImage.resize(viewer::maxPixels);
    for(auto& bodyImage : bodyImages)
        bodyImage.resize(viewer::maxPixels);
    for(auto& distance : distances)
        distance.resize(viewer::maxPixels);

    //** Try to create a penRed viewer **//

    QString libPath = QCoreApplication::applicationDirPath() + "/libgeoView_C";
    printf("%s\n", libPath.toStdString().c_str());
    fflush(stdout);
    viewerLib.setFileName(libPath);
    if(viewerLib.load()){
        printf("Geometry library load success\n");
        fflush(stdout);

        //Get viewer constructor
        constructViewer = (viewerConstructor) viewerLib.resolve("pen_geoView_new");

        if(constructViewer){
            //Instance a viewer
            penRedViewer = constructViewer() ;

            //Set resolution 3D
            penRedViewer->set3DResolution(width3D, height3D, pixelSize3D, pixelSize3D, 0.3490658503988659);
        }else{
            printf("Unable to load the viewer constructor function 'pen_geoView_new'\n");
        }

        //Get viewer destructor
        destroyViewer = (viewerDestructor) viewerLib.resolve("pen_geoView_delete");


    }else{
        printf("Unable to load geometry library: %s\n", viewerLib.errorString().toStdString().c_str());
        fflush(stdout);
        throw std::runtime_error("Geometry library not found");
    }

    //******************************

    // ** Instance all viewers
    for(size_t i = 0; i < maxViewers; ++i)
        createViewer(i);

    //Show only the first viewer and select it
    viewersArray[0]->show();
    nViewers = 1;
    setActiveViewer(0);    
}

MainWindow::~MainWindow()
{
    //Delete viewers
    for(auto& viewer : viewersArray)
        delete viewer;

    delete ui;
    if(destroyViewer)
        destroyViewer(penRedViewer);
}

void MainWindow::on_saveImage(const QString &file){

    if(viewersArray[activeViewer] != nullptr){
        printf("Saving to: %s\n", file.toStdString().c_str());
        fflush(stdout);
        viewersArray[activeViewer]->readImage().save(file,"PNG");
    }

}

void MainWindow::on_loadConfig(const QString &file){
    printf("Loading geometry from configuration '%s'", file.toStdString().c_str());

    //Initialize the viewer
    int err = penRedViewer->init(file.toStdString().c_str());
    if(err != 0){
        printf("Error loading the geometry\n");
        fflush(stdout);
        return;
    }

    //Emit load signal
    emit geometryLoad();
}

void MainWindow::on_loadQuadric(const QString &file){
    printf("Loading quadric geometry from file '%s'", file.toStdString().c_str());

    //Write a default configuration file
    FILE* fout = nullptr;
    fout = fopen("quadConf.txt","w");
    fprintf(fout,"type \"PEN_QUADRIC\"\n");
    fprintf(fout,"input-file \"%s\"\n", file.toStdString().c_str());
    fprintf(fout,"processed-geo-file \"report.geo\"\n");
    fclose(fout);

    //Initialize the viewer
    int err = penRedViewer->init("quadConf.txt");
    if(err != 0){
        printf("Error loading the geometry\n");
        fflush(stdout);
        return;
    }

    //Emit load signal
    emit geometryLoad();
}

void MainWindow::on_loadMesh(const QString &file){
    printf("Loading triangular mesh geometry from file '%s'", file.toStdString().c_str());

    //Write a default configuration file
    FILE* fout = nullptr;
    fout = fopen("triMeshConf.txt","w");
    fprintf(fout,"type \"MESH_BODY\"\n");
    fprintf(fout,"input-file \"%s\"\n", file.toStdString().c_str());
    //fprintf(fout,"report-file \"report.geo\"\n");
    fclose(fout);

    //Initialize the viewer
    int err = penRedViewer->init("triMeshConf.txt");
    if(err != 0){
        printf("Error loading the geometry\n");
        fflush(stdout);
        return;
    }

    //Emit load signal
    emit geometryLoad();
}

void MainWindow::setActiveViewer(unsigned index){

    if(viewersArray[activeViewer] != nullptr)
        viewersArray[activeViewer]->setStyleSheet("border: none");
    activeViewer = index;
    viewersArray[activeViewer]->setStyleSheet("border: 5px outset rgb(0,128,255); border-radius: 4px;");

    updateViewerInfo();
}

void MainWindow::updateViewerInfo(){

    viewer* pviewer = viewersArray[activeViewer];

    ui->Xedit->setText(QString::number(pviewer->readX(), 'e', 5));
    ui->Yedit->setText(QString::number(pviewer->readY(), 'e', 5));
    ui->Zedit->setText(QString::number(pviewer->readZ(), 'e', 5));

    if(pviewer->readMatView()){
        ui->matBodyViewButton->setText("Mat");
        ui->matBodyViewButton->setChecked(true);
    }else{
        ui->matBodyViewButton->setText("Body");
        ui->matBodyViewButton->setChecked(false);
    }

    ui->perspectiveSelector->setCurrentIndex(pviewer->readPerspective());

    ui->resolutionEditX->setValue(pviewer->readImageWidth());
    ui->resolutionEditY->setValue(pviewer->readImageHeight());

    ui->pixelSizeEdit->setValue(pviewer->readPixelSize());

    ui->rhoEdit->setText(QString::number(pviewer->readRho(), 'e', 4));
    ui->thetaEdit->setText(QString::number(pviewer->readTheta(), 'f', 5));
    ui->phiEdit->setText(QString::number(pviewer->readPhi(), 'f', 5));

    ui->lookX->setText(QString::number(pviewer->readCamera3DX(), 'e', 5));
    ui->lookY->setText(QString::number(pviewer->readCamera3DY(), 'e', 5));
    ui->lookZ->setText(QString::number(pviewer->readCamera3DZ(), 'e', 5));

    updateKey();
}

void MainWindow::updateKey(){
    ui->keyText->setHtml(viewersArray[activeViewer]->readKeyText());
}

void MainWindow::createViewer(const size_t index){

    if(index < maxViewers){
        //Create a new viewer
        viewer* newViewer = new viewer(buffers[index], matImages[index], bodyImages[index], distances[index]);
        if(viewersArray[index] != nullptr)
            delete viewersArray[index];
        viewersArray[index] = newViewer;

        ui->horizontalLayout_images->insertWidget(1,newViewer,1);

        //Set PenRed viewer to QT viewer
        newViewer->setViewer(penRedViewer);

        //Connect clicked event
        //QObject::connect(newViewer, SIGNAL(clicked(viewer*)), this, SLOT(on_viewerClicked(viewer*)));
        connect(newViewer, &viewer::clicked, this, &MainWindow::on_viewerClicked);
        //Connect load geometry signal
        connect(this, &MainWindow::geometryLoad, newViewer, &viewer::geometryLoad);
        //Connect viewer changed signal
        connect(newViewer, &viewer::changed, this, &MainWindow::on_viewerChanged);
        //Connect viewer zoom in 3D signal
        connect(newViewer, &viewer::zoomIn3D, this, &MainWindow::on_zoomIn3D);
        //Connect viewer zoom out 3D signal
        connect(newViewer, &viewer::zoomOut3D, this, &MainWindow::on_zoomOut3D);

        //Hide viewer
        newViewer->hide();
    }
}

void MainWindow::on_Xedit_editingFinished()
{
    double value = ui->Xedit->text().toDouble();
    ui->Xedit->setText(QString::number(value, 'e', 5));
    if(viewersArray[activeViewer] != nullptr){
        viewersArray[activeViewer]->setX(value);
        updateKey();
    }
}


void MainWindow::on_Yedit_editingFinished()
{
    double value = ui->Yedit->text().toDouble();
    ui->Yedit->setText(QString::number(value, 'e', 5));
    if(viewersArray[activeViewer] != nullptr){
        viewersArray[activeViewer]->setY(value);
        updateKey();
    }
}


void MainWindow::on_Zedit_editingFinished()
{
    double value = ui->Zedit->text().toDouble();
    ui->Zedit->setText(QString::number(value, 'e', 5));
    if(viewersArray[activeViewer] != nullptr){
        viewersArray[activeViewer]->setZ(value);
        updateKey();
    }
}

void MainWindow::on_matBodyViewButton_released()
{
    if(viewersArray[activeViewer] != nullptr){
        if(ui->matBodyViewButton->isChecked()){
            ui->matBodyViewButton->setText("Mat");
            viewersArray[activeViewer]->setMatView(true);
        }else{
            ui->matBodyViewButton->setText("Body");
            viewersArray[activeViewer]->setMatView(false);
        }
        updateKey();
    }
}


void MainWindow::on_perspectiveSelector_currentIndexChanged(int index)
{
    if(index >= 0 && index < 4){
        if(viewersArray[activeViewer] != nullptr){
            viewersArray[activeViewer]->setPerspective(index);
            updateKey();
        }
    }
}

void MainWindow::on_viewerClicked(viewer* pviewer){

    for(unsigned i = 0; i < maxViewers; ++i){
        if(pviewer == viewersArray[i]){
            if(activeViewer != i){
                setActiveViewer(i);
            }
            break;
        }
    }
}

void MainWindow::on_viewerChanged(viewer* pviewer){
    if(pviewer == viewersArray[activeViewer])
        updateViewerInfo();
}

void MainWindow::on_zoomIn3D(){
    pixelSize3D *= 0.9;
    if(pixelSize3D < 0.00001)
        pixelSize3D = 0.00001;

    ui->pixelSize3D->setValue(pixelSize3D);
    update3Dresolution();
}

void MainWindow::on_zoomOut3D(){
    pixelSize3D *= 1.1;
    ui->pixelSize3D->setValue(pixelSize3D);
    update3Dresolution();
}

void MainWindow::on_resolutionEditY_valueChanged(int arg1)
{
    if(viewersArray[activeViewer] != nullptr){
        viewersArray[activeViewer]->setImageHeight(arg1);
        updateKey();
    }
}


void MainWindow::on_resolutionEditX_valueChanged(int arg1)
{
    if(viewersArray[activeViewer] != nullptr){
        viewersArray[activeViewer]->setImageWidth(arg1);
        updateKey();
    }
}


void MainWindow::on_pixelSizeEdit_valueChanged(double arg1)
{
    if(viewersArray[activeViewer] != nullptr){
        viewersArray[activeViewer]->setPixelSize(arg1);
        updateKey();
    }
}


void MainWindow::on_testButton_released()
{
    ui->testOutput->setText("");
    if(viewersArray[activeViewer] != nullptr){

        QElapsedTimer timer;
        timer.start();

        std::vector<geoError> errors = viewersArray[activeViewer]->test();

        qint64 elapsed = timer.elapsed();

        if(errors.empty())
            ui->testOutput->setText(QString("Test completed in %1 milliseconds.\n No errors found at this plane\n").arg(elapsed));
        else{

            QString output;
            for(const auto& error : errors){
                char auxStr[500];
                sprintf(auxStr, "Error going from (%.5e,%.5e,%.5e) to (%.5e,%.5e,%.5e): \n"
                                "   - Initial body and material  : %3u %3u\n"
                                "   - Final body and material    : %3u %3u\n"
                                "   - Expected body and material : %3u %3u\n",
                        error.from[0],error.from[1],error.from[2],
                        error.to[0],error.to[1],error.to[2],
                        error.iIBODY, error.iMAT,
                        error.fIBODY, error.fMAT,
                        error.eIBODY, error.eMAT);
                output.append(auxStr);
            }
            ui->testOutput->setText(QString("Test completed in %1 milliseconds.\n\n%2").arg(elapsed)
                                                                                       .arg(output));
        }
    }
}


void MainWindow::on_lookX_editingFinished()
{
    double value = ui->lookX->text().toDouble();
    ui->lookX->setText(QString::number(value, 'e', 5));    
    if(viewersArray[activeViewer] != nullptr){
        updateKey();
    }
}


void MainWindow::on_lookY_editingFinished()
{
    double value = ui->lookY->text().toDouble();
    ui->lookY->setText(QString::number(value, 'e', 5));
    if(viewersArray[activeViewer] != nullptr){
        updateKey();
    }
}


void MainWindow::on_lookZ_editingFinished()
{
    double value = ui->lookZ->text().toDouble();
    ui->lookZ->setText(QString::number(value, 'e', 5));
    if(viewersArray[activeViewer] != nullptr){
        updateKey();
    }
}

void MainWindow::update3Dresolution(){

    if(penRedViewer != nullptr){
        penRedViewer->set3DResolution(width3D, height3D, pixelSize3D, pixelSize3D, 0.3490658503988659);
        for(auto& viewer : viewersArray)
            viewer->update3D(width3D,height3D,pixelSize3D);
    }
}

void MainWindow::on_pixelSize3D_valueChanged(double arg1)
{
    pixelSize3D = arg1;
    update3Dresolution();
}


void MainWindow::on_resolutionH3D_valueChanged(int arg1)
{
    width3D = arg1;
    update3Dresolution();
}


void MainWindow::on_resolutionV3D_valueChanged(int arg1)
{
    height3D = arg1;
    update3Dresolution();
}

void MainWindow::on_rhoEdit_editingFinished()
{
    double value = ui->rhoEdit->text().toDouble();
    ui->rhoEdit->setText(QString::number(value, 'e', 5));
    if(viewersArray[activeViewer] != nullptr){
        viewersArray[activeViewer]->setRho(value);
        updateKey();
    }
}


void MainWindow::on_thetaEdit_editingFinished()
{
    double value = std::clamp(ui->thetaEdit->text().toDouble(),0.0,pi);

    ui->thetaEdit->setText(QString::number(value, 'f', 5));
    if(viewersArray[activeViewer] != nullptr){
        viewersArray[activeViewer]->setTheta(value);
        updateKey();
    }
}


void MainWindow::on_phiEdit_editingFinished()
{
    double value = ui->phiEdit->text().toDouble();

    value -= static_cast<unsigned>(value/pi)*pi;

    ui->phiEdit->setText(QString::number(value, 'f', 5));
    if(viewersArray[activeViewer] != nullptr){
        viewersArray[activeViewer]->setPhi(value);
        updateKey();
    }
}


void MainWindow::on_actionconfig_triggered()
{
    loadConfigDialog.exec();
}


void MainWindow::on_actionQadric_triggered()
{
    loadQuadricDialog.exec();
}


void MainWindow::on_actionMesh_triggered()
{
    loadMeshDialog.exec();
}


void MainWindow::on_actionSave_triggered()
{
    saveDialog.exec();
}


void MainWindow::on_actionAdd_triggered()
{
    //Show next hided viewer
    if(nViewers < maxViewers){

        //Find first hide viewer
        for(size_t i = 0; i < maxViewers; ++i){
            if(!viewersArray[i]->isVisible()){
                //Show it
                viewersArray[i]->show();
                //Try to copy from selected viewer
                if(viewersArray[activeViewer] != nullptr){
                    viewersArray[i]->copy(*(viewersArray[activeViewer]));
                }
                break;
            }
        }

        //Increase number of active viewers
        nViewers++;
    }
}


void MainWindow::on_actionDelete_triggered()
{
    if(nViewers > 1){

        //Hide viewer
        viewersArray[activeViewer]->hide();
        //Reset active viewer to first non hide viewer
        for(size_t i = 0; i < maxViewers; ++i)
            if(viewersArray[i]->isVisible())
                setActiveViewer(i);

        //Decrease number of viewers
        --nViewers;
    }
}


void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

