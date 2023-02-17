#include "viewer.h"

std::array<unsigned char, viewer::nColorsPos> viewer::colors;

viewer::viewer(std::vector<uchar>& bufferIn,
               std::vector<unsigned char>& matImageIn,
               std::vector<unsigned int>& bodyImageIn,
               std::vector<float>& distancesIn,
               QWidget *parent)
    : QWidget{parent}, buffer(bufferIn), matImage(matImageIn), bodyImage(bodyImageIn), distances(distancesIn),
      x(0.0), y(0.0), z(0.0), xlast(0.0), ylast(0.0), zlast(0.0), camera3DX(0.0), camera3DY(0.0), camera3DZ(0.0),
      u(0.0), v(0.0), w(1.0), rho(10.0), theta(1.5707963267948966), phi(0.0), omega(-1.5707963267948966), lastRender3DPhi(0.0),
      perspective(0), matView(true), pixelSize(0.1), pixelSize3D(0.1), pPenRedViewer(nullptr), geometryLoaded(false)
{

    //Calculate the number of threads
    nthreads = std::thread::hardware_concurrency();
    if(nthreads % 2 == 0)
        nthreads /= 2;
    else
        nthreads = nthreads/2 + 1;

    //Configure the label
    label.setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    label.setMinimumSize(10,10); //Enable resizing the label itself to small size
    label.setTextInteractionFlags(Qt::TextSelectableByMouse);
    label.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    label.setTextInteractionFlags(Qt::TextSelectableByMouse);

    //Set the main layout
    QVBoxLayout* widgetLayout = new QVBoxLayout;
    setLayout(widgetLayout);
    //Add the label
    layout()->addWidget(&label);

    //Create the base image
    imageWidth = 600;
    imageHeight = 600;
    image3DWidth = 400;
    image3DHeight = 400;


    for(size_t i = 0; i < imageWidth*imageHeight; ++i){
        buffer[i*3  ] = 0;   //Red
        buffer[i*3+1] = 0;   //Green
        buffer[i*3+2] = 0;   //Blue
    }

    //Create the image
    image = QImage(buffer.data(), imageWidth, imageHeight, imageWidth*3, QImage::Format_RGB888);

    //Create a pixel map from image
    pixMap = QPixmap::fromImage(image);

    //Set the pixmap in the label scaling int
    resizeImage();
}

std::array<unsigned char, viewer::nColorsPos> viewer::defaultColors(){

    const unsigned int maxIntensity = 250;
    const unsigned int baseIncrementPerRow = 100;
    const unsigned int baseSteps = 2;
    const unsigned int stepIncrement = baseIncrementPerRow/baseSteps;
    const unsigned int colorsPerRow = 7*baseSteps;
    const unsigned int nRows = 1 + nColors / colorsPerRow;

    std::array<unsigned char, nColorsPos> a{};

    size_t icolor = 0;
    for(size_t irow = 0; irow < nRows ; ++irow){
        //Init RGB
        unsigned int R = baseIncrementPerRow*irow;
        unsigned int G = baseIncrementPerRow*irow;
        unsigned int B = baseIncrementPerRow*irow;

        size_t colorIndex = 3*icolor++;
        a[colorIndex  ] = R;
        a[colorIndex+1] = G;
        a[colorIndex+2] = B;

        //R increase
        for(size_t istep = 0; istep < baseSteps; ++istep){
            R += stepIncrement;
            colorIndex = 3*icolor++;
            if(icolor < nColors){
                a[colorIndex  ] = R;
                a[colorIndex+1] = G;
                a[colorIndex+2] = B;
            }
        }

        //G increase
        for(size_t istep = 0; istep < baseSteps; ++istep){
            G += stepIncrement;
            colorIndex = 3*icolor++;
            if(icolor < nColors){
                a[colorIndex  ] = R;
                a[colorIndex+1] = G;
                a[colorIndex+2] = B;
            }
        }

        //R decrease
        for(size_t istep = 0; istep < baseSteps; ++istep){
            R -= stepIncrement;
            colorIndex = 3*icolor++;
            if(icolor < nColors){
                a[colorIndex  ] = R;
                a[colorIndex+1] = G;
                a[colorIndex+2] = B;
            }
        }

        //B increase
        for(size_t istep = 0; istep < baseSteps; ++istep){
            B += stepIncrement;
            colorIndex = 3*icolor++;
            if(icolor < nColors){
                a[colorIndex  ] = R;
                a[colorIndex+1] = G;
                a[colorIndex+2] = B;
            }
        }

        //G decrease
        for(size_t istep = 0; istep < baseSteps; ++istep){
            G -= stepIncrement;
            colorIndex = 3*icolor++;
            if(icolor < nColors){
                a[colorIndex  ] = R;
                a[colorIndex+1] = G;
                a[colorIndex+2] = B;
            }
        }

        //R increase
        for(size_t istep = 0; istep < baseSteps; ++istep){
            R += stepIncrement;
            colorIndex = 3*icolor++;
            if(icolor < nColors){
                a[colorIndex  ] = R;
                a[colorIndex+1] = G;
                a[colorIndex+2] = B;
            }
        }

        //B partial decrease
        for(size_t istep = 0; istep < baseSteps-1; ++istep){
            B -= stepIncrement;
            colorIndex = 3*icolor++;
            if(icolor < nColors){
                a[colorIndex  ] = R;
                a[colorIndex+1] = G;
                a[colorIndex+2] = B;
            }
        }
    }

    return a;
}

