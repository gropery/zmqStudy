#include "mainwindow.h"

#include <QApplication>
#include <libs/windows/include/zmq.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    int major, minor, patch;
        zmq_version(&major, &minor, &patch);
        printf("Current ZeroMQ version is %d.%d.%d\n", major, minor, patch);
    MainWindow w;
    w.show();
    return a.exec();
}
