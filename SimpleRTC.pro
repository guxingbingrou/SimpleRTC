QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

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

INCLUDEPATH += $$PWD/thirdparty/includes
INCLUDEPATH += $$PWD/thirdparty/includes/ffmpeg


LIBS += -L$$PWD/thirdparty/libs/ffmpeg -lavcodec -lavutil
LIBS += -L$$PWD/thirdparty/libs/libyuv -lyuv

SOURCES += \
    PeerConnection/simplepeerconnection.cpp \
    SessionDescription/mediadescription.cpp \
    SessionDescription/simplesessiondescription.cpp \
    Utils/logger.cpp \
    VideoDecoder/videodecoder.cpp \
    VideoDecoder/videodecoderh264.cpp \
    main.cpp \
    mainwidget.cpp \
    testvideodecoder.cpp

HEADERS += \
    PeerConnection/simplepeerconnection.h \
    SessionDescription/SessionDesciptionOption.h \
    SessionDescription/mediadescription.h \
    SessionDescription/simplesessiondescription.h \
    Utils/logger.h \
    VideoDecoder/videodecoder.h \
    VideoDecoder/videodecoderh264.h \
    mainwidget.h \
    testvideodecoder.h

FORMS += \
    mainwidget.ui \
    testvideodecoder.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
