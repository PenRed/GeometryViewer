#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <cstdio>
#include <vector>
#include <array>
#include <stdexcept>
#include <algorithm>
#include <QMainWindow>
#include <QLibrary>
#include <QElapsedTimer>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMenuBar>
#include <QMenu>
#include <QDialogButtonBox>
#include <QColorDialog>
#include <QFuture>
#include <QtConcurrent>
#include <QMessageBox>
#include <QProgressBar>
#include <QProgressDialog>
#include "viewer.h"
#include "pen_geoViewInterface.hh"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static constexpr double pi = 3.141592653589793;
    static const unsigned maxViewers = 4;
    std::array<viewer*,maxViewers> viewersArray{nullptr};

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_viewerClicked(viewer*);

    void on_viewerChanged(viewer*);

    void on_saveImage(const QString &file);

    void on_loadConfig(const QString &file);

    void on_loadQuadric(const QString &file);

    void on_loadMesh(const QString &file);

    void on_zoomIn3D();

    void on_zoomOut3D();

    void on_Xedit_editingFinished();

    void on_Yedit_editingFinished();

    void on_Zedit_editingFinished();

    void on_matBodyViewButton_released();

    void on_perspectiveSelector_currentIndexChanged(int index);

    void on_resolutionEditY_valueChanged(int arg1);

    void on_resolutionEditX_valueChanged(int arg1);

    void on_pixelSizeEdit_valueChanged(double arg1);

    void on_testButton_released();

    void on_lookX_editingFinished();

    void on_lookY_editingFinished();

    void on_lookZ_editingFinished();

    void on_pixelSize3D_valueChanged(double arg1);

    void on_resolutionH3D_valueChanged(int arg1);

    void on_resolutionV3D_valueChanged(int arg1);

    void on_rhoEdit_editingFinished();

    void on_thetaEdit_editingFinished();

    void on_phiEdit_editingFinished();

    void on_actionconfig_triggered();

    void on_actionQadric_triggered();

    void on_actionMesh_triggered();

    void on_actionSave_triggered();

    void on_actionAdd_triggered();

    void on_actionDelete_triggered();

    void on_actionExit_triggered();

    void on_actionColors_triggered();

signals:

    void geometryLoad();

private:

    std::array<std::vector<uchar>,maxViewers> buffers;
    std::array<std::vector<unsigned char>,maxViewers> matImages;
    std::array<std::vector<unsigned int>,maxViewers> bodyImages;
    std::array<std::vector<float>,maxViewers> distances;

    pen_geoViewInterface* penRedViewer;
    QLibrary viewerLib;

    QFileDialog saveDialog;
    QFileDialog loadConfigDialog;
    QFileDialog loadQuadricDialog;
    QFileDialog loadMeshDialog;

    QDialog* colorsDialog;
    QVBoxLayout* colorListLayout;
    QColorDialog* selectColorDialog;

    typedef pen_geoViewInterface* (*viewerConstructor)();
    viewerConstructor constructViewer;
    typedef void (*viewerDestructor)(pen_geoViewInterface*);
    viewerDestructor destroyViewer;

    unsigned nViewers;
    unsigned activeViewer;

    unsigned width3D, height3D;
    double pixelSize3D;

    Ui::MainWindow *ui;

    void setActiveViewer(unsigned index);
    void updateViewerInfo();
    void updateKey();
    void createViewer(const size_t index);
    void update3Dresolution();
    void changeViewerColors();

    inline void resetViewerColors(){
        viewer::resetColors();
        saveViewerColors("colors.conf");
        loadViewerColors("colors.conf");
    }
    inline void saveConfigColors(){
        saveViewerColors("colors.conf");
    }
    inline void loadConfigColors(){
        loadViewerColors("colors.conf");
    }
    void loadViewerColors(const QString &file);
    void saveViewerColors(const QString &file);

};
#endif // MAINWINDOW_H
