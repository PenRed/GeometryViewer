#ifndef VIEWER_H
#define VIEWER_H

#include <cmath>
#include <thread>
#include <algorithm>
#include <vector>
#include <array>
#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QImage>
#include <QColor>
#include <QPushButton>
#include <QKeyEvent>
#include <QPainter>

#include "pen_geoViewInterface.hh"

class viewer : public QWidget
{
    Q_OBJECT

public:

    static const size_t nColors = 60;
    static const size_t nColorsPos = nColors*3;
    static const constexpr std::array<unsigned char, nColorsPos> colors = []{

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
    }();

    static const size_t maxWidth = 2000;
    static const size_t maxHeight = 2000;
    static constexpr size_t maxPixels = maxWidth*maxHeight;

private:

    static constexpr double rot3Dtheta = 5.0;
    static constexpr double ctheta = 0.9961946980917455;
    static constexpr double stheta = 0.08715574274765817;

    std::vector<uchar>& buffer;
    std::vector<unsigned char>& matImage;
    std::vector<unsigned int>& bodyImage;
    std::vector<float>& distances;
    float minD, maxD;

    unsigned int imageWidth;    //2D
    unsigned int imageHeight;   //2D
    unsigned int image3DWidth;  //3D
    unsigned int image3DHeight; //3D
    QImage image;
    QLabel label;
    QPixmap pixMap;

    double x, y, z;
    double xlast, ylast, zlast; //Last x,y,z values
    //3D camera position perspective
    double camera3DX, camera3DY, camera3DZ;
    //3D direction vector between look at point and position
    double u, v, w;
    //Spherical coordinates for 3D view
    double rho,theta,phi;
    //Roll angle
    double omega;
    //Save last render phi angle (3D only)
    float lastRender3DPhi;

    unsigned perspective; // x,y,z,3d -> 0,1,2,3
    bool matView;     //True -> Material view, False -> Body view
    double pixelSize; //in cm
    double pixelSize3D; //in cm

    const pen_geoViewInterface* pPenRedViewer;

    QString keyText; //HTML text with key table

    bool geometryLoaded;

    unsigned nthreads;

    void update3Ddirections();

protected:
    void mousePressEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent *event);

public:

    explicit viewer(std::vector<uchar>& bufferIn,
                    std::vector<unsigned char>& matImageIn,
                    std::vector<unsigned int>& bodyImage,
                    std::vector<float>& distancesIn,
                    QWidget *parent = nullptr);

    void copy(const viewer& viewer2copy);

    void render(bool moveOnPlane = false, unsigned char direction = 0, unsigned nPixels = 0);
    void resizeImage();
    void updateMatView();
    std::vector<geoError> test() const;

    //Getter functions
    constexpr const QImage& readImage() const {return image;}

    constexpr unsigned readImageWidth() const {return imageWidth;}
    constexpr unsigned readImageHeight() const {return imageHeight;}

    constexpr double readX() const {return x;}
    constexpr double readY() const {return y;}
    constexpr double readZ() const {return z;}

    constexpr double readRho() const {return rho;}
    constexpr double readTheta() const {return theta;}
    constexpr double readPhi() const {return phi;}

    constexpr double readCamera3DX() const {return camera3DX;}
    constexpr double readCamera3DY() const {return camera3DY;}
    constexpr double readCamera3DZ() const {return camera3DZ;}

    constexpr double readU() const {return u;}
    constexpr double readV() const {return v;}
    constexpr double readW() const {return w;}

    constexpr unsigned readPerspective() const {return perspective;}
    constexpr bool readMatView() const {return matView;}
    constexpr double readPixelSize() const {return pixelSize;}

    constexpr const QString& readKeyText() const {return keyText;}

    //Setter functions

    void setViewer(const pen_geoViewInterface* p, const bool _geometryLoaded = false);

    void setImageWidth(unsigned width);
    void setImageHeight(unsigned height);

    void setX(double newX);
    void setY(double newY);
    void setZ(double newZ);

    void setRho(double newRho);
    void setTheta(double newTheta);
    void setPhi(double newPhi);

    void setPerspective(unsigned index);
    void setMatView(bool enabled);
    void setPixelSize(double newPixelSize);

    void update3D(unsigned width, unsigned height, double pixSize);

public slots:
    void resizeEvent(QResizeEvent *);

    void geometryLoad();

signals:
    void clicked(viewer*);
    void changed(viewer*);
    void zoomIn3D();
    void zoomOut3D();
};

#endif // VIEWER_H
