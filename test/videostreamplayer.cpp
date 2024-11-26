#include "videostreamplayer.h"
#include "Decryptor.h"
#include "deserializer.h"
#include "Frame.h"
#include <QDebug>
#include <opencv2/opencv.hpp>
#include <vector>
#include <fstream>
#include <arpa/inet.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
}

VideoStreamPlayer::VideoStreamPlayer(QObject *parent)
    : QThread(parent), tcpSocket(nullptr), stop(true)
{
    qDebug() << "VideoStreamPlayer initialized.";
}

VideoStreamPlayer::~VideoStreamPlayer()
{
    stopStream();
    wait();  // 스레드 종료 대기
    qDebug() << "VideoStreamPlayer destroyed.";
}

void VideoStreamPlayer::startStream(QTcpSocket *socket, int width, int height, int size)
{
    tcpSocket = socket;
    stop = false;

    if (!isRunning()) {
        start(LowPriority);  // 스레드 실행
        qDebug() << "Thread started.";
    }
}

void VideoStreamPlayer::stopStream()
{
    stop = true;
    qDebug() << "Stream stopped.";
}

void VideoStreamPlayer::pauseStream()
{
    pause = true;  // 일시 정지 활성화
    qDebug() << "Stream paused.";
}

void VideoStreamPlayer::resumeStream()
{
    pause = false;  // 일시 정지 해제
    qDebug() << "Stream resumed.";
}

bool VideoStreamPlayer::isStopped() const
{
    return stop;
}


void VideoStreamPlayer::run()
{
    QByteArray buffer;

    // 키 초기화 및 복호화기 생성
    QString keyFilePath = "/Volumes/jjeongni/QtProgramming/test/keyfile.bin";
    QByteArray key;
    if (!loadKey(keyFilePath, key)) {
        qDebug() << "Failed to load key from" << keyFilePath;
        stop = true;
        return;
    }
    Decryptor decryptor(key);
    frame::Deserializer deserializer(key);

    qDebug() << "Stream running...";

    while (!stop) {
        // 데이터 수신
        if (tcpSocket && tcpSocket->bytesAvailable() > 0) {
            qDebug() << "Receiving data from socket...";

            // 헤더 받기
            // headerBuffer 크기만큼 데이터 읽기
            QByteArray headerBuffer;
            headerBuffer = tcpSocket->read(sizeof(frame::HeaderStruct));
            qDebug() << "headerBuffer.size(): " << headerBuffer.size();
            // 데이터 확인 및 처리
            if (headerBuffer.size() != sizeof(frame::HeaderStruct)) {
                qDebug() << "Failed to read the header.";
                return;
            }

            // 헤더 역직렬화
            std::vector<uint8_t> headerVector(headerBuffer.begin(), headerBuffer.end());
            frame::Header header;
            header.Deserialize(headerVector);


            qDebug() << "header.GetHeaderStruct().frameId: " << header.GetHeaderStruct().frameId;
            qDebug() << "header.GetHeaderStruct().imageHeight: " << header.GetHeaderStruct().imageHeight;
            qDebug() << "header.GetHeaderStruct().imageWidth: " << header.GetHeaderStruct().imageWidth;
            qDebug() << "header.GetHeaderStruct().bodySize: " << header.GetHeaderStruct().bodySize;
            qDebug() << "header.GetHeaderStruct().timestamp: " << header.GetHeaderStruct().timestamp;

            // 정은님이 알아야 할 조건
            // tcp 를 통해 바이너리 데이터 받기
                // 1. 어? 얼마큼 받아야하지?
                    // 1-1. 아~  : header.GetHeaderStruct().bodySize 이구나
            QByteArray bodyBuffer;
            bodyBuffer = tcpSocket->read(header.GetHeaderStruct().bodySize);
            qDebug() << "header.GetHeaderStruct().bodySize: " << header.GetHeaderStruct().bodySize;
            // 데이터 확인 및 처리
            if (bodyBuffer.size() != header.GetHeaderStruct().bodySize) {
                qDebug() << "Failed to read the body.";
                return;
            }

            // ㅇㅋ 그 크기만큼 받았어 ->
                // 1. 아 이거는 지기성씨가 암호화 했댓지?
                    // 우선 복호화부터 하자
            // 복호화
            std::vector<uint8_t> decryptedBody;
            std::vector<uint8_t> encryptedBody(bodyBuffer.begin(), bodyBuffer.end());
            std::vector<uint8_t> nonce(12, 0x00);
            std::string timestamp = header.GetHeaderStruct().timestamp;

            // nonce를 timestamp에서 가져오기 (마지막 12바이트 사용)
            if (timestamp.size() >= 12) {
                std::copy(timestamp.end() - 12, timestamp.end(), nonce.begin());
            }

            // QByteArray로 변환 (복호화 함수에서 QByteArray 사용 시 필요)
            QByteArray encryptedData = QByteArray::fromRawData(
                reinterpret_cast<const char*>(encryptedBody.data()), encryptedBody.size());

            // 복호화 수행
            QByteArray decryptedData = decryptor.decrypt(encryptedData);
            if (decryptedData.isEmpty()) {
                qDebug() << "Decryption failed!";
                continue;
            }

            // 복호화된 데이터를 vector<uint8_t>로 변환
            decryptedBody.assign(decryptedData.begin(), decryptedData.end());

            qDebug() << "Decryption successful. Decrypted body size:" << decryptedBody.size();


            // 2. (역직렬화) : body 구조 : 어? image밖에 없네
            std::vector<uint8_t> bodyVector(decryptedBody.begin(), decryptedBody.end());
            frame::Body body;
            body.Deserialize(bodyVector);

            // 2. Body 데이터 확인
            const auto& bodyData = body.GetBodyData();
            if (bodyData.empty()) {
                qDebug() << "Body data is empty!";
                return;
            }

            // Body 데이터 확인
            qDebug() << "Body deserialized. Image size:" << body.GetBodyData().size();

        } else {
            qDebug() << "Waiting for data...";
        }

        msleep(150);  // CPU 사용 제한
    }

    qDebug() << "Stream has stopped.";
}
