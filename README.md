# JIKIMZON_Qt

## 개발 환경
- Qt Framework
- FFmpeg 
- OpenSSL
- C++

## 튜토리얼
### ① 외부 라이브러리 설치
#### macOS
- Qt Framework 6.5.3
- ffmpeg 7.1
  ```bash
    brew install ffmpeg
  ```
- openssl
    ```bash
        brew install openssl

        //OpenSSL의 경로를 환경 변수에 추가(`.bash_profile` 또는 `.zshrc`  파일)
        export PATH="/usr/local/opt/openssl/bin:$PATH"
        export LDFLAGS="-L/usr/local/opt/openssl/lib"
        export CPPFLAGS="-I/usr/local/opt/openssl/include"

        //변경 사항 적용
        source ~/.bash_profile  # or source ~/.zshrc
    ```  
#### Windows
- Qt 6.8.1
    - 환경변수 설정
        ```
        C:\Qt\6.8.1\mingw_64\bin
        C:\Qt\Tools\mingw1310_64\bin
        ```
- ffmpeg 7.1
    1. https://www.gyan.dev/ffmpeg/builds/ffmpeg-release-full-shared.7z 다운로드 및 압축해제
    2. 해당 경로 환경변수 설정
        ```
        C:\dev\ffmpeg\bin
        ```
- openssl 3.4.0
    1. https://slproweb.com/products/Win32OpenSSL.html win64 다운로드 및 실행
    2. 경로 지정 및 설치
    3. 해당 경로 환경변수 설정
        ```
        C:\dev\openssl\bin
        ```
→ 환경변수 설정 이후 재부팅


#### Linux
- Qt
- ffmpeg 5.1.6
    ``` bash
    sudo apt update
    sudo apt install libavcodec-dev libavformat-dev libavutil-dev libswscale-dev
    ```
- openssl 3.0.15
    ``` bash
    sudo apt update
    sudo apt install libssl-dev libcrypto++-dev
    ```



### ② `JIKIMZON.pro` config 파일 수정
- FFmpeg, OpenSSL 라이브러리 설치 경로를 `JIKIMZON.pro` 파일에 추가
    - `QMAKE_RPATHDIR`: 런타임 라이브러리 경로 설정
    - `INCLUDEPATH`: 헤더 파일 경로 설정
    - `LIBS`: 라이브러리에 링크 경로 설정
#### macOS
```ini
# macOS 전용 설정
macx {
    QMAKE_RPATHDIR += /opt/homebrew/Cellar/ffmpeg/7.1_3/lib

    # OpenSSL 라이브러리 경로
    INCLUDEPATH += /opt/homebrew/opt/openssl@3/include
    LIBS += -L/opt/homebrew/opt/openssl@3/lib -lssl -lcrypto
    LIBS += -lz 

    # FFmpeg 포함 및 라이브러리 경로
    INCLUDEPATH += /opt/homebrew/Cellar/ffmpeg/7.1_3/include
    LIBS += -L/opt/homebrew/Cellar/ffmpeg/7.1_3/lib \\
            -lavcodec -lavformat -lavutil -lswscale -lpostproc -lavdevice -lavfilter
}
```
#### Windows
```ini
#windows
win32 {
    LIBS += -lws2_32    # winsock2

    INCLUDEPATH += "C:\dev\ffmpeg\include"
    LIBS += -LC:\dev\ffmpeg\lib \
            -lavcodec -lavformat -lavutil -lswscale -lpostproc

    INCLUDEPATH += "C:\dev\openssl\include"
    LIBS += -LC:\dev\openssl\lib \
            -lssl -lcrypto
}
```

#### Linux
```ini
#linux
linux {
    INCLUDEPATH += /usr/include/aarch64-linux-gnu/qt5
    INCLUDEPATH += /usr/include/aarch64-linux-gnu
    LIBS += -lavcodec -lavformat -lavutil -lswscale \
            -lssl -lcrypto
}
```

### ③ 빌드 및 실행

