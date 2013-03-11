TEMPLATE = app
CONFIG += console
CONFIG += qt

QT       += core gui network opengl

SOURCES += main.cpp \
    GoruntuYoneticisi.cpp \
    GoruntuIsleme.cpp \
    cvblob/cvBlob/cvtrack.cpp \
    cvblob/cvBlob/cvlabel.cpp \
    cvblob/cvBlob/cvcontour.cpp \
    cvblob/cvBlob/cvcolor.cpp \
    cvblob/cvBlob/cvblob.cpp \
    cvblob/cvBlob/cvaux.cpp \
    TcpBaglantisi.cpp \
    SeriPrt.cpp \
    VideoStab.cpp

unix:!macx:!symbian: LIBS += -L$$PWD/../OpenCV-2.4.4/lib/ -lopencv_core

LIBS +=  -lopencv_imgproc -lopencv_objdetect -lopencv_highgui -lopencv_calib3d -lopencv_features2d -lopencv_video

HEADERS += \
    GoruntuYoneticisi.h \
    GoruntuIsleme.h \
    SeriPort.h \
    cvblob/cvBlob/cvblob.h \
    TcpBaglantisi.h \
    SeriPrt.h \
    ParalelP.h \
    VideoStab.h



unix:!macx:!symbian: LIBS += -L$$PWD/cvblobs/ -lblob

INCLUDEPATH += $$PWD/cvblobs
DEPENDPATH += $$PWD/cvblobs

unix:!macx:!symbian: PRE_TARGETDEPS += $$PWD/cvblobs/libblob.a

LIBS += -lqextserialport
unix:DEFINES = _TTY_POSIX_

INCLUDEPATH += $$PWD/qextserialport

OTHER_FILES += \
    cross_compile.txt \
    respawn.sh \
    vncboot.sh


