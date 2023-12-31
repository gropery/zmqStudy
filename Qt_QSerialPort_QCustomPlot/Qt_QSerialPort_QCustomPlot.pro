QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    openserialport.cpp \
    plot.cpp \
    qcustomplot\qcustomplot.cpp

HEADERS += \
    channel_generated.h \
    flatbuffers/base.h \
    flatbuffers/code_generators.h \
    flatbuffers/flatbuffers.h \
    flatbuffers/flatc.h \
    flatbuffers/flexbuffers.h \
    flatbuffers/grpc.h \
    flatbuffers/hash.h \
    flatbuffers/idl.h \
    flatbuffers/minireflect.h \
    flatbuffers/pch/flatc_pch.h \
    flatbuffers/pch/pch.h \
    flatbuffers/reflection.h \
    flatbuffers/reflection_generated.h \
    flatbuffers/registry.h \
    flatbuffers/stl_emulation.h \
    flatbuffers/util.h \
    libs/windows/include/zmq.h \
    mainwindow.h \
    openserialport.h \
    plot.h \
    qcustomplot\qcustomplot.h

FORMS += \
    mainwindow.ui \
    plot.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc



win32: LIBS += -L$$PWD/libs/windows/lib/x64/ -llibzmq-v142-mt-4_3_4

INCLUDEPATH += $$PWD/libs/windows/lib/x64
DEPENDPATH += $$PWD/libs/windows/lib/x64
