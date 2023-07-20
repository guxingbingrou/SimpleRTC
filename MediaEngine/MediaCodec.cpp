#include "MediaCodec.h"

//MediaCodec &MediaCodec::operator=(const MediaCodec &codec) = default;
//MediaCodec &MediaCodec::operator=(MediaCodec &&codec) = default;
MediaCodec::~MediaCodec() = default;

MediaCodec::MediaCodec(const int id, const std::string &name, const int clockrate)
    : mID(id), mName(name), mClockrate(clockrate)
{
    AddDefalutParams();
}

void MediaCodec::AddDefalutParams()
{
    mFeedbackParams.emplace_back(FeedbackParam("transport-cc", ""));
}


//VideoCodec &VideoCodec::operator=(const VideoCodec &codec) = default;
//VideoCodec &VideoCodec::operator=(VideoCodec &&codec) = default;
VideoCodec::~VideoCodec() = default;

void VideoCodec::AddDefalutParams()
{
    MediaCodec::AddDefalutParams();
    //fmtp
    mParamMaps["level-asymmetry-allowed"] = "1";
    mParamMaps["packetization-mode"] = "1";
    mParamMaps["profile-level-id"] = "42e01f";

    //rtcp-fb

    mFeedbackParams.emplace_back(FeedbackParam("nack", ""));
    mFeedbackParams.emplace_back(FeedbackParam("nack", "pli"));
    mFeedbackParams.emplace_back(FeedbackParam("goog-remb", ""));
//    mFeedbackParams.emplace_back(FeedbackParam("ccm", "fir"));

}

//AudioCodec &AudioCodec::operator=(const AudioCodec &codec) = default;
//AudioCodec &AudioCodec::operator=(AudioCodec &&codec) = default;
AudioCodec::~AudioCodec() = default;
