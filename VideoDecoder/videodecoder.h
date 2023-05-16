#ifndef VIDEODECODER_H
#define VIDEODECODER_H
#include <memory>

enum DecoderType{
    SimpleH264 = 0,
    SimpleH265,
    SimpleNone
};

enum FrameFormat{
    SimpleFrameFormatRGBA = 0,
    SimpleFrameFormatI420,
    SimpleFrameFormatNV12,
    SimpleFrameFormatNV21,
    SimpleFrameFormatNone
};

using OnDecodedFrame = void(*)(void* user, uint8_t* data, int width, int height, FrameFormat format);

class VideoDecoder
{
public:
    virtual ~VideoDecoder() = default;
    static std::shared_ptr<VideoDecoder> CreateVideoDecoder(DecoderType decoder_type);

    void InitVideoDecoder(OnDecodedFrame callback, void* user);

    virtual void DestroyVideoDecoder() = 0;

    virtual bool DecodePacket(uint8_t* data, size_t size) = 0;

protected:
    VideoDecoder();

    virtual void InitVideoDecoderInternel() = 0;

    OnDecodedFrame mDecodedCallBack = nullptr;
    void* mUserData = nullptr;
};

#endif // VIDEODECODER_H
