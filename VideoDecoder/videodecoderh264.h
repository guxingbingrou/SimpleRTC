#ifndef VIDEODECODERH264_H
#define VIDEODECODERH264_H

#include "VideoDecoder/videodecoder.h"

extern "C"{
#include "libavcodec/avcodec.h"
//#include "libavutil/avutil.h"
}
class VideoDecoderH264 : public VideoDecoder
{
public:
    VideoDecoderH264();
    ~VideoDecoderH264();

    virtual bool DecodePacket(uint8_t* data, size_t size) override;
    virtual void DestroyVideoDecoder() override;
private:
    virtual void InitVideoDecoderInternel() override;
    bool DecodeOnePacket();

    const AVCodec* mAVCodec = nullptr;
    AVCodecContext* mAVCodecContext = nullptr;
    AVPacket* mAVPacket = nullptr;
    AVFrame* mAVFrame = nullptr;
    AVCodecParserContext* mAVCodecParserContext = nullptr;

    uint8_t* mYuvData = nullptr;
    int mSize = 0;
};

#endif // VIDEODECODERH264_H
