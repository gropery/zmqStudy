TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp

win32: LIBS += -L$$PWD/libs/windows/lib/x64/ -llibzmq-v142-mt-4_3_4

INCLUDEPATH += $$PWD/libs/windows/lib/x64
DEPENDPATH += $$PWD/libs/windows/lib/x64

HEADERS += \
    libs/windows/include/zmq.h
