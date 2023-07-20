#include "VideoDecoderH264.h"
#include "libyuv/libyuv.h"
#include "QDebug"
VideoDecoderH264::VideoDecoderH264()
{

}

VideoDecoderH264::~VideoDecoderH264()
{

}

bool VideoDecoderH264::DecodePacket(uint8_t *data, size_t size)
{
    size_t data_size = size;
    size_t ret = 0;
    bool result = true;
    while(data_size > 0){
        ret = av_parser_parse2(mAVCodecParserContext, mAVCodecContext, &mAVPacket->data, &mAVPacket->size, data, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);

        if(ret < 0){
            qDebug() << "parser data error";
            return false;
        }

        data += ret;
        data_size -= ret;

        if(mAVPacket->size > 0){
            result = DecodeOnePacket();
        }
    }
    return result;

}

void VideoDecoderH264::DestroyVideoDecoder()
{
    if(mAVCodecContext){
        avcodec_free_context(&mAVCodecContext);
        mAVCodecContext = nullptr;
    }

    if(mAVCodecParserContext){
        av_parser_close(mAVCodecParserContext);
        mAVCodecParserContext = nullptr;
    }

    if(mAVFrame){
        av_frame_free(&mAVFrame);
        mAVFrame = nullptr;
    }

    if(mAVPacket){
        av_packet_free(&mAVPacket);
        mAVPacket = nullptr;
    }
}

void VideoDecoderH264::InitVideoDecoderInternel()
{
    mAVCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if(!mAVCodec){
        qDebug() << "can not find h264 decoder";
        return;
    }

    mAVCodecContext = avcodec_alloc_context3(mAVCodec);
    if(!mAVCodecContext){
        qDebug() << "can not allocate context";
        return;
    }

    mAVCodecParserContext = av_parser_init(AV_CODEC_ID_H264);
    if(!mAVCodecParserContext){
        qDebug() << "can not init parser context";
        return;
    }

    if(avcodec_open2(mAVCodecContext, mAVCodec, nullptr)){
        qDebug() << "can not open codec";
        return;
    }

    mAVFrame = av_frame_alloc();
    if(!mAVFrame){
        qDebug() << "can not allocate avframe";
        return;
    }

    mAVPacket = av_packet_alloc();
    if(!mAVPacket){
        qDebug() << "can not allocate avpacket";
        return;
    }

}

bool VideoDecoderH264::DecodeOnePacket()
{
    int ret = 0;
    ret = avcodec_send_packet(mAVCodecContext, mAVPacket);
    if(ret < 0){
        qDebug() << "send packet failed :" << ret;
        return false;
    }

    while (ret >= 0) {
        ret = avcodec_receive_frame(mAVCodecContext, mAVFrame);
        if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
            return true;
        }else if(ret < 0){
            qDebug() << "decode error";
            return false;
        }

        if(!mYuvData){
//            mSize = mAVCodecContext->width * mAVCodecContext->height * 3 / 2;
            mSize = mAVCodecContext->width * mAVCodecContext->height * 4;
            mYuvData = static_cast<uint8_t*>(malloc(mSize));
        }

//        libyuv::I420Copy(mAVFrame->data[0], mAVFrame->linesize[0],
//                mAVFrame->data[1], mAVFrame->linesize[1],
//                mAVFrame->data[2], mAVFrame->linesize[2],
//                mYuvData, mAVCodecContext->width,
//                mYuvData + mAVCodecContext->width * mAVCodecContext->height, mAVCodecContext->width/2,
//                mYuvData + mAVCodecContext->width * mAVCodecContext->height * 5 / 4, mAVCodecContext->width/2,
//                mAVCodecContext->width, mAVCodecContext->height);


        libyuv::I420ToARGB(mAVFrame->data[0], mAVFrame->linesize[0],
                mAVFrame->data[1], mAVFrame->linesize[1],
                mAVFrame->data[2], mAVFrame->linesize[2],
                mYuvData, mAVCodecContext->width*4,
                mAVCodecContext->width, mAVCodecContext->height);
        mDecodedCallBack(mUserData, mYuvData, mAVCodecContext->width, mAVCodecContext->height, SimpleFrameFormatRGBA);
    }


    return true;
}