- QtCreator
  - `.pro` 파일을 더블 클릭하여 프로젝트를 열고, 빌드하여 비디오 스트리밍과 객체 탐지 결과를 실시간으로 확인
   ![실행방법](https://github.com/user-attachments/assets/20d2d9c8-6d81-44e1-ba01-4d07b7ed0b41)

- Windows
    ```
    ./build.cmd
    ```
- Linux
    ```
    chmod +x ./build.sh
    ./build.sh
    ```
## 스택

- Qt Framework: GUI 개발
- FFmpeg: 비디오 스트리밍 및 디코딩
- C++
- QChartView: 데이터 시각화

## 기능

- **비디오 스트리밍**: FFmpeg를 사용한 실시간 비디오 스트리밍 및 디코딩
    - **FFmpeg**를 활용하여 실시간 비디오 스트리밍을 디코딩하고, Qt를 사용하여 UI에 비디오를 표시
    - 별도 스레드에서 처리해 메인 UI 스레드 멈춤 없이 비디오를 지연 없이 스트리밍
- **객체 탐지 및 통계 시각화**: 실시간으로 객체 탐지 결과를 표시하고, 변화 추이를 대시보드에서 시각화
    - 객체 탐지의 메타데이터를 처리하여 시각화하는 역할 예를 들어, 객체가 감지된 시간과 객체의 종류 등을 실시간으로 화면에 표시
    - 객체 탐지 결과를 **도넛 차트**로 시각화
- **설정 기능**: 밝기, 대비, 채도 조정 등 비디오 스트리밍 관련 설정
    - 비디오 스트리밍에 관련된 **밝기**, **대비**, **채도** 등의 설정을 사용자에게 제공
    - 설정 창은 `SliderDialog`로 구현되어 있으며, 사용자가 실시간으로 스트리밍되는 비디오의 시각적 요소를 조정
- **실시간 대시보드**: 객체 탐지 변화 추이 및 실시간 알림 시스템
    - **QChartView**를 사용하여 실시간 객체 탐지 결과와 변화 추이를 **선형 그래프** 형태로 시각화
    - **알림 시스템**을 통해 중요한 객체나 이벤트가 탐지되면 즉시 사용자에게 알림을 보냄
- **캘린더 위젯**: 저장된 객체 탐지 통계를 막대그래프 형태로 표시
    - **QChartView**를 사용하여 저장된 객체 탐지 통계를 **막대그래프**로 표시
    - 사용자가 저장된 과거 데이터를 분석하거나, 특정 시간대의 탐지 결과를 비교

## 프로젝트 구조

```
project_root/
│
├── JIKIMZON.pro
├── Card.qml
├── main.qml
│
├── build.cmd
├── build.sh
├── README.md
│
├── certs
│   ├── keyfile.bin
│   ├── keyfile1.bin
│   └── server.cert
│
├── inc
│   ├── calendarwidget.h
│   ├── dashboardwidget.h
│   ├── decryptor.h
│   ├── deserializer.h
│   ├── eventlogmanager.h
│   ├── frame.h
│   ├── mainwindow.h
│   ├── metadatadisplay.h
│   ├── sliderdialog.h
│   └── videostreamplayer.h
│
├── src
│   ├── calendarwidget.cpp
│   ├── dashboardwidget.cpp
│   ├── decryptor.cpp
│   ├── deserializer.cpp
│   ├── eventlogmanager.cpp
│   ├── main.cpp
│   ├── mainwindow.cpp
│   ├── metadatadisplay.cpp
│   ├── sliderdialog.cpp
│   └── videostreamplayer.cpp
│
├── ui
│   ├── calendarwidget.ui
│   ├── dashboardwidget.ui
│   ├── mainwindow.ui
│   ├── metadatadisplay.ui
│   └── sliderdialog.ui
│    
├── res
│   ├── Moon2.png
│   ├── Sun2.png
│   ├── arrow_down.png
│   ├── backward.png
│   ├── biodegradable.png
│   ├── box.png
│   ├── calendar.png
│   ├── data-collection.png
│   ├── default.png
│   ├── desktop-2.png
│   ├── event_log.db
│   ├── forward.png
│   ├── glass.png
│   ├── growth.png
│   ├── icons.qrc
│   ├── left.png
│   ├── linechart.png
│   ├── logo.png
│   ├── metal.png
│   ├── monitoring.png
│   ├── paper.png
│   ├── pause.png
│   ├── plastic.png
│   ├── play.png
│   ├── right.png
│   ├── setting.png
│   └── timeline.png
├── archive
│   └── ...
└── build
    └── ...
```
