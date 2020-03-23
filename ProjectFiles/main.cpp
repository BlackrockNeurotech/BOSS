#include "mainwindow.h"
#include <QApplication>
#include "../ProjectFiles/mysplasscreen.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationDomain("Blackrock");
    QCoreApplication::setOrganizationName("Blackrock Microsystems");
    QCoreApplication::setApplicationName("Blackrock Offline Spike Sorter");
    QCoreApplication::setApplicationVersion("1.2.0");
    

    MainWindow w;
    w.showWindow();


    
    
    return a.exec();
}