void viewer::resetColors(){
    viewer::colors = defaultColors();
}

void viewer::copy(const viewer& viewer2copy){
    imageWidth = viewer2copy.imageWidth;
    imageHeight = viewer2copy.imageHeight;

    //Copy material, body and distances buffers
    matImage = viewer2copy.matImage;
    bodyImage = viewer2copy.bodyImage;
    distances = viewer2copy.distances;

    //Copy buffer data
    buffer = viewer2copy.buffer;
    //Create the image
    image = QImage(buffer.data(), imageWidth, imageHeight, imageWidth*3, QImage::Format_RGB888);

    //Create a pixel map from image
    pixMap = QPixmap::fromImage(image);

    //Copy position
    x = viewer2copy.x;
    y = viewer2copy.y;
    z = viewer2copy.z;

    //Copy last position
    xlast = viewer2copy.xlast;
    ylast = viewer2copy.ylast;
    zlast = viewer2copy.zlast;

    //Copy look at position
    camera3DX = viewer2copy.camera3DX;
    camera3DY = viewer2copy.camera3DY;
    camera3DZ = viewer2copy.camera3DZ;

    //Copy 3D direction
    u = viewer2copy.u;
    v = viewer2copy.v;
    w = viewer2copy.w;

    //Copy 3D spherical coordinates
    rho   = viewer2copy.rho;
    theta = viewer2copy.theta;
    phi   = viewer2copy.phi;

    //Copy perspective
    perspective = viewer2copy.perspective;

    //Copy material/body view type
    matView = viewer2copy.matView;

    //Copy pixel size
    pixelSize = viewer2copy.pixelSize;

    //Copy penred render
    pPenRedViewer = viewer2copy.pPenRedViewer;

    //Copy key text
    keyText = viewer2copy.keyText;

    //Copy geometry loaded flag
    geometryLoaded = viewer2copy.geometryLoaded;

    //Set the pixmap in the label scaling it
    resizeImage();

}

void viewer::geometryLoad(){
    geometryLoaded = true;
    render();
    emit changed(this);
}

