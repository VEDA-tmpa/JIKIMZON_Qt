#include "decodehandler.h"
#include <QDebug>

DecodeHandler::DecodeHandler(int height, int width, int bitrate, int fps, enum AVPixelFormat srcPixFmt, enum AVPixelFormat dstPixFmt, QObject *parent)
    : QObject{parent}, mHeight(height), mWidth(width), mBitrate(bitrate), mFps(fps), mSrcPixFmt(srcPixFmt), mDstPixFmt(dstPixFmt)
{
    // srcPixFmt: AV_PIX_FMT_YUV420P
    // dstPixFmt: AV_PIX_FMT_RGB24

    initFFmpegDecoder();
}

DecodeHandler::~DecodeHandler()
{
    av_parser_close(mCodecParser);
    avcodec_free_context(&mCodecContext);
    sws_freeContext(mSwsContext);
}

void DecodeHandler::initFFmpegDecoder()
{
    mCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!mCodec)
    {
        qDebug() << "[DecodeHandler] Codec H.264 not found";
        exit(EXIT_FAILURE);
    }

    mCodecParser = av_parser_init(mCodec->id);
    if (!mCodecParser)
    {
        qDebug() << "[DecodeHandler] Codec parser not found";
        exit(EXIT_FAILURE);
    }

    mCodecContext = avcodec_alloc_context3(mCodec);
    if (!mCodecContext)
    {
        qDebug() << "[DecodeHandler] Codec context not found";
        exit(EXIT_FAILURE);
    }

    // AVDictionary *param = nullptr;
    // av_dict_set(&param, "preset", "ultrafast", 0);
    // av_dict_set(&param, "tune", "zerolatency", 0);
    // 인코드에서는 이렇게 설정하고 아래 nullptr에 넣었는데, 디코드에서는 필요없는듯

    if (avcodec_open2(mCodecContext, mCodec, nullptr) < 0)
    {
        qDebug() << "[DecodeHandler] Could not open codec";
        exit(EXIT_FAILURE);
    }

    mSwsContext = sws_getContext(
        mWidth, mHeight, mSrcPixFmt,   // input resolution, format
        mWidth, mHeight, mDstPixFmt,     // output resolution, format //AV_PIX_FMT_RGB565 //AV_PIX_FMT_BGR24
        SWS_BILINEAR, nullptr, nullptr, nullptr                             // SWS_BICUBIC
    );
    if (!mSwsContext)
    {
        qDebug() << "[DecodeHandler] Could not initialize sws context";
        exit(EXIT_FAILURE);
    }
}

// data -> parser -> pkt(YUV) -> frame(YUV) -> frame(RGB) -> outFrame(RGB)
void DecodeHandler::DecodeFrame(const std::vector<uint8_t>& data, OUT cv::Mat& outFrame)
{
    AVPacket *pkt = av_packet_alloc();  // encoded pkt
    if (!pkt)
    {
        qDebug() << "[DecodeHandler] Could not allocate packet";
        exit(EXIT_FAILURE);
    }

    AVFrame *frame = av_frame_alloc();  // decoded frame
    if (!frame)
    {
        qDebug() << "[DecodeHandler] Could not allocate frame";
        exit(EXIT_FAILURE);
    }

    uint8_t* buf = const_cast<uint8_t*>(data.data());
    int bufSize = data.size();

    // pkt에 바로 data를 넣지 않고, data를 buf에 넣고 parser로 pkt에 넘겨줌
    while (bufSize > 0)
    {
        int consumed = av_parser_parse2(mCodecParser, mCodecContext, &pkt->data, &pkt->size, buf, bufSize, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
        if (consumed < 0)
        {
            qDebug() << "[DecodeHandler] Error while parsing";
            exit(EXIT_FAILURE);
        }

        buf += consumed;
        bufSize -= consumed;

        if (pkt->size > 0)
        {
            if (avcodec_send_packet(mCodecContext, pkt) < 0)
            {
                qDebug() << "[DecodeHandler] Error while sending packet";
                exit(EXIT_FAILURE);
            }

            // 디코더의 어딘가로 pkt를 보내고 frame으로 받음

            while (avcodec_receive_frame(mCodecContext, frame) == 0)
            {
                // assign new rgbFrame
                AVFrame* rgbFrame = av_frame_alloc();
                if (!rgbFrame)
                {
                    qDebug() << "[DecodeHandler] Could not allocate frame";
                    exit(EXIT_FAILURE);
                }

                rgbFrame->format = mDstPixFmt;
                rgbFrame->width = mWidth;
                rgbFrame->height = mHeight;

                if (av_frame_get_buffer(rgbFrame, 32) < 0)
                {
                    qDebug() << "[DecodeHandler] Could not allocate frame buffer";
                    exit(EXIT_FAILURE);
                }

                // convert YUV to RGB
                sws_scale(mSwsContext,
                         frame->data,
                         frame->linesize,
                         0,
                         mHeight,
                         rgbFrame->data, 
                         rgbFrame->linesize);

                outFrame.release();
                outFrame = cv::Mat(mHeight, mWidth, CV_8UC3, rgbFrame->data[0]);
                
                qDebug() << "[DecodeHandler] Frame decoded";
                //return; ?

                // emit signal
                // emit frameDecoded(outData);

                av_frame_free(&rgbFrame);
            }

            av_packet_unref(pkt);
        }
    }

    av_packet_free(&pkt);
    av_frame_free(&frame);

    return;
}