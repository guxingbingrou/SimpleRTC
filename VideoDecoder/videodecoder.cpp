#include "videodecoder.h"
#include "videodecoderh264.h"

std::shared_ptr<VideoDecoder> VideoDecoder::CreateVideoDecoder(DecoderType decoder_type)
{
    std::shared_ptr<VideoDecoder> decoder;
    switch (decoder_type) {
        case SimpleH264:
            decoder = std::make_shared<VideoDecoderH264>();
        break;
        default:
            decoder = nullptr;
        break;
    }
    return decoder;

}

void VideoDecoder::InitVideoDecoder(OnDecodedFrame callback, void *user)
{
    mDecodedCallBack = callback;
    mUserData = user;
    InitVideoDecoderInternel();
}

VideoDecoder::VideoDecoder()
{

}
