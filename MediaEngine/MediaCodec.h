#ifndef MEDIACODEC_H
#define MEDIACODEC_H
#include <string>
#include <map>
#include <vector>

using CodecParamterMap = std::map<std::string, std::string>;

class FeedbackParam{
public:
    FeedbackParam() = default;
    FeedbackParam(const std::string& id, const std::string& param)
        :m_id(id), m_param(param){}
    explicit FeedbackParam(const std::string& id) : m_id(id), m_param(""){}

//    bool operator==(const FeedbackParam& other) const;

    const std::string& id() const {return m_id;}

    const std::string& param() const {return m_param;}

private:
    std::string m_id;
    std::string m_param;
};


class MediaCodec
{
public:
    virtual ~MediaCodec();
//    MediaCodec& operator=(const MediaCodec& codec);
//    MediaCodec& operator=(MediaCodec&& codec);


    int mID;
    std::string mName;
    int mClockrate;

    CodecParamterMap mParamMaps;
    std::vector<FeedbackParam> mFeedbackParams;

protected:
    MediaCodec(const int id, const std::string& name, const int clockrate);
    virtual void AddDefalutParams();

};


class VideoCodec : public MediaCodec{
public:
    VideoCodec(const int id, const std::string& name)
        : MediaCodec(id, name, 90000){
    }

    virtual ~VideoCodec();
//    VideoCodec& operator=(const VideoCodec& codec);
//    VideoCodec& operator=(VideoCodec&& codec);
private:
    void AddDefalutParams() override;
};

class AudioCodec : public MediaCodec{
public:
    AudioCodec(const int id, const std::string& name, int channels)
        : MediaCodec(id, name, 48000), mChannels(channels){}

    virtual ~AudioCodec();
//    AudioCodec& operator=(const AudioCodec& codec);
//    AudioCodec& operator=(AudioCodec&& codec);



    int mChannels;

};

#endif // MEDIACODEC_H
