#-------------------------------------------------
#
# Project created by QtCreator 2018-05-30T10:14:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LabDemo
TEMPLATE = app
CONFIG += c++11
#CONFIG += link_pkgconfig
#PKGCONFIG += x11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        widget.cpp \
    ccameracontrol.cpp \
    playlocalm4.cpp \
    objectdect/object_detection_wrapper.cpp \
    objectdect/tensorflow_object_detector.cpp \
    chuamdectthd.cpp \
    cfaceclassification.cpp \
    cfaceclsthread.cpp \
    cfacefeat.cpp \
    cpersonreidthd.cpp

#SOURCES += /home/dlib-19.13/dlib/all/source.cpp

HEADERS += \
        widget.h \
    publicfun.h \
    includeCn/DataType.h \
    includeCn/DecodeCardSdk.h \
    includeCn/HCNetSDK.h \
    includeCn/LinuxPlayM4.h \
    includeCn/PlayM4.h \
    ccameracontrol.h \
    playlocalm4.h \
    objectdect/object_detection_wrapper.h \
    objectdect/tensorflow_object_detector.h \
    chuamdectthd.h \
    g.h \
    cfaceclassification.h \
    cfaceclsthread.h \
    cfacefeat.h \
    cpersonreidthd.h

FORMS += \
        widget.ui


INCLUDEPATH +=  /usr/local/include/opencv \
/home/proj/library/opencv/include \
/home/proj/library/tensorflow \
/home/proj/library/tensorflow/third_party \
/home/proj/library/tensorflow/bazel-genfiles \
/usr/local/lib/python3.5/dist-packages \
/usr/local/lib/python3.5/dist-packages/tensorflow/include \
/usr/local/lib/python3.5/dist-packages/tensorflow/include/external/nsync/public/

INCLUDEPATH +=  /usr/include/python2.7 \
-fno-strict-aliasing -Wdate-time -D_FORTIFY_SOURCE=2 -g -fstack-protector-strong -Wformat -Werror=format-security  -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes



LIBS +=-L/usr/lib64/python2.7/config -L/usr/lib64 -lpython2.7 \
-lpthread -ldl -lutil -lm  -Xlinker -export-dynamic -Wl,-O1 -Wl,-Bsymbolic-functions

LIBS += /home/proj/library/opencv/build/lib/libopencv_core.so \
/home/proj/library/opencv/build/lib/libopencv_imgproc.so \
/home/proj/library/opencv/build/lib/libopencv_imgcodecs.so


LIBS += -L/home/proj/lab/human_cpp_detector/lib -ltensorflow_cc -ltensorflow_framework


LIBS += -L/lib/hclib/ -Wl,-rpath=./:/lib/hclib/HCNetSDKCom:/lib/hclib -lhcnetsdk -lPlayCtrl -lAudioRender -lSuperRender

INCLUDEPATH += /home/proj/lab/usrcontrol/LAB
LIBS += /home/proj/lab/usrcontrol/Release/liblabplugin.so


#INCLUDEPATH += /home/dlib-19.13
#LIBS += /home/dlib-19.13/dlib/build/libdlib.a
