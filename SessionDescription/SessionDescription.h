#ifndef SESSIONDESCRIPTION_H
#define SESSIONDESCRIPTION_H
#include <string>
#include <vector>
#include <memory>
#include "SimpleSsrc.h"
#include "MediaEngine/MediaCodec.h"
#include "TransportDescription.h"
enum MediaType {
  MEDIA_TYPE_AUDIO,
  MEDIA_TYPE_VIDEO,
  MEDIA_TYPE_DATA,
  MEDIA_TYPE_UNSUPPORTED
};

enum MediaDirection{
    MediaSendOnly = 1,
    MediaRecvOnly = 2,
    MediaSendRecv = 3,
    MediaInactive = 4
};

struct Extmap{
    Extmap(int id, std::string extmap){
        this->id = id;
        this->extmap = extmap;
    }
    int id;
    std::string extmap;
};

class AudioContentDescription;
class VideoContentDescription;
class SctpDataContentDescription;
class UnsupportedContentDescription;

class MediaContentDescription{
public:
    MediaContentDescription() = default;
    virtual ~MediaContentDescription() = default;

    virtual MediaType type() const = 0;

    virtual std::unique_ptr<MediaContentDescription> Clone() const{
        return std::unique_ptr<MediaContentDescription>(CloneInternal());
    }

    virtual AudioContentDescription* AsAudio() { return nullptr; }
    virtual const AudioContentDescription* AsAudio() const { return nullptr; }

    virtual VideoContentDescription* AsVideo() { return nullptr; }
    virtual const VideoContentDescription* AsVideo() const { return nullptr; }

    virtual SctpDataContentDescription* AsSctp() { return nullptr; }
    virtual const SctpDataContentDescription* AsSctp() const { return nullptr; }

    virtual UnsupportedContentDescription* AsUnsupported() { return nullptr; }
    virtual const UnsupportedContentDescription* AsUnsupported() const {
      return nullptr;
    }


    // `protocol` is the expected media transport protocol, such as RTP/AVPF,
    // RTP/SAVPF or SCTP/DTLS.
    virtual std::string Protocol() const { return mProtocol; }
    virtual void SetProtocol(const std::string& protocol) {
      mProtocol = protocol;
    }

    virtual MediaDirection Direction() const {return mMediaDirection;}
    virtual void SetMediaDirection(MediaDirection direction){
        mMediaDirection = direction;
    }

    virtual bool RtcpMux() const {return mRtcpMux;}
    virtual void SetRtcpMux(bool rtcp_mux){
        mRtcpMux = rtcp_mux;
    }

    virtual bool RtcpReducedSize() const {return mRtcpReducedSize;}
    virtual void SetRtcpReducedSize(bool reduced_size){
        mRtcpReducedSize = reduced_size;
    }

    virtual void AddExtMap(const Extmap& ext_map){
        mExtmaps.push_back(ext_map);
    }

    virtual void GenerateSsrc(){
        SsrcInfo ssrc;
        ssrc.mSsrc = SsrcGenerator::GenerateSsrc();
        mSsrcInfos.push_back(ssrc);
    }


    std::vector<Extmap> extmaps() const;
    void SetExtmaps(const std::vector<Extmap> &extmaps);

    std::vector<SsrcGroup> ssrcGroups() const;
    void SetSsrcGroups(const std::vector<SsrcGroup> &ssrcGroups);

    std::vector<SsrcInfo> ssrcInfos() const;
    void SetSsrcInfos(const std::vector<SsrcInfo> &ssrcInfos);

protected:
    MediaDirection mMediaDirection = MediaInactive;

    bool mRtcpMux = true;           //Multiplexing RTP Data and Control Packets on a Single Port    RFC 5761
    bool mRtcpReducedSize = true;         //An a=rtcp-rsize attribute present at the SDP session-level MUST be ignored
    std::string mProtocol;

    std::vector<Extmap> mExtmaps;
    std::vector<SsrcGroup> mSsrcGroups;
    std::vector<SsrcInfo> mSsrcInfos;

protected:
    virtual MediaContentDescription* CloneInternal() const = 0;
};

template<class C>
class MediaContentDescriptionImpl : public MediaContentDescription{
public:
    using CodecType = C;
    virtual const std::vector<C>& Codecs() const { return mCodecs; }
    virtual void SetCodecs(const std::vector<C>& codecs){ mCodecs = codecs; }
    virtual void AddCodec(const C& codec){ mCodecs.push_back(codec); }

private:
    std::vector<C> mCodecs;
};


