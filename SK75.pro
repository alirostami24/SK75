QT       += core gui serialport concurrent network

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

HEADERS += \
    $$PWD/Detector/Detector.h \
    $$PWD/Detector/DetectorInterface.h \
    $$PWD/Detector/globalCalculation.h \
    $$PWD/Detector/IntensityDetector/IntensityDetector.h \
    $$PWD/Detector/IntensityDetector/TH/TH.hpp \
    Controller.h \
    SerialController.h \
    VideoCapture.h

SOURCES += \
    $$PWD/Detector/Detector.cpp \
    $$PWD/Detector/globalCalculation.cpp \
    $$PWD/Detector/IntensityDetector/IntensityDetector.cpp \
    $$PWD/Detector/IntensityDetector/TH/TH.cpp \
    Controller.cpp \
    SerialController.cpp \
    VideoCapture.cpp \
    main.cpp \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32 {
    CONFIG += link_pkgconfig

    PKGCONFIG += gstreamer-1.0 \
            gstreamer-rtsp-server-1.0 \
            gstreamer-video-1.0 \
            glib-2.0 \
            gobject-2.0 \
            gio-2.0

    LIBS += -L'C:/Program Files/OpenCV3.4.16/lib/' -lopencv_world3416

    INCLUDEPATH += 'C:/Program Files/OpenCV3.4.16/include'
    DEPENDPATH += 'C:/Program Files/OpenCV3.4.16/include'

    LIBS += -L$$PWD/ExternalLibraries/ControlPanelSDK/Windows/ -lControlPanelSDK

    INCLUDEPATH += $$PWD/ExternalLibraries/ControlPanelSDK/include
    DEPENDPATH += $$PWD/ExternalLibraries/ControlPanelSDK/include

    DEFINES += DUMMY_VIDEO
}

unix {
    CONFIG += link_pkgconfig

    PKGCONFIG += gstreamer-1.0 \
            gstreamer-rtsp-server-1.0 \
            gstreamer-video-1.0 \
            glib-2.0 \
            gobject-2.0 \
            gio-2.0 \
            opencv

    LIBS += -L$$PWD/ExternalLibraries/ControlPanelSDK/Linux/ -lControlPanelSDK

    INCLUDEPATH += $$PWD/ExternalLibraries/ControlPanelSDK/include
    DEPENDPATH += $$PWD/ExternalLibraries/ControlPanelSDK/include

    DEFINES += DUMMY_VIDEO
}


