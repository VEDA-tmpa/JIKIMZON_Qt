QT       += core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    decryptor.cpp \
    deserializer.cpp \
    main.cpp \
    mainwindow.cpp \
    videostreamplayer.cpp

HEADERS += \
    Frame.h \
    decryptor.h \
    deserializer.h \
    mainwindow.h \
    videostreamplayer.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# windows 전처리 지시어
win32 {
    LIBS += -lws2_32    # winsock

    INCLUDEPATH += C:\dev\ffmpeg\include
    LIBS += -LC:\dev\ffmpeg\lib \
        -lavcodec -lavformat -lavutil -lswscale -lpostproc

    INCLUDEPATH += C:\dev\openssl\include
    LIBS += -LC:\dev\openssl\lib \
        -lssl -lcrypto

    INCLUDEPATH += C:\dev\opencv\build\include
    LIBS += -LC:\dev\opencv\build\x64\vc16\lib \
        -lopencv_world4100
}

# mac os 전처리 지시어
macx {
    # FFmpeg 경로 설정
    INCLUDEPATH += /opt/homebrew/Cellar/ffmpeg/7.1_3/include
    LIBS += -L/opt/homebrew/Cellar/ffmpeg/7.1_3/lib \
            -lavcodec -lavformat -lavutil -lswscale -lpostproc

    # OpenSSL 라이브러리 경로 추가
    INCLUDEPATH += /opt/homebrew/opt/openssl@3/include
    LIBS += -L/opt/homebrew/opt/openssl@3/lib -lssl -lcrypto
    LIBS += -lz

    # OpenCV Include and Library Paths
    INCLUDEPATH += /opt/homebrew/opt/opencv/include/opencv4

    LIBS += -L/opt/homebrew/opt/opencv/lib \
            -lopencv_core \
            -lopencv_imgproc \
            -lopencv_highgui \
            -lopencv_imgcodecs \
            -lopencv_videoio
}


