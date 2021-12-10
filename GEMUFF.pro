#-------------------------------------------------
#
# Project created by QtCreator 2013-09-30T18:07:57
#
#-------------------------------------------------

QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CUDA_PATH       = /usr/local/cuda
INCLUDEPATH += $$CUDA_PATH/include
INCLUDEPATH += $$CUDA_PATH/samples/common/inc

OPENCV = OPENCV = /home/josericardo/libs/opencv
GEMUFFLIB = /home/josericardo/eclipse-workspace/GMUFFLib
TARGET = GEMUFF
TEMPLATE = app
INCLUDEPATH += /opt/local/include /usr/local/include /usr/include/ffmpeg $${OPENCV}/include/opencv4 $${GEMUFFLIB}/include
QMAKE_LIBDIR += $${CUDA_PATH}/lib64 /usr/local/lib $${OPENCV}/lib $${GEMUFFLIB}/Debug
LIBS += -lz -lGMUFFLib -lcudart -lavcodec  -lopencv_cudaarithm -lopencv_imgcodecs -lssl -lcrypto -lavformat -lswscale  -lavutil  -lopencv_core  -lopencv_highgui  -lopencv_imgproc   -lboost_filesystem  -lboost_system  -lopencv_cudafilters -lopencv_cudaimgproc

message("libs:" $$QMAKE_LIBDIR)

SOURCES += main.cpp \
    Helper.cpp \
        mainwindow.cpp \
    #VideoProcessing.cpp \
    #diffprocessing.cpp \
    player.cpp \
    diffplayer.cpp \
    VideoPlayer.cpp \
    mergeplayer.cpp

HEADERS  += mainwindow.h \
    #VideoProcessing.h \
    #diffprocessing.h \
    Helper.h \
    player.h \
    diffplayer.h \    
    VideoPlayer.h \
    mergeplayer.h


FORMS    += mainwindow.ui

QMAKE_LFLAGS += '-Wl,-rpath,/usr/local/cuda/lib'


