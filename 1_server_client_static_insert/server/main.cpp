#include "mainwindow.h"
#include <QApplication>

#include "libs\windows\include/zmq.h"
#include <string.h>
#include <stdio.h>
#include <QDebug>

int main(int argc, char *argv[])
{
//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();
//    return a.exec();

    int major, minor, patch;
    zmq_version (&major, &minor, &patch);
    qDebug()<<"VERSION:"<< major<<minor<<patch;

    //  Socket to talk to clients
    qDebug()<<"binding\r\n";
    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);
    int rc = zmq_bind (responder, "tcp://*:5555");
    assert (rc == 0);
    if(rc)
        qDebug()<<"rc!=0";
    else
        qDebug()<<"bind ok";

    while (1) {
        char buffer [10];
        zmq_recv (responder, buffer, 10, 0);
        qDebug()<<"Received Hello\n";
//        sleep (1);          //  Do some 'work'
        zmq_send (responder, "World", 5, 0);
    }
    return 0;
}
