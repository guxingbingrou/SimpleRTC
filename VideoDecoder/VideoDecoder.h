#ifndef VIDEODECODER_H
#define VIDEODECODER_H
#include <memory>
#include "Utils/Util.h"


using OnDecodedFrame = void(*)(void* user, uint8_t* data, int width, int height, VideoFrameFormat format);

class VideoDecoder
{
public:
    virtual ~VideoDecoder() = default;
    static std::shared_ptr<VideoDecoder> CreateVideoDecoder(VideoCodecType decoder_type);

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