class AudioContentDescription : public MediaContentDescriptionImpl<AudioCodec>{
public:
    virtual MediaType type() const override {return MEDIA_TYPE_AUDIO; }
    virtual AudioContentDescription* AsAudio() override{ return this; }
    virtual const AudioContentDescription* AsAudio() const override{ return this; }

private:
    virtual MediaContentDescription* CloneInternal() const override{
        return new AudioContentDescription(*this);
    }
};

class VideoContentDescription : public MediaContentDescriptionImpl<VideoCodec>{
public:
    virtual MediaType type() const override {return MEDIA_TYPE_VIDEO; }
    virtual VideoContentDescription* AsVideo() override{ return this; }
    virtual const VideoContentDescription* AsVideo() const override{ return this; }

private:
    virtual MediaContentDescription* CloneInternal() const override{
        return new VideoContentDescription(*this);
    }
};

class SctpDataContentDescription : public MediaContentDescription{
public:
    virtual MediaType type() const override {return MEDIA_TYPE_DATA; }
    virtual SctpDataContentDescription* AsSctp() override{ return this; }
    virtual const SctpDataContentDescription* AsSctp() const override{ return this; }

    bool UseSctpMap() const {return mUseSctpMap; }
    void SetUseSctpMap(bool enable){ mUseSctpMap = enable; }

    int Port() const {return mPort; }
    void SetPort(int port) {mPort = port;}

    int MaxMessageSize() const {return mMaxMessageSize; }
    void SetMaxMessageSize(int size) {mMaxMessageSize = size;}
private:
    bool mUseSctpMap = false;
    int mPort = 5000;
    int mMaxMessageSize = 64 * 1024;

private:
    virtual MediaContentDescription* CloneInternal() const override{
        return new SctpDataContentDescription(*this);
    }
};


class ContentInfo{
public:
    ContentInfo() = default;
    ~ContentInfo() = default;
    ContentInfo(const ContentInfo& o);
    ContentInfo& operator=(const ContentInfo& o);

    ContentInfo(ContentInfo&& o) = default;
    ContentInfo& operator=(ContentInfo&& o) = default;

    MediaContentDescription* GetMediaContentDescription(){return mMediaContentDescription.get();}
    const MediaContentDescription* GetMediaContentDescription() const{return mMediaContentDescription.get();}

    void SetMediaContentDescription(std::unique_ptr<MediaContentDescription> description){
        mMediaContentDescription = std::move(description);
    }

    std::string GetMid() const {return mMid; }
    void SetMid(const std::string& mid){ mMid = mid; }


private:
    std::unique_ptr<MediaContentDescription> mMediaContentDescription;
    std::string mMid;
};


class ContentGroup{
public:
    explicit ContentGroup(const std::string& semantics)
        : mSemantics(semantics) {}

    void AddContentName(const std::string& content_name){
        mContentNames.push_back(content_name);
    }

    const std::string Semantics() const{ return mSemantics;}

    const std::vector<std::string> ContentNames() const {return mContentNames;}
private:
    std::string mSemantics;
    std::vector<std::string> mContentNames;
};

struct TransportInfo{
    TransportInfo(const std::string& content_name, const TransportDescription& transportDescription)
        : mContentName(content_name), mTransportDescription(transportDescription) {}

    std::string mContentName;
    TransportDescription mTransportDescription;
};

class SessionDescription
{
public:
    SessionDescription();

    void AddContentInfo(const std::string& mid, std::unique_ptr<MediaContentDescription> desc){
        ContentInfo content;
        content.SetMid(mid);
        content.SetMediaContentDescription(std::move(desc));
        mContents.push_back(content);
    }

    void AddContenGroup(const ContentGroup& content_group){
        mContentGroups.push_back(content_group);
    }

    void AddTransportInfo(const std::string& content_name , const TransportDescription& desc){
        mTransports.push_back(TransportInfo(content_name, desc));
    }

    std::vector<ContentInfo> ContentInfos() const {return mContents;}

    const TransportInfo* GetTransportInfoByName(const std::string& name) const;


    std::string ToString();


private:
    std::vector<ContentInfo> mContents;
    std::vector<ContentGroup> mContentGroups;
    std::vector<TransportInfo> mTransports;

    bool mExtmapAllowMixed = true;


    void BuildMediaDescription(const ContentInfo* content_info, const TransportInfo* transport_info, std::string* message);
};

#endif // SESSIONDESCRIPTION_H
