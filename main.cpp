#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //
    QDesktopWidget wd;
    QRect mainScreenSize = wd.availableGeometry(wd.primaryScreen()); // or screenGeometry(), depending on your needs
    w.setGeometry(mainScreenSize);
    //
    w.show();
    return a.exec();
}
