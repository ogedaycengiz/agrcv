QT       += core network

QT       -= gui

TARGET = ozdoken-pancar
CONFIG   += console
CONFIG   -= app_bundle

SOURCES += main.cpp \
    GoruntuYoneticisi.cpp \
    GoruntuIsleme.cpp \
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



LIBS += -lqextserialport
unix:DEFINES = _TTY_POSIX_

INCLUDEPATH += INCLUDEPATH += /usr/local/qextserialport
#INCLUDEPATH += $$PWD/qextserialport

OTHER_FILES += \
    cross_compile.txt \
    respawn.sh \
    vncboot.sh \
    compile.sh



unix:!macx: LIBS += -L$$PWD/qextserialport/build/ -lqextserialport

#INCLUDEPATH += $$PWD/qextserialport/build
#DEPENDPATH += $$PWD/qextserialport/build
