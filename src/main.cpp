#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //Set number formatting to standard C like
    std::setlocale(LC_NUMERIC, "C");

    MainWindow w;
    w.show();
    return a.exec();
}