void viewer::render(bool moveOnPlane, unsigned char direction, unsigned nPixels){

    // direction -> On move on plane renders, 0, 1, 2, 3 means left, right, up and down respectivelly

    if(pPenRedViewer != nullptr && geometryLoaded){
        if(perspective == 0){
            if(moveOnPlane){
                switch(direction){
                    case 0:
                        pPenRedViewer->renderXtoLeft(matImage.data(), bodyImage.data(), nPixels,
                                                     xlast, ylast, zlast, pixelSize, pixelSize, imageWidth, imageHeight);
                        break;
                    case 1:
                        pPenRedViewer->renderXtoRight(matImage.data(), bodyImage.data(), nPixels,
                                                      xlast, ylast, zlast, pixelSize, pixelSize, imageWidth, imageHeight);
                        break;
                    case 2:
                        pPenRedViewer->renderXtoUp(matImage.data(), bodyImage.data(), nPixels,
                                                     xlast, ylast, zlast, pixelSize, pixelSize, imageWidth, imageHeight);
                        break;
                    case 3:
                        pPenRedViewer->renderXtoDown(matImage.data(), bodyImage.data(), nPixels,
                                                     xlast, ylast, zlast, pixelSize, pixelSize, imageWidth, imageHeight);
                        break;
                }
            }else{
                pPenRedViewer->renderX(matImage.data(), bodyImage.data(),
                                       x,y,z, pixelSize, pixelSize, imageWidth, imageHeight, nthreads);
            }
        }else if(perspective == 1){
            if(moveOnPlane){
                switch(direction){
                    case 0:
                        pPenRedViewer->renderYtoLeft(matImage.data(), bodyImage.data(), nPixels,
                                                     xlast, ylast, zlast, pixelSize, pixelSize, imageWidth, imageHeight);
                        break;
                    case 1:
                        pPenRedViewer->renderYtoRight(matImage.data(), bodyImage.data(), nPixels,
                                                      xlast, ylast, zlast, pixelSize, pixelSize, imageWidth, imageHeight);
                        break;
                    case 2:
                        pPenRedViewer->renderYtoUp(matImage.data(), bodyImage.data(), nPixels,
                                                     xlast, ylast, zlast, pixelSize, pixelSize, imageWidth, imageHeight);
                        break;
                    case 3:
                        pPenRedViewer->renderYtoDown(matImage.data(), bodyImage.data(), nPixels,
                                                     xlast, ylast, zlast, pixelSize, pixelSize, imageWidth, imageHeight);
                        break;
                }
            }else{
                pPenRedViewer->renderY(matImage.data(), bodyImage.data(),
                                       x,y,z, pixelSize, pixelSize, imageWidth, imageHeight, nthreads);
            }
        }else if(perspective == 2){
            if(moveOnPlane){
                switch(direction){
                    case 0:
                        pPenRedViewer->renderZtoLeft(matImage.data(), bodyImage.data(), nPixels,
                                                     xlast, ylast, zlast, pixelSize, pixelSize, imageWidth, imageHeight);
                        break;
                    case 1:
                        pPenRedViewer->renderZtoRight(matImage.data(), bodyImage.data(), nPixels,
                                                      xlast, ylast, zlast, pixelSize, pixelSize, imageWidth, imageHeight);
                        break;
                    case 2:
                        pPenRedViewer->renderZtoUp(matImage.data(), bodyImage.data(), nPixels,
                                                     xlast, ylast, zlast, pixelSize, pixelSize, imageWidth, imageHeight);
                        break;
                    case 3:
                        pPenRedViewer->renderZtoDown(matImage.data(), bodyImage.data(), nPixels,
                                                     xlast, ylast, zlast, pixelSize, pixelSize, imageWidth, imageHeight);
                        break;
                }
            }else{
                pPenRedViewer->renderZ(matImage.data(), bodyImage.data(),
                                       x,y,z, pixelSize, pixelSize, imageWidth, imageHeight, nthreads);
            }
        }else if(perspective == 3){

            pPenRedViewer->render3D(matImage.data(), bodyImage.data(),
                                    camera3DX, camera3DY, camera3DZ, u, v, w, omega, lastRender3DPhi, distances.data(), minD, maxD);
        }

        xlast = x;
        ylast = y;
        zlast = z;
        updateMatView();
    }
}

void viewer::updateMatView(){

    //Check if geometry has been loaded
    if(!geometryLoaded)
        return;

    //Set the image in the label
    std::array<bool,nColors> visibleColors{false};

    const char* matString  = " Material ";
    const char* bodyString = "   Body   ";
    const char* pMatBody = matString;

    unsigned int renderWidth;
    unsigned int renderHeight;
    if(perspective == 3){ //3D
        renderWidth = image3DWidth;
        renderHeight= image3DHeight;
    }else{
        renderWidth = imageWidth;
        renderHeight= imageHeight;
    }

    const unsigned int nRenderPixels = renderWidth*renderHeight;

    if(perspective != 3){ //No 3D
        if(matView){
            for(size_t i = 0; i < nRenderPixels; ++i){
                size_t index = i*3;
                unsigned imat = matImage[i];
                unsigned icolor = 3*imat;
                if(imat < nColors){
                    buffer[index  ] = colors[icolor  ];
                    buffer[index+1] = colors[icolor+1];
                    buffer[index+2] = colors[icolor+2];

                    //Flag this material as visible
                    visibleColors[imat] = true;
                }else{
                    //Out of range, set it to white
                    buffer[index  ] = 255;
                    buffer[index+1] = 255;
                    buffer[index+2] = 255;
                }
            }
        }else{
            pMatBody = bodyString;
            for(size_t i = 0; i < nRenderPixels; ++i){
                size_t index = i*3;
                unsigned ibody = bodyImage[i];
                unsigned icolor = 3*ibody;
                if(ibody < nColors){
                    buffer[index  ] = colors[icolor  ];
                    buffer[index+1] = colors[icolor+1];
                    buffer[index+2] = colors[icolor+2];

                    //Flag this body as visible
                    visibleColors[ibody] = true;
                }else{
                    //Out of range, set it to white
                    buffer[index  ] = 255;
                    buffer[index+1] = 255;
                    buffer[index+2] = 255;
                }
            }
        }
    }else{ //3D case
        float distInterval = (maxD-minD);
        if(matView){
            for(size_t i = 0; i < nRenderPixels; ++i){
                size_t index = i*3;
                unsigned imat = matImage[i];
                unsigned icolor = 3*imat;
                float beyondFact = (distances[i]-minD)/distInterval;
                float distanceCorrection = 1.0/(1.0 + 1.1*beyondFact);
                if(imat < nColors){
                    buffer[index  ] = colors[icolor  ]*distanceCorrection;
                    buffer[index+1] = colors[icolor+1]*distanceCorrection;
                    buffer[index+2] = colors[icolor+2]*distanceCorrection;

                    //Flag this material as visible
                    visibleColors[imat] = true;
                }else{
                    //Out of range, set it to white
                    buffer[index  ] = 255;
                    buffer[index+1] = 255;
                    buffer[index+2] = 255;
                }
            }
        }else{
            pMatBody = bodyString;
            for(size_t i = 0; i < nRenderPixels; ++i){
                size_t index = i*3;
                unsigned ibody = bodyImage[i];
                unsigned icolor = 3*ibody;
                float beyondFact = (distances[i]-minD)/distInterval;
                float distanceCorrection = 1.2/(1.0 + 2.0*beyondFact);
                if(ibody < nColors){
                    buffer[index  ] = colors[icolor  ]*distanceCorrection;
                    buffer[index+1] = colors[icolor+1]*distanceCorrection;
                    buffer[index+2] = colors[icolor+2]*distanceCorrection;

                    //Flag this body as visible
                    visibleColors[ibody] = true;
                }else{
                    //Out of range, set it to white
                    buffer[index  ] = 255;
                    buffer[index+1] = 255;
                    buffer[index+2] = 255;
                }
            }
        }
    }

    //Fill key text with the corresponding colors
    keyText = QString("<table>\n <tr>");
    size_t included = 0;
    for(size_t i = 0; i < viewer::nColors; ++i){

        if(visibleColors[i]){
            if(included % 3 == 0 && included > 0){
                keyText.append(" </tr>\n<tr>");
            }
            size_t index = i*3;
            keyText.append(std::string("<th style=\"color:rgb(" + std::to_string(viewer::colors[index]) + "," + std::to_string(viewer::colors[index+1]) + "," + std::to_string(viewer::colors[index+2]) + ")\"> " + pMatBody + std::to_string(i) + " </th>\n").c_str());
            ++included;
        }
    }
    keyText.append(" </tr>\n</table>");

    //Create the image
    image = QImage(buffer.data(), renderWidth, renderHeight, renderWidth*3, QImage::Format_RGB888);

    //Create a pixel map from image
    pixMap = QPixmap::fromImage(image);

    resizeImage();
}

void viewer::resizeImage(){

    //Create a resized pixel map
    QPixmap scaledPixMap = pixMap.scaled(label.width(), label.height(), Qt::KeepAspectRatioByExpanding);

    if(perspective != 3){ //No 3D
        //Add the coordinate system

        size_t midH = scaledPixMap.width()/2;
        size_t midV = scaledPixMap.height()/2;

        QPainter painter(&scaledPixMap);
        painter.setPen(QPen(Qt::white, std::min(scaledPixMap.width(),scaledPixMap.height())/2000.0, Qt::DashLine));
        painter.setOpacity(0.8);
        painter.drawLine(0,midV,scaledPixMap.width(),midV);
        painter.drawLine(midH,0,midH,scaledPixMap.height());
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::TextAntialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

        if(perspective == 0){ //X
            painter.drawText(QPoint(scaledPixMap.width()*0.9,midV-2), "Y");
            painter.drawText(QPoint(midH+2, scaledPixMap.height()*0.1), "Z");
        }
        else if(perspective == 1){ //Y
            painter.drawText(QPoint(scaledPixMap.width()*0.9,midV-2), "X");
            painter.drawText(QPoint(midH+2, scaledPixMap.height()*0.1), "Z");
        }
        else if(perspective == 2){ //Z
            painter.drawText(QPoint(scaledPixMap.width()*0.9,midV-2), "X");
            painter.drawText(QPoint(midH+2, scaledPixMap.height()*0.1), "Y");
        }
    }

    //Set the pixmap in the label scaling int
    label.setPixmap(scaledPixMap);
}

