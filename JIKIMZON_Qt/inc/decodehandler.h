#ifndef JIKIMZON_DECODEHANDLER_H
#define JIKIMZON_DECODEHANDLER_H

#include <QObject>
#include <QByteArray>

#include <opencv2/core.hpp>

#include <vector>
#include <cstdint>

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavutil/imgutils.h>
    #include <libswscale/swscale.h>
    #include <libavformat/avformat.h>
}

#define OUT

class DecodeHandler : public QObject
{
    Q_OBJECT

public:
    explicit DecodeHandler(int, int, int, int, enum AVPixelFormat, enum AVPixelFormat, QObject *parent = nullptr);
    ~DecodeHandler();

    void DecodeFrame(const std::vector<uint8_t>&, OUT cv::Mat&);

private:
    void initFFmpegDecoder();

private:
    const AVCodec* mCodec;
    SwsContext* mSwsContext;
    AVCodecContext* mCodecContext;
    AVCodecParserContext *mCodecParser;

    enum AVPixelFormat mSrcPixFmt;
    enum AVPixelFormat mDstPixFmt;

    int mWidth;
    int mHeight;
    int mBitrate;
    int mFps;

signals:


};

#endif // JIKIMZON_DECODEHANDLER_H
