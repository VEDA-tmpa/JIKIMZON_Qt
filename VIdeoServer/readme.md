# 컴파일
## Boost 라이브러리 설치
```shell
brew install boost  # Homebrew를 사용하여 Boost 라이브러리를 설치
```
## 이미 설치된 Boost 라이브러리를 최신 버전으로 업그레이드
```shell
brew upgrade boost  # 기존에 설치된 Boost를 최신 버전으로 업그레이드
```
## DYLD_LIBRARY_PATH 환경 변수에 라이브러리 경로 추가 (리눅스 및 macOS에서 라이브러리 경로 설정)
```shell
export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH  # /usr/local/lib 경로를 추가
export DYLD_LIBRARY_PATH=/opt/homebrew/lib:$DYLD_LIBRARY_PATH  # /opt/homebrew/lib 경로를 추가 (Apple Silicon Mac에서 사용)
```
## C++14 표준을 사용하여 TCP_Server.cpp 파일을 컴파일하고, 결과물을 server라는 실행 파일로 생성
```shell
g++ -std=c++14 -o server TCP_Server.cpp \
  -I/opt/homebrew/opt/boost/include \  # Boost 헤더 파일 경로를 포함
  -I/usr/local/include/opencv4 \  # OpenCV 4 헤더 파일 경로를 포함
  -L/opt/homebrew/opt/boost/lib \  # Boost 라이브러리 파일 경로를 링크
  -L/usr/local/lib \  # OpenCV 라이브러리 파일 경로를 링크
  -lboost_system \  # Boost 시스템 라이브러리 링크
  -lopencv_core \  # OpenCV 핵심 라이브러리 링크
  -lopencv_imgproc \  # OpenCV 이미지 처리 라이브러리 링크
  -lopencv_highgui \  # OpenCV 고수준 GUI 라이브러리 링크
  -lopencv_videoio \  # OpenCV 비디오 입출력 라이브러리 링크
  -lopencv_imgcodecs  # OpenCV 이미지 코덱 라이브러리 링크
```
