# JIKIMZON_Qt

## 개발 환경

- Qt Framework 6.5.3
- FFmpeg 7.1
- C++ 15.0.0

## 튜토리얼

### 외부 라이브러리 설치 (macOS)

### ffmpeg
```bash
    brew install ffmpeg
```
    
### openssl

```bash
    brew install openssl

    //OpenSSL의 경로를 환경 변수에 추가(`.bash_profile` 또는 `.zshrc` 파일)
    export PATH="/usr/local/opt/openssl/bin:$PATH"
    export LDFLAGS="-L/usr/local/opt/openssl/lib"
    export CPPFLAGS="-I/usr/local/opt/openssl/include"

    //변경 사항을 적용
    source ~/.bash_profile  # 또는 source ~/.zshrc
```

### 플랫폼 별 config 수정

Qt 프로젝트의 `.pro` 파일을 FFmpeg, OpenSSL를 macOS에서 사용하기 위해 라이브러리 설치 경로에 따라 설정을 조정

## `.pro` 파일 수정 단계

1. **`.pro` 파일 열기**: Qt 프로젝트 디렉토리에서 해당 프로젝트의 `.pro` 파일을 찾습니다.
2. **필요한 모듈 추가**: 파일의 시작 부분에 필요한 Qt 모듈을 포함합니다:
    
    ```
    QT  += core gui network multimedia sql charts  
    ```
    
3. **플랫폼별 설정**: 조건문을 사용하여 macOS를 확인하고 라이브러리 경로를 추가
    1. **macx 블록**: 이 블록은 macOS에 특화된 설정을 포함합니다. `macx` 키워드는 이러한 설정이 macOS에서만 적용되도록 보장
    2. **라이브러리 경로**:
    - `QMAKE_RPATHDIR`: FFmpeg의 런타임 라이브러리 경로를 설정
    - `INCLUDEPATH`: OpenSSL, FFmpeg의 헤더 파일을 찾는 위치를 지정
    - `LIBS`: 지정된 라이브러리에 링크하여 애플리케이션이 해당 기능을 사용

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
}

```

빌드가 완료된 후 `.pro` 파일 파일을 더블 클릭하여 애플리케이션을 실행, 비디오 스트리밍과 객체 탐지 결과를 실시간으로 확인


## 스택

- Qt Framework: GUI 개발
- FFmpeg: 비디오 스트리밍 및 디코딩
- C++: 주요 프로그래밍 언어
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
