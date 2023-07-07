#include <iostream>
#include <assert.h>
#include <string.h>

#include "libs/windows/include/zmq.h"


using namespace std;

void *context;
void *socketSend;
void *socketRcv;


#define LOGC(strInfo) cout<<"C:"<<strInfo<<endl
#define LOGD(strInfo) cout<<"D:"<<strInfo<<endl
#define LOGE(strInfo) cout<<"E:"<<strInfo<<endl
#define jassert assert

void *buf;
size_t bufsize;

void createSendSocket(void)
{
    if (!socketSend)
    {
        LOGD("Create send socket");
        socketSend = zmq_socket(context, ZMQ_PUB);

        if (!socketSend)
        {
            LOGD("Couldn't create send socket");
            LOGE(zmq_strerror(zmq_errno()));
            jassert(false);
        }

        if (zmq_bind(socketSend, "tcp://*:5557"))
        {
            LOGD("Couldn't bind socket");
            LOGE(zmq_strerror(zmq_errno()));
            jassert(false);
        }
    }
}

void createRcvSocket(void)
{
    if (!socketRcv)
    {
        LOGD("Create recv socket");
        socketRcv = zmq_socket(context, ZMQ_SUB);

        if (!socketRcv)
        {
            LOGD("Couldn't create recv socket");
            LOGE(zmq_strerror(zmq_errno()));
            jassert(false);
        }

        zmq_setsockopt(socketRcv, ZMQ_SUBSCRIBE, nullptr, 0);
        if (zmq_connect(socketRcv, "tcp://127.0.0.1:3335"))
        {
            LOGD("Couldn't connect socket");
            LOGC(zmq_strerror(zmq_errno()));
            jassert(false);
        }
    }
}

void closeSocket(void)
{
    if (socketSend)
    {
        LOGD("Closing send socket");
        zmq_close(socketSend);
        socketSend = nullptr;
    }
    if (socketRcv)
    {
        LOGD("Closing recv socket");
        zmq_close(socketRcv);
        socketRcv = nullptr;
    }
    if (context)
    {
        LOGD("Closing zmq context");
        zmq_ctx_destroy(context);
        context = nullptr;
    }
}


int main()
{
    context = zmq_ctx_new();

    socketSend = 0;
    if (!socketSend)
        createSendSocket();

    socketRcv = 0;
    if (!socketRcv)
        createRcvSocket();

    zmq_msg_t messageSend;
    zmq_msg_t messageRcv;

    while(true)
    {
        // 接收数据
        zmq_msg_init(&messageRcv);
        if (zmq_msg_recv(&messageRcv, socketRcv, ZMQ_DONTWAIT) != -1)  // Non-blocking to wait to receive a messageRcv
        {
            // 获取接收数据的指针和大小
            bufsize = zmq_msg_size(&messageRcv);
            buf = zmq_msg_data(&messageRcv);

            cout<<"||"<<bufsize;

            // 发送数据
            zmq_msg_init_size(&messageSend, bufsize);
            memcpy(zmq_msg_data(&messageSend), (void *)buf, bufsize);
            int size_m = zmq_msg_send(&messageSend, socketSend, 0);
            assert(size_m);
        }
    }

    zmq_msg_close(&messageSend);
    zmq_msg_close(&messageRcv);

    closeSocket();
    return 0;
}
