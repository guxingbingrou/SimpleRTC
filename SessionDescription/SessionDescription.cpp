#include "SessionDescription.h"
#include "Utils/Util.h"
#include "Utils/Logger.h"
#include <sstream>

static const char kDummyAddress[] = "0.0.0.0";
static const char kDummyPort[] = "9";

SessionDescription::SessionDescription()
{

}

const TransportInfo *SessionDescription::GetTransportInfoByName(const std::string &name) const
{
    for (const auto& transport : mTransports) {
      if (transport.mContentName == name) {
        return &(transport);
      }
    }
    return NULL;
}

std::string SessionDescription::ToString()
{
    std::string message;

    //session description
    message.append("v=0\r\n");

    std::stringstream ss;
    long long session_id = 94320512513472;
    ss << "o=simpleRTC " << session_id << " 2 " << "IN IP4 0.0.0.0" << "\r\n";

    message.append(ss.str());

    message.append("s=simpleRTCSession\r\n");

    message.append("t=0 0\r\n");

    // BUNDLE Groups
    for(const auto& group : mContentGroups){
        if(group.Semantics() == "BUNDLE"){
            ss.str("");
            ss << "a=group:BUNDLE";
            for(const auto& name : group.ContentNames()){
                ss << " " << name;
            }
            ss << "\r\n";
            message.append(ss.str());
        }

    }

    // MediaStream semantics
    message.append("a=msid-semantic: WMS\r\n");

    //ice-lite
    for(const auto& transport : mTransports){
        if(transport.mTransportDescription.iceMode() == ICEMODE_LITE){
            message.append("a=ice-lite\r\n");
            break;
        }
    }


    //media description
    for(const auto& content : mContents){
        BuildMediaDescription(&content, GetTransportInfoByName(content.GetMid()), &message);
    }


    return message;
}

