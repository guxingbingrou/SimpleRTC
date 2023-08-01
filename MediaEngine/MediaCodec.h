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

    int mID;
    std::string mName;
    int mClockrate;

    CodecParamterMap mParamMaps;
    std::vector<FeedbackParam> mFeedbackParams;
    virtual void AddDefalutParams();

    void AddCodecParamter(const std::string& key, const std::string& value);

    void AddFeedbackParam(const std::string& id, const std::string& value);
protected:
    MediaCodec(const int id, const std::string& name, const int clockrate);


};


class VideoCodec : public MediaCodec{
public:
    VideoCodec(const int id, const std::string& name, int clockrate=90000)
        : MediaCodec(id, name, clockrate){
    }

    VideoCodec(const int id)
        : MediaCodec(id, "", 90000){
    }

    virtual ~VideoCodec();
    void AddDefalutParams() override;

private:

};

class AudioCodec : public MediaCodec{
public:
    AudioCodec(const int id, const std::string& name, int channels, int clockrate=48000)
        : MediaCodec(id, name, clockrate), mChannels(channels){}

    AudioCodec(const int id)
        : MediaCodec(id, "", 48000){}

    virtual ~AudioCodec();

    int mChannels;

};

#endif // MEDIACODEC_H
