QT       += core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    decodehandler.cpp \
    main.cpp \
    mainwindow.cpp \
    videostreamplayer.cpp

HEADERS += \
    decodehandler.h \
    mainwindow.h \
    videostreamplayer.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


#######
## 타겟 플랫폼별 include, lib

#windows
win32 {
    LIBS += -lws2_32    # winsock2

    INCLUDEPATH += "C:\dev\ffmpeg\include"
    LIBS += -LC:\dev\ffmpeg\lib \
            -lavcodec -lavformat -lavutil -lswscale -lpostproc

    INCLUDEPATH += "C:\dev\openssl\include"
    LIBS += -LC:\dev\openssl\lib \
            -lssl -lcrypto
    
    INCLUDEPATH += "C:\dev\opencv\build\include"
    LIBS += -LC:\dev\opencv\build\x64\lib \
            -lopencv_core4100 -lopencv_highgui4100 -lopencv_imgcodecs4100 -lopencv_imgproc4100 -lopencv_videoio4100
}

#linux
linux {
    INCLUDEPATH += /usr/include/opencv4
    INCLUDEPATH += /usr/include/aarch64-linux-gnu
    LIBS += -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio \
            -lavcodec -lavformat -lavutil -lswscale \  # apt install libavcodec-dev libavformat-dev libavutil-dev libswscale-dev
            -lssl -lcrypto
}

#mac
macx {
    INCLUDEPATH += /opt/homebrew/Cellar/ffmpeg/7.1_3/include
    LIBS += -L/opt/homebrew/Cellar/ffmpeg/7.1_3/lib \
            -lavcodec -lavformat -lavutil -lswscale -lpostproc

    INCLUDEPATH += /opt/homebrew/opt/openssl@3/include
    LIBS += -L/opt/homebrew/opt/openssl@3/lib -lssl -lcrypto
    LIBS += -lz

    INCLUDEPATH += /opt/homebrew/opt/opencv/include/opencv4
    LIBS += -L/opt/homebrew/opt/opencv/lib \
            -lopencv_core \
            -lopencv_imgproc \
            -lopencv_highgui \
            -lopencv_imgcodecs \
            -lopencv_videoio
}
