# JIKIMZON_Qt
## 개발 환경

---

- Qt Framework
- FFmpeg
- C++

## 튜토리얼

---

*빌드 방법 (코드)*

### 외부 라이브러리 설치 (macOS)

### ffmpeg

1. Homebrew를 사용하여 FFmpeg를 설치합니다. 터미널을 열고 다음 명령어를 입력합니다:
    
    ```bash
    brew install ffmpeg
    
    ```
    
2. 설치가 완료되면, FFmpeg의 경로가 자동으로 설정됩니다. 설치된 버전을 확인하려면 다음 명령어를 입력합니다:
    
    ```bash
    ffmpeg -version
    
    ```
    

### openssl

1. Homebrew를 사용하여 OpenSSL을 설치합니다:
    
    ```bash
    brew install openssl
    
    ```
    
2. OpenSSL의 경로를 환경 변수에 추가합니다. `.bash_profile` 또는 `.zshrc` 파일에 다음을 추가합니다:
    
    ```bash
    export PATH="/usr/local/opt/openssl/bin:$PATH"
    export LDFLAGS="-L/usr/local/opt/openssl/lib"
    export CPPFLAGS="-I/usr/local/opt/openssl/include"
    
    ```
    
3. 변경 사항을 적용하기 위해 터미널에서 다음 명령어를 실행합니다:
    
    ```bash
    source ~/.bash_profile  # 또는 source ~/.zshrc
    
    ```
    

### opencv

1. Homebrew를 사용하여 OpenCV를 설치합니다:
    
    ```bash
    brew install opencv
    
    ```
    
2. OpenCV의 경로를 환경 변수에 추가합니다. `.bash_profile` 또는 `.zshrc` 파일에 다음을 추가합니다:
    
    ```bash
    export PKG_CONFIG_PATH="/usr/local/opt/opencv/lib/pkgconfig"
    
    ```
    
3. 변경 사항을 적용하기 위해 터미널에서 다음 명령어를 실행합니다:
    
    ```bash
    source ~/.bash_profile  # 또는 source ~/.zshrc
    
    ```
    

### 플랫폼 별 config 수정

Qt 프로젝트의 `.pro` 파일을 FFmpeg, OpenSSL, OpenCV를 macOS에서 사용하기 위해 수정하는 방법은 다음과 같습니다. 라이브러리 설치 경로에 따라 설정을 조정해야 합니다.

## `.pro` 파일 수정 단계

1. **`.pro` 파일 열기**: Qt 프로젝트 디렉토리에서 해당 프로젝트의 `.pro` 파일을 찾습니다.
2. **필요한 모듈 추가**: 파일의 시작 부분에 필요한 Qt 모듈을 포함합니다:
    
    ```
    QT       += core gui network multimedia sql charts
    
    ```
    
3. **플랫폼별 설정**: 조건문을 사용하여 macOS를 확인하고 라이브러리 경로를 추가합니다. 아래와 같이 설정할 수 있습니다:
    1. **macx 블록**: 이 블록은 macOS에 특화된 설정을 포함합니다. `macx` 키워드는 이러한 설정이 macOS에서만 적용되도록 보장합니다.
    2. **라이브러리 경로**:
    - `QMAKE_RPATHDIR`: FFmpeg의 런타임 라이브러리 경로를 설정합니다.
    - `INCLUDEPATH`: OpenSSL, FFmpeg, OpenCV의 헤더 파일을 찾는 위치를 지정합니다.
    - `LIBS`: 지정된 라이브러리에 링크하여 애플리케이션이 해당 기능을 사용할 수 있도록 합니다.

```
# macOS 전용 설정
macx {
    # FFmpeg 라이브러리 경로 추가
    QMAKE_RPATHDIR += /opt/homebrew/Cellar/ffmpeg/7.1_3/lib

    # OpenSSL 라이브러리 경로
    INCLUDEPATH += /opt/homebrew/opt/openssl@3/include
    LIBS += -L/opt/homebrew/opt/openssl@3/lib -lssl -lcrypto
    LIBS += -lz  # 압축을 위한 zlib

    # FFmpeg 포함 및 라이브러리 경로
    INCLUDEPATH += /opt/homebrew/Cellar/ffmpeg/7.1_3/include
    LIBS += -L/opt/homebrew/Cellar/ffmpeg/7.1_3/lib \\
            -lavcodec -lavformat -lavutil -lswscale -lpostproc -lavdevice -lavfilter

    # OpenCV 포함 및 라이브러리 경로
    INCLUDEPATH += /opt/homebrew/opt/opencv/include/opencv4
    LIBS += -L/opt/homebrew/opt/opencv/lib \\
            -lopencv_core \\
            -lopencv_imgproc \\
            -lopencv_highgui \\
            -lopencv_imgcodecs \\
            -lopencv_videoio \\
            # 필요한 추가 OpenCV 모듈
            -lopencv_gapi \\
            -lopencv_stitching \\
            ...  # 여기에 다른 필요한 OpenCV 모듈 추가
}

```

