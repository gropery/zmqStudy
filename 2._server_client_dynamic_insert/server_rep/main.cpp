#include <QCoreApplication>
#include <QDebug>


#include <libs/windows/include/zmq.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int major, minor, patch;
    zmq_version (&major, &minor, &patch);
    printf ("VERSION: %d.%d.%d\n", major, minor, patch);

    //  Socket to talk to clients
    qDebug()<<"binding\r\n";
    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);
    int rc = zmq_bind (responder, "tcp://*:5556");
    if(rc)
        qDebug()<<"rc!=0";
    else
        qDebug()<<"bind ok";
    assert (rc == 0);

    while (1) {
        char buffer [10];
        zmq_recv (responder, buffer, 10, 0);
        qDebug()<<"Received Hello\n";
//        sleep (1);          //  Do some 'work'
        zmq_send (responder, "World", 5, 0);
    }

    return a.exec();
}