void SessionDescription::BuildMediaDescription(const ContentInfo *content_info, const TransportInfo *transport_info, std::string *message)
{
    const MediaContentDescription* media_desc = content_info->GetMediaContentDescription();

    std::stringstream ss;

    // RFC 4566
    // m=<media> <port> <proto> <fmt>
    std::string fmt;
    std::string type;

    if(media_desc->type() == MEDIA_TYPE_VIDEO){
        type = "video";
        const VideoContentDescription* video_desc = media_desc->AsVideo();
        for(const VideoCodec& codec : video_desc->Codecs()){
            fmt.append(" ");
            fmt.append(std::to_string(codec.mID));
        }
    }else if(media_desc->type() == MEDIA_TYPE_AUDIO){
        type = "audio";
        const AudioContentDescription* audio_desc = media_desc->AsAudio();
        for(const AudioCodec& codec : audio_desc->Codecs()){
            fmt.append(" ");
            fmt.append(std::to_string(codec.mID));
        }
    }else if(media_desc->type() == MEDIA_TYPE_DATA){
        type = "application";
        const SctpDataContentDescription* data_desc = media_desc->AsSctp();
        if(data_desc){
            fmt.append(" ");
            if(data_desc->UseSctpMap()){
                fmt.append(std::to_string(data_desc->Port()));
            }else{
                fmt.append("webrtc-datachannel");
            }
        }
    }

    if(fmt.empty()){
        fmt = " 0";
    }

    ss << "m=" << type << " " << kDummyPort << " " << media_desc->Protocol() << fmt << "\r\n";
    message->append(ss.str());

    message->append("c=IN IP4 0.0.0.0\r\n");

    IceParameters ice = transport_info->mTransportDescription.ice();
    //session info
    ss.str("");
    ss << "a=ice-ufrag:" << ice.mUfrag << "\r\n";
    message->append(ss.str());

    ss.str("");
    ss << "a=ice-pwd:" << ice.mPwd << "\r\n";
    message->append(ss.str());


    const auto options = transport_info->mTransportDescription.tranportOptions();
    if(!options.empty()){
        ss.str("");
        ss << "a=ice-options:";
        ss << options[0];
        for(size_t i=1; i<options.size(); ++i ){
            ss << " " << options[i];
        }

        ss << "\r\n";
        message->append(ss.str());
    }


    ss.str("");
    FingerprintParameters fingerprint = transport_info->mTransportDescription.fingerprint();
    ss << "a=fingerprint:" << fingerprint.mAlgorithm << " " << fingerprint.mFingerPrint << "\r\n";
    message->append(ss.str());

    ss.str("");
    ss << "a=setup:" << TransportDescription::ConnectionRoleToString(transport_info->mTransportDescription.connectionRole()) << "\r\n";
    message->append(ss.str());


    //mid
    ss.str("");
    ss << "a=mid:" << content_info->GetMid() << "\r\n";
    message->append(ss.str());


    if(media_desc->type() == MEDIA_TYPE_DATA){
        const SctpDataContentDescription* data_desc = media_desc->AsSctp();
        if(data_desc){
            // draft-ietf-mmusic-sctp-sdp-23
            // a=sctp-port:<port>
            ss.str("");
            ss << "a=sctp-port:" << data_desc->Port() << "\r\n";

            ss << "a=max-message-size:" << data_desc->MaxMessageSize() << "\r\n";

            message->append(ss.str());

        }
    }else{
        //extmap
        // RFC 8285
        // a=extmap:<value>["/"<direction>] <URI> <extensionattributes>
        ss.str("");
        for(auto& map : content_info->GetMediaContentDescription()->extmaps()){
            ss << "a=extmap:" << map.id << " " << map.extmap << "\r\n";
        }
        message->append(ss.str());

        // RFC 3264
        // a=sendrecv || a=sendonly || a=sendrecv || a=inactive
        //direction
        ss.str("");
        ss << "a=";
        switch (content_info->GetMediaContentDescription()->Direction()) {
        case MediaSendOnly:
            ss << "sendonly" << "\r\n";
            break;
        case MediaRecvOnly:
            ss << "recvonly" << "\r\n";
            break;
        case MediaSendRecv:
            ss << "sendrecv" << "\r\n";
            break;
        case MediaInactive:
            ss << "inactive" << "\r\n";
            break;
        }
        message->append(ss.str());

        // RFC 5761
        //rtcp-mux
        if(content_info->GetMediaContentDescription()->RtcpMux())
            message->append("a=rtcp-mux\r\n");

        // RFC 5506
        //rtcp-rsize
        if(content_info->GetMediaContentDescription()->RtcpReducedSize())
            message->append("a=rtcp-rsize\r\n");


        if(media_desc->type() == MEDIA_TYPE_VIDEO){
            const VideoContentDescription* video_desc = media_desc->AsVideo();
            for(const VideoCodec& codec : video_desc->Codecs()){
                // RFC 4566
                // a=rtpmap:<payload type> <encoding name>/<clock rate>
                ss.str("");
                ss << "a=rtpmap:" << codec.mID << " " << codec.mName << "/" << codec.mClockrate << "\r\n";

                //a=rtcp-fb:<payload type> <parameters>/<ccm <ccm_parameters>>
                for(const auto& fb : codec.mFeedbackParams){
                    ss << "a=rtcp-fb:" << codec.mID << " " << fb.id();
                    if(!fb.param().empty())
                        ss << " " << fb.param();
                    ss << "\r\n";
                }


                //a=fmtp:<payload type> <parameters>
                if(!codec.mParamMaps.empty()){
                    ss << "a=fmtp:" << codec.mID << " ";

                    std::string end_str = "";
                    for(const auto& it : codec.mParamMaps){
                        ss << end_str;
                        end_str = ";";

                        ss << it.first << "=" << it.second;
                    }
                    ss << "\r\n";
                }

                message->append(ss.str());

            }
        }else if(media_desc->type() == MEDIA_TYPE_AUDIO){
            const AudioContentDescription* audio_desc = media_desc->AsAudio();
            for(const AudioCodec& codec : audio_desc->Codecs()){
                // RFC 4566
                // a=rtpmap:<payload type> <encoding name>/<clock rate>
                ss.str("");
                ss << "a=rtpmap:" << codec.mID << " " << codec.mName << "/" << codec.mClockrate << "/" << codec.mChannels << "\r\n";

                //a=rtcp-fb:<payload type> <parameters>/<ccm <ccm_parameters>>
                for(const auto& fb : codec.mFeedbackParams){
                    ss << "a=rtcp-fb:" << codec.mID << " " << fb.id();
                    if(!fb.param().empty())
                        ss << " " << fb.param();
                    ss << "\r\n";
                }

                //a=fmtp:<payload type> <parameters>
                if(!codec.mParamMaps.empty()){
                    ss << "a=fmtp:" << codec.mID <<" ";

                    std::string end_str = "";
                    for(const auto& it : codec.mParamMaps){
                        ss << end_str;
                        end_str = ";";

                        ss << it.first << "=" << it.second;
                    }
                    ss << "\r\n";
                }

                message->append(ss.str());
            }
        }

        // RFC 5576
        // a=ssrc-group:<semantics> <ssrc-id> ...
        for(const auto& ssrc_group : content_info->GetMediaContentDescription()->ssrcGroups()){
            if(ssrc_group.mSsrcs.empty())
                continue;
            ss.str("");
            ss << "a=ssrc-group:" << ssrc_group.mSemantics;
            for(const auto& ssrc : ssrc_group.mSsrcs){
                ss << " " << ssrc;
            }
            ss << "\r\n";
            message->append(ss.str());
        }

        // RFC 5576
        for(const auto& ssrc : content_info->GetMediaContentDescription()->ssrcInfos()){
            // a=ssrc:<ssrc-id> cname:<value>
            // a=ssrc:<ssrc-id> label:<value>
            ss.str("");
            ss << "a=ssrc:" << ssrc.mSsrc << " " << "cname:" << ssrc.mCname << "\r\n";
            ss << "a=ssrc:" << ssrc.mSsrc << " " << "label:" << ssrc.mLabel << "\r\n";
            message->append(ss.str());
        }

    }


    //candidate
    int32_t foundation = 0;
    int32_t component_id = 1; /* RTP */
    uint32_t priority = (1 << 24) * (126) + (1 << 8) * (65535)
            + (1) * (256 - component_id);
    ss.str("");
    for(auto& candidate: transport_info->mTransportDescription.candidates()){
        ss << "a=candidate:" << foundation++ << " " << component_id << " " << "udp" << " "
           << priority << " " << candidate.candidateIp << " " << candidate.candidatePort
           << " typ " << candidate.candidateType << " generation 0" << "\r\n";
    }
    message->append(ss.str());

}

