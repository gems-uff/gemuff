#-------------------------------------------------
#
# Project created by QtCreator 2013-09-30T18:07:57
#
#-------------------------------------------------

QT       += core gui declarative widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GEMUFF
TEMPLATE = app
INCLUDEPATH += /opt/local/include /usr/local/include
QMAKE_LIBDIR += /opt/local/lib /usr/local/lib
LIBS += -lavcodec -lavformat -lswscale -lavutil -lssl -lcrypto -lopencv_core -lopencv_highgui -lopencv_imgproc  -lopencv_gpu

SOURCES += main.cpp \
        mainwindow.cpp \
    video.cpp \
    imageregister.cpp \
    VideoProcessing.cpp \
    diffprocessing.cpp \
    player.cpp \
    diffplayer.cpp \
    helperfunctions.cpp \
    diffalgorithms.cpp \
    hash.cpp \
    image.cpp

HEADERS  += mainwindow.h \
    K_IMUFF.h \
    video.h \
    imageregister.h \
    VideoProcessing.h \
    diffprocessing.h \
    player.h \
    diffplayer.h \
    helperfunctions.h \
    GEMUFF.h \
    diffalgorithms.h \
    hash.h \
    image.h

FORMS    += mainwindow.ui

QMAKE_LFLAGS += '-Wl,-rpath,/usr/local/cuda/lib'

CUDA_SOURCES = K_IMUFF.cu

unix:macx {
   # CUDA_INSTALL_PATH = $$quote(/usr/local/cuda)
   # CUDA_SDK_PATH     = $$quote("/Developer/GPU Computing/C")
   # CUDA_INCLUDES     = -I$${CUDA_INSTALL_PATH}/include -I$${CUDA_SDK_PATH}/common/inc
   # CUDA_LIBS         = -L$${CUDA_INSTALL_PATH}/lib -L$${CUDA_SDK_PATH}/lib

        # auto-detect CUDA path
    CUDA_PATH       = /Developer/NVIDIA/CUDA-5.5
    CUDA_INC_PATH   = $$CUDA_PATH/include

    #CUDA_DIR = /usr/local/cuda
    CUDA_SDK_DIR = $$quote("/Developer/GPU Computing/C")
    CUDA_CC = $$CUDA_PATH/bin/nvcc

        #$$system(which nvcc | sed 's,/bin/nvcc$,,')
    message("nvcc resides in :" $$CUDA_CC)

    INCLUDEPATH += $$CUDA_PATH/include
    INCLUDEPATH += $$CUDA_PATH/samples/common/inc
    QMAKE_CXXFLAGS =

    macx {
        INCLUDEPATH += $${CUDA_SDK_DIR}/common/inc
        QMAKE_LIBDIR += $${CUDA_PATH}/lib
        QMAKE_LIBDIR += $${CUDA_SDK_DIR}/lib
    }
    else {
        QMAKE_LIBDIR += $$CUDA_DIR/lib64
    }
#nvcc -ccbin=/usr/bin/clang -m64 -I/Developer/NVIDIA/CUDA-5.5/incude -I/Developer/NVIDIA/CUDA-5.5/samples/common/inc  -c K_IMUFF.cu -o K_IMUFF_1.o

    LIBS += -lcudart

    cuda.output = ${OBJECTS_DIR}${QMAKE_FILE_BASE}_cuda.o

    macx {
        cuda.commands = $$CUDA_CC -ccbin=/usr/bin/clang -m64 -c -arch sm_21 $$join(QMAKE_CXXFLAGS,",") $$join(INCLUDEPATH,'" -I "','-I "','"') ${QMAKE_FILE_NAME} -o ${QMAKE_FILE_OUT}
    }
    else {
        cuda.commands = $$CUDA_CC  -c -arch sm_12 $$join(QMAKE_CXXFLAGS,",") $$join(INCLUDEPATH,'" -I "','-I "','"') ${QMAKE_FILE_NAME} -o ${QMAKE_FILE_OUT}
    }

    cuda.dependcy_type = TYPE_C
    cuda.depend_command = nvcc -ccbin=/usr/bin/clang  -M -Xcompiler $$join(QMAKE_CXXFLAGS,",") $$join(INCLUDEPATH,'" -I "','-I "','"') ${QMAKE_FILE_NAME} | sed "s,^.*: ,," | sed "s,^ *,," | tr -d '\\\n'
}
cuda.input = CUDA_SOURCES
QMAKE_EXTRA_COMPILERS += cuda