std::vector<geoError> viewer::test() const{

    std::vector<geoError> errors;
    if(pPenRedViewer != nullptr && geometryLoaded){
        if(perspective == 0){
            pPenRedViewer->testX(errors,
                                   x,y,z, pixelSize, pixelSize, imageWidth, imageHeight);
        }else if(perspective == 1){
            pPenRedViewer->testY(errors,
                                   x,y,z, pixelSize, pixelSize, imageWidth, imageHeight);
        }else if(perspective == 2){
            pPenRedViewer->testZ(errors,
                                   x,y,z, pixelSize, pixelSize, imageWidth, imageHeight);

        }else if(perspective == 3){
            //TODO
        }
    }
    return errors;
}

void viewer::update3D(unsigned width, unsigned height, double pixSize){
    image3DWidth = width;
    image3DHeight = height;
    pixelSize3D = pixSize;
    if(perspective == 3){ //3D
        render();
    }
}

//Setter functions

void viewer::setViewer(const pen_geoViewInterface* p, const bool _geometryLoaded){
    geometryLoaded = _geometryLoaded;
    pPenRedViewer = p;
}

void viewer::setImageWidth(unsigned width){
    imageWidth = width;
    if(perspective != 3) // not 3D
        render();
}
void viewer::setImageHeight(unsigned height){
    imageHeight = height;
    if(perspective != 3) // not 3D
        render();
}

void viewer::setX(double newX){
    x = newX;
    render();
}
void viewer::setY(double newY){
    y = newY;
    render();
}
void viewer::setZ(double newZ){
    z = newZ;
    render();
}

void viewer::update3Ddirections(){

    //Obtain x,y,z position
    const double cphi = cos(phi);
    const double sphi = sin(phi);
    const double stheta = sin(theta);

    camera3DX = rho*cphi*stheta;
    camera3DY = rho*sphi*stheta;
    camera3DZ = rho*cos(theta);

    //Obtain rotation matrix
    u = x-camera3DX;
    v = y-camera3DY;
    w = z-camera3DZ;

    double norm = sqrt(u*u + v*v + z*z);
    u /= norm;
    v /= norm;
    w /= norm;

    emit changed(this);
}

void viewer::setRho(double newRho){
    rho = newRho;
    update3Ddirections();
    if(perspective == 3) //3D
        render();
}
void viewer::setTheta(double newTheta){
    theta = newTheta;
    update3Ddirections();
    if(perspective == 3) //3D
        render();
}
void viewer::setPhi(double newPhi){
    phi = newPhi;
    update3Ddirections();
    if(perspective == 3) //3D
        render();
}

void viewer::setPerspective(unsigned index){
    perspective = index;
    if(perspective == 3) //3D
        update3Ddirections();
    render();
}
void viewer::setMatView(bool enabled){
    matView = enabled;
    updateMatView();
}
void viewer::setPixelSize(double newPixelSize){
    pixelSize = newPixelSize;
    if(perspective != 3) // not 3D
        render();
}

void viewer::resizeEvent(QResizeEvent *){
    //Handle the viewer resize event
    resizeImage();
}

void viewer::mousePressEvent(QMouseEvent* event) {
    emit clicked(this);
}