std::vector<Extmap> MediaContentDescription::extmaps() const
{
    return mExtmaps;
}

void MediaContentDescription::SetExtmaps(const std::vector<Extmap> &extmaps)
{
    mExtmaps = extmaps;
}

std::vector<SsrcGroup> MediaContentDescription::ssrcGroups() const
{
    return mSsrcGroups;
}

void MediaContentDescription::SetSsrcGroups(const std::vector<SsrcGroup> &ssrcGroups)
{
    mSsrcGroups = ssrcGroups;
}

std::vector<SsrcInfo> MediaContentDescription::ssrcInfos() const
{
    return mSsrcInfos;
}

void MediaContentDescription::SetSsrcInfos(const std::vector<SsrcInfo> &ssrcInfos)
{
    mSsrcInfos = ssrcInfos;
}

ContentInfo::ContentInfo(const ContentInfo &o)
    : mMediaContentDescription(o.mMediaContentDescription->Clone()),
      mMid(o.mMid)
{

}

ContentInfo &ContentInfo::operator=(const ContentInfo &o)
{
    mMediaContentDescription = o.mMediaContentDescription->Clone(),
    mMid = o.mMid;
    return *this;
}

AudioCodec *AudioContentDescription::FindCodecById(int id)
{
    for(auto& codec : mCodecs){
        if(codec.mID == id){
            return &codec;
        }
    }
    return nullptr;
}

VideoCodec *VideoContentDescription::FindCodecById(int id)
{
    for(auto& codec : mCodecs){
        if(codec.mID == id){
            return &codec;
        }
    }
    return nullptr;
}

AudioCodec *AudioContentDescription::FindCodecByName(const std::string &name)
{
    for(auto& codec : mCodecs){
        if(codec.mName == name){
            return &codec;
        }
    }
    return nullptr;
}

VideoCodec *VideoContentDescription::FindCodecByName(const std::string &name)
{
    for(auto& codec : mCodecs){
        if(codec.mName == name){
            return &codec;
        }
    }
    return nullptr;
}