### 추가 설명:

- **FFmpeg**: 비디오 스트리밍 및 디코딩을 위한 라이브러리입니다. `QMAKE_RPATHDIR`을 사용하여 런타임 시 라이브러리 경로를 설정합니다.
- **OpenSSL**: 보안 통신을 위한 암호화 라이브러리입니다. SSL/TLS 연결을 구현하는 데 필요합니다.
- **OpenCV**: 이미지 및 비디오 처리 라이브러리로, 객체 탐지 및 컴퓨터 비전 기능을 제공합니다. 필요한 모듈을 추가하여 기능을 확장할 수 있습니다.

이 설정을 통해 macOS에서 Qt 프로젝트가 FFmpeg, OpenSSL, OpenCV를 원활하게 사용할 수 있도록 구성됩니다. 필요에 따라 추가적인 OpenCV 모듈을 포함할 수 있으며, 각 모듈은 프로젝트의 요구 사항에 맞게 조정

*실행 방법 (영상이나 캡처)*

빌드가 완료된 후 실행 파일을 더블 클릭하여 애플리케이션을 실행합니다. 실행 시 비디오 스트리밍과 객체 탐지 결과를 실시간으로 확인할 수 있습니다.

## 스택

---

- Qt Framework: GUI 개발
- FFmpeg: 비디오 스트리밍 및 디코딩
- C++: 주요 프로그래밍 언어
- QChartView: 데이터 시각화

## 기능

---

- **비디오 스트리밍**: FFmpeg를 사용한 실시간 비디오 스트리밍 및 디코딩
    - **FFmpeg**를 활용하여 실시간 비디오 스트리밍을 디코딩하고, Qt를 사용하여 UI에 비디오를 표시합니다.
    - 비디오 스트리밍을 별도의 스레드에서 처리하여 메인 UI 스레드가 멈추지 않고 비디오를 부드럽게 표시할 수 있도록 합니다.
- **객체 탐지 및 통계 시각화**: 실시간으로 객체 탐지 결과를 표시하고, 변화 추이를 대시보드에서 시각화
    - 객체 탐지의 메타데이터를 처리하여 시각화하는 역할을 합니다. 예를 들어, 객체가 감지된 시간과 객체의 종류 등을 실시간으로 화면에 표시합니다.
    - 객체 탐지 결과를 **도넛 차트**로 시각화할 수 있습니다.
- **설정 기능**: 밝기, 대비, 채도 조정 등 비디오 스트리밍 관련 설정
    - 비디오 스트리밍에 관련된 **밝기**, **대비**, **채도** 등의 설정을 사용자에게 제공합니다. 설정 창은 `SliderDialog`로 구현되어 있으며, 사용자가 실시간으로 스트리밍되는 비디오의 시각적 요소를 조정할 수 있습니다.
- **실시간 대시보드**: 객체 탐지 변화 추이 및 실시간 알림 시스템
    - **QChartView**를 사용하여 실시간 객체 탐지 결과와 변화 추이를 **선형 그래프** 형태로 시각화합니다.
    - **알림 시스템**을 통해 중요한 객체나 이벤트가 탐지되면 즉시 사용자에게 알림을 보냅니다.
- **캘린더 위젯**: 저장된 객체 탐지 통계를 막대그래프 형태로 표시
    - **QChartView**를 사용하여 저장된 객체 탐지 통계를 **막대그래프**로 표시합니다. 사용자가 과거 데이터를 분석하거나, 특정 시간대의 탐지 결과를 비교할 수 있게 합니다.

## 프로젝트 구조(트리)

---

```
project_root/
│
├── src/
│   ├── main.cpp
│   ├── mainwindow.cpp
│   ├── mainwindow.h
│   ├── videostreamplayer.cpp
│   ├── videostreamplayer.h
│   ├── metadatadisplay.cpp
│   ├── metadatadisplay.h
│   ├── dashboard.cpp
│   ├── dashboard.h
│   ├── calendarwidget.cpp
│   ├── calendarwidget.h
│   └── sliderdialog.cpp
│
├── include/
│   └── ffmpeg/
│
├── resources/
│   └── icons/
│
├── tests/
│
└── project.pro
```