void viewer::keyPressEvent(QKeyEvent *event){
    bool known = true;
    bool needRender = true;
    bool moveOnPlane = false;
    unsigned char direction = 0;
    unsigned nPixels = 0;

    const unsigned dPixels = 10;
    const double d = static_cast<double>(dPixels)*pixelSize;
    const double d3D = static_cast<double>(dPixels)*pixelSize3D;
    const double dangle3D = 0.1;

    switch(event->key()){
        case Qt::Key_Up:
            if(perspective == 3){
                //Move theta Up
                theta -= dangle3D;
                if(theta < 0.1)
                    theta = 0.1;
                update3Ddirections();
                break;
            }
        case Qt::Key_W:
            if(perspective == 0)
                z += d;
            else if(perspective == 1)
                z += d;
            else if(perspective == 2)
                y += d;
            else if(perspective == 3){ //3D
                //Movement look at point Up
                z += d3D;
                update3Ddirections();
            }

            if(perspective != 3){ //not 3D
                moveOnPlane = true; //Adaptative render
                direction = 2; //Up
                nPixels = dPixels; //Move 10 pixels
            }

            break;
        case Qt::Key_Down:
            if(perspective == 3){
                //Move theta down
                theta += dangle3D;
                if(theta > 3.141592653589793-0.1){
                    theta = 3.141592653589793-0.1;
                }
                update3Ddirections();
                break;
            }
        case Qt::Key_S:
            if(perspective == 0)
                z -= d;
            else if(perspective == 1)
                z -= d;
            else if(perspective == 2)
                y -= d;
            else if(perspective == 3){
                //Movement look at point Down
                z -= d3D;
                update3Ddirections();
            }

            if(perspective != 3){ //not 3D
                moveOnPlane = true; //Adaptative render
                direction = 3; //Down
                nPixels = dPixels; //Move 10 pixels
            }

            break;
        case Qt::Key_Left:
            if(perspective == 3){
                //Move phi on positive direction
                phi += dangle3D;
                if(phi > 6.283185307179586){
                    phi -= 6.283185307179586;
                }
                update3Ddirections();
                break;
            }
        case Qt::Key_A:
            if(perspective == 0)
                y -= d;
            else if(perspective == 1)
                x -= d;
            else if(perspective == 2)
                x -= d;
            else if(perspective == 3){
                //Movement look at point Left
                y -= d3D;
                update3Ddirections();
            }

            if(perspective != 3){ //not 3D
                moveOnPlane = true; //Adaptative render
                direction = 0; //Left
                nPixels = dPixels; //Move 10 pixels
            }

            break;
        case Qt::Key_Right:
        if(perspective == 3){
            //Move phi to negative
            phi -= dangle3D;
            if(phi < 0.0){
                phi += 6.283185307179586;
            }
            update3Ddirections();
            break;
        }
        case Qt::Key_D:
            if(perspective == 0)
                y += d;
            else if(perspective == 1)
                x += d;
            else if(perspective == 2)
                x += d;
            else if(perspective == 3){
                //Movement look at point Right
                y += d3D;
                update3Ddirections();
            }

            if(perspective != 3){ //not 3D
                moveOnPlane = true; //Adaptative render
                direction = 1; //Right
                nPixels = dPixels; //Move 10 pixels
            }
            break;
        case Qt::Key_F: //Forward
            if(perspective == 0)
                x += d;
            else if(perspective == 1)
                y += d;
            else if(perspective == 2)
                z += d;
            else if(perspective == 3){ // 3D
                //Reduce radius
                rho -= d3D;
                if(rho < 1.0){
                    rho = 1.0;
                }
                update3Ddirections();
            }
            break;
        case Qt::Key_B: //Backward
            if(perspective == 0)
                x -= d;
            else if(perspective == 1)
                y -= d;
            else if(perspective == 2)
                z -= d;
            else if(perspective == 3){ // 3D
                //Increase radius
                rho += d3D;
                update3Ddirections();
            }
            break;
        case Qt::Key_Plus: // zoom in
            if(perspective == 3){ //3D
                known = false;
                emit zoomIn3D();
            }else{
                pixelSize *= 0.9;
                if(pixelSize < 0.00001)
                    pixelSize = 0.00001;
            }
            break;
        case Qt::Key_Minus:  // zoom out
            if(perspective == 3){ //3D
                known = false;
                emit zoomOut3D();
            }else{
                pixelSize *= 1.1;
            }
            break;
        case Qt::Key_X: //Change perspective to X
            if(perspective == 0)
                known = false;
            else
                perspective = 0;
            break;
        case Qt::Key_Y: //Change perspective to Y
            if(perspective == 1)
                known = false;
            else
                perspective = 1;
            break;
        case Qt::Key_Z: //Change perspective to Z
            if(perspective == 2)
                known = false;
            else
                perspective = 2;
            break;
        case Qt::Key_M: //Change between material and body view
            matView = !matView;
            updateMatView();
            needRender = false;
            break;
        default:
            known = false;
            break;
    }
    if(known){
        if(needRender)
            render(moveOnPlane,direction,nPixels);
        emit changed(this);
    }
}
