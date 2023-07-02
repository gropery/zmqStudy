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

    qDebug()<<"Connecting to hello world server......\r\n";
    void *context = zmq_ctx_new ();
    void *requester = zmq_socket (context, ZMQ_REQ);
    int rc = zmq_connect (requester, "tcp://localhost:5555");
    assert (rc == 0);
    if(rc)
        qDebug()<<"rc!=0";
    else
        qDebug()<<"connect ok";

    int request_nbr;
    for (request_nbr = 0; request_nbr != 10; request_nbr++) {
        char buffer [10];
        qDebug()<<"Sending Hello ..."<<request_nbr;
        zmq_send (requester, "Hello", 5, 0);
        zmq_recv (requester, buffer, 10, 0);
        qDebug()<<"Received World "<<request_nbr;
    }
    zmq_close (requester);
    zmq_ctx_destroy (context);
    return 0;

}
