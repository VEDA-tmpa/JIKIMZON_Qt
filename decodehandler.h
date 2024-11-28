#ifndef DECODEHANDLER_H
#define DECODEHANDLER_H

#include <QObject>
#include <QByteArray>

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

    void DecodeData(const std::vector<uint8_t>&, OUT std::vector<uint8_t>&);

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

#endif // DECODEHANDLER_H
