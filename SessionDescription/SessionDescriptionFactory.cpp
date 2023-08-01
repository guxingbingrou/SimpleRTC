#include "SessionDescriptionFactory.h"
#include "Utils/Util.h"
#include "Utils/Logger.h"
#include <sstream>

SessionDescriptionFactory::SessionDescriptionFactory()
{

}

std::unique_ptr<SessionDescription> SessionDescriptionFactory::CreateOffer(const SessionDescriptionOption *option)
{
    int startCodecId = 96;

    std::string twccExt = "http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01";
    int twccID = 3;
    Extmap twccExtMap(twccID, twccExt);

    auto offer = std::make_unique<SessionDescription>();

    if(option->has_audio || option->recieve_audio){
        auto audioContent = std::make_unique<AudioContentDescription>();

        if(option->has_audio && option->recieve_audio){
            audioContent->SetMediaDirection(MediaSendRecv);
        }else if(option->has_audio){
            audioContent->SetMediaDirection(MediaSendOnly);
        }else{
            audioContent->SetMediaDirection(MediaRecvOnly);
        }

        AudioCodec audioCodec(startCodecId++, "opus", 2);
        audioCodec.AddDefalutParams();
        audioContent->AddCodec(audioCodec);

        if(audioContent->Direction() & MediaSendOnly)
            audioContent->GenerateSsrc();

        audioContent->AddExtMap(twccExtMap);

        audioContent->SetProtocol("UDP/TLS/RTP/SAVPF");

        TransportDescription transportDesc;
        transportDesc.AddOption(ICE_OPTION_TRICKLE);
//        transportDesc.SetIceMode(ICEMODE_LITE);
        transportDesc.SetConnectionRole(CONNECTIONROLE_ACTPASS);
        transportDesc.AddCandidate("127.0.0.1", 60000, "host");

        offer->AddContentInfo("audio" , std::move(audioContent));
        offer->AddTransportInfo("audio", transportDesc);

    }

    if(option->has_video || option->recieve_video){
        auto videoContent = std::make_unique<VideoContentDescription>();

        if(option->has_video && option->recieve_video){
            videoContent->SetMediaDirection(MediaSendRecv);
        }else if(option->has_video){
            videoContent->SetMediaDirection(MediaSendOnly);
        }else{
            videoContent->SetMediaDirection(MediaRecvOnly);
        }

        VideoCodec videoCodec(startCodecId++, "H264");
        videoCodec.AddDefalutParams();
        videoContent->AddCodec(videoCodec);

        if(videoContent->Direction() & MediaSendOnly)
            videoContent->GenerateSsrc();

        videoContent->AddExtMap(twccExtMap);

        videoContent->SetProtocol("UDP/TLS/RTP/SAVPF");

        TransportDescription transportDesc;
        transportDesc.AddOption(ICE_OPTION_TRICKLE);
//        transportDesc.SetIceMode(ICEMODE_LITE);
        transportDesc.SetConnectionRole(CONNECTIONROLE_ACTPASS);
        transportDesc.AddCandidate("127.0.0.1", 60000, "host");

        offer->AddContentInfo("video" , std::move(videoContent));
        offer->AddTransportInfo("video", transportDesc);
    }
    if(option->enable_data){
        auto dataContent = std::make_unique<SctpDataContentDescription>();

        dataContent->SetProtocol("UDP/DTLS/SCTP");

        TransportDescription transportDesc;
        transportDesc.AddOption(ICE_OPTION_TRICKLE);
//        transportDesc.SetIceMode(ICEMODE_LITE);
        transportDesc.SetConnectionRole(CONNECTIONROLE_ACTPASS);
        transportDesc.AddCandidate("127.0.0.1", 60000, "host");

        offer->AddContentInfo("data" , std::move(dataContent));
        offer->AddTransportInfo("data", transportDesc);

    }

    ContentGroup group("BUNDLE");
    for(const auto& content : offer->ContentInfos()){
        group.AddContentName(content.GetMid());
    }
    offer->AddContenGroup(group);

    return std::move(offer);
}

std::unique_ptr<SessionDescription> SessionDescriptionFactory::SdpDeSerilaize(const std::string &sdp)
{
    std::unique_ptr<SessionDescription> session_description = std::make_unique<SessionDescription>();

    TransportDescription transport_desc;
    std::vector<Extmap> extMaps;

    size_t line_pos = 0;
    if(!ParseSessionDescription(sdp, &line_pos, &transport_desc, session_description.get(), &extMaps)){
        ERROR("parse session description failed");
        session_description.reset();
        return std::move(session_description);
    }

    if(!ParseMediaDescription(sdp, &line_pos, &transport_desc, session_description.get(), &extMaps)){
        ERROR("parse media description failed");
        session_description.reset();
        return std::move(session_description);
    }

    return std::move(session_description);

}


bool SessionDescriptionFactory::ParseSessionDescription(const std::string &sdp, size_t* pos, TransportDescription *transport_desc,
                                                        SessionDescription *session_desc, std::vector<Extmap>* extMaps)
{
    std::string line;
    // RFC 4566
    // v=  (protocol version)
    if(!GetLineWithType(sdp, pos, &line, 'v')){
        ERROR("parse sdp version failed (%s)", line.c_str());
        return false;
    }

    //parse origin
    // o=<username> <sess-id> <sess-version> <nettype> <addrtype> <unicast-address>
    if(!GetLineWithType(sdp, pos, &line, 'o')){
        ERROR("parse sdp origin failed (%s)", line.c_str());
        return false;
    }

    auto origin = Util::Split(line.substr(2), " ");
    if(origin.size() != 6){
        ERROR("parse sdp origin failed (%s)", line.c_str());
        return false;
    }
    mSessionID = origin[1];
    mSessionVersion = origin[2];

    //parse session name
    // s=  (session name)
    if(!GetLineWithType(sdp, pos, &line, 's')){
        ERROR("parse sdp session name failed (%s)", line.c_str());
        return false;
    }

    // i=* (session information)
    GetLineWithType(sdp, pos, &line, 'i');

    // u=* (URI of description)
    GetLineWithType(sdp, pos, &line, 'u');

    // e=* (email address)
    GetLineWithType(sdp, pos, &line, 'e');

    // p=* (phone number)
    GetLineWithType(sdp, pos, &line, 'p');


    //parse session connection information
    // c=* (connection information -- not required if included in all media)
    if(GetLineWithType(sdp, pos, &line, 'c')){
        //ParseConnectionInfo(line);
    }

    //parse session bandwidth
    // b=* (zero or more bandwidth information lines)
    while(GetLineWithType(sdp, pos, &line, 'b')){
        // By pass zero or more b lines.
    }


    // One or more time descriptions ("t=" and "r=" lines; see below)
    // t=  (time the session is active)
    // r=* (zero or more repeat times)
    // Ensure there's at least one time description
    if(!GetLineWithType(sdp, pos, &line, 't')){
        ERROR("parse sdp session time failed (%s)", line.c_str());
        return false;
    }

    while(GetLineWithType(sdp, pos, &line, 'r')){
        // By pass zero or more r lines.
    }

    while(GetLineWithType(sdp, pos, &line, 't')){
        while(GetLineWithType(sdp, pos, &line, 'r')){
            // By pass zero or more r lines.
        }
    }


    //parse time zone
    // z=* (time zone adjustments)
    GetLineWithType(sdp, pos, &line, 'z');

    // parse encryption key
    // k=* (encryption key)
    GetLineWithType(sdp, pos, &line, 'k');


    //parse attributes
    while(GetLineWithType(sdp, pos, &line, 'a')){
        // a=<attribute>
        // a=<attribute>:<value>
        auto line_str = line.substr(2);

        std::string ice_ufrag;
        std::string ice_pwd;

        std::string attribute, value;
        std::string::size_type position = line_str.find(":");
        if(position != std::string::npos){
            attribute = line_str.substr(0, position);
            value = line_str.substr(position+1);
        }else{
            attribute = line_str;
        }

        if(attribute == "group"){
            auto fields = Util::Split(value, " ");
            if(fields.size() < 1){
                ERROR("pasre group filed (%s)", line.c_str());
                return false;
            }
            ContentGroup group(fields[0]);
            for(size_t i=1; i<fields.size(); ++i){
                group.AddContentName(fields[i]);
            }

            session_desc->AddContenGroup(group);
            INFO("AddContenGroup done");
        }else if(attribute == "ice-ufrag"){
            ice_ufrag = value;
            if(!ice_ufrag.empty() && !ice_pwd.empty()){
                transport_desc->SetIce(IceParameters(ice_ufrag, ice_pwd, false));
                ice_pwd.clear();
                ice_ufrag.clear();
            }
        }else if(attribute == "ice-pwd"){
            ice_pwd = value;
            if(!ice_ufrag.empty() && !ice_pwd.empty()){
                transport_desc->SetIce(IceParameters(ice_ufrag, ice_pwd, false));
                ice_pwd.clear();
                ice_ufrag.clear();
            }
        }else if(attribute == "ice-lite"){
            transport_desc->SetIceMode(ICEMODE_LITE);
        }else if(attribute == "ice-options"){
            auto fields = Util::Split(value, " ");
            for(const auto& option : fields){
                transport_desc->AddOption(option);
            }
        }else if(attribute == "fingerprint"){
            auto fingerprintParam = Util::Split(value, " ");
            if(fingerprintParam.size() != 2){
                ERROR("parse fingerprint failed (%s)", line.c_str());
                return false;
            }
            transport_desc->SetFingerprint(FingerprintParameters(fingerprintParam[0], fingerprintParam[1]));
        }else if(attribute == "setup"){
            transport_desc->SetConnectionRole(TransportDescription::StringToConnectionRole(value));
        }else if(attribute == "msid-semantic"){
            auto msid_param = Util::Split(value, " ");
            mMsidSemantic = msid_param[0];
            for(size_t i=1; i<msid_param.size();++i){
                mMsids.push_back(msid_param[i]);
            }
        }else if(attribute == "extmap-allow-mixed"){
            mExtMapMixed = true;
        }else if(attribute == "extmap"){
            Extmap extmap;
            if(!ParseExtmap(value, &extmap)){
                ERROR("parse extmap failed (%s)", line.c_str());
            }
            extMaps->push_back(extmap);
        }

    }

    return true;

}

bool SessionDescriptionFactory::ParseMediaDescription(const std::string &sdp, size_t *pos, TransportDescription *transport_desc, SessionDescription *session_desc, std::vector<Extmap> *extMaps)
{
    std::string line;
    int line_index = -1;

    // Zero or more media descriptions
    // RFC 4566
    // m=<media> <port> <proto> <fmt>
    while(GetLineWithType(sdp, pos, &line, 'm')){
        ++line_index;

        std::vector<std::string> fields = Util::Split(line.substr(2), " ");
        if(fields.size() < 4){
            ERROR("parse media description failed (%s)", line.c_str());
            return false;
        }

        int port = std::atoi(fields[1].c_str());

        std::vector<int> payload_types;
        for(size_t i=3; i< fields.size(); ++i){
            payload_types.push_back(std::atoi(fields[i].c_str()));
        }

        TransportDescription transport_description(*transport_desc);

        std::unique_ptr<MediaContentDescription> content;
        std::string content_name;
        bool bundle_only = false;
        const std::string& media_type = fields[0];

        if(media_type == "video"){
            content = std::make_unique<VideoContentDescription>();
            content->SetProtocol(fields[2]);
            for(const int payload : payload_types){
                content->AsVideo()->AddCodec(VideoCodec(payload));
            }

            if(!ParseContent(sdp, MEDIA_TYPE_VIDEO, pos,
                             &content_name, content.get(), &transport_description)){
                ERROR("parse video content failed");
                return false;
            }
        }else if(media_type == "audio"){
            content = std::make_unique<AudioContentDescription>();
            content->SetProtocol(fields[2]);
            for(const int payload : payload_types){
                content->AsAudio()->AddCodec(AudioCodec(payload));
            }

            if(!ParseContent(sdp, MEDIA_TYPE_AUDIO, pos,
                             &content_name, content.get(), &transport_description)){
                ERROR("parse audio content failed");
                return false;
            }
        }else if(media_type == "application"){
            content = std::make_unique<SctpDataContentDescription>();
            content->SetProtocol(fields[2]);

            if(!ParseContent(sdp, MEDIA_TYPE_DATA, pos,
                             &content_name, content.get(), &transport_description)){
                ERROR("parse sctp content failed");
                return false;
            }
        }


        session_desc->AddContentInfo(content_name, std::move(content));
        session_desc->AddTransportInfo(content_name, transport_description);

    }

    return true;
}

bool SessionDescriptionFactory::ParseExtmap(const std::string &line, Extmap *extMap)
{
    // RFC 5285
     // a=extmap:<value>["/"<direction>] <URI> <extensionattributes>
    std::vector<std::string> fields = Util::Split(line, " ");
    if(fields.size() < 2){
        return false;
    }

    extMap->id = std::atoi(fields[0].c_str());
    extMap->extmap = fields[1];
    return true;

}

bool SessionDescriptionFactory::ParseContent(const std::string &sdp, MediaType media_type,
                                             size_t *pos, std::string *content_name,
                                             MediaContentDescription *media_desc, TransportDescription *transport_desc)
{

    std::string line;
    std::string ice_ufrag;
    std::string ice_pwd;
    //循环直到下个media
    while(!IsLineType(sdp, 'm', *pos)){
         if(!GetLine(sdp, pos, &line)){
             if(*pos >= sdp.size()){
                 break;  //解析结束
             }else {
                 ERROR("failed to parse content (%s)", line.c_str());
                 return false;
             }
         }

         // RFC 4566
         // b=* (zero or more bandwidth information lines)
         if(IsLineType(line, 'b', 0)){
             continue;
         }


         if(IsLineType(line, 'c', 0)){
             continue;
         }

         if(!IsLineType(line, 'a', 0)){
             //不解析其他非attributes 信息
             continue;
         }


         // a=<attribute>
         // a=<attribute>:<value>
         auto line_str = line.substr(2);

         std::string attribute, value;
         std::string::size_type position = line_str.find(":");
         if(position != std::string::npos){
             attribute = line_str.substr(0, position);
             value = line_str.substr(position+1);
         }else{
             attribute = line_str;
         }

         if(attribute == "mid"){
            *content_name = value;
         }else if(attribute == "bundle-only"){
            //暂时不解析
         }else if(attribute == "candidate"){
            //暂时不解析
         }else if(attribute == "ice-ufrag"){
             ice_ufrag = value;
             if(!ice_ufrag.empty() && !ice_pwd.empty()){
                 transport_desc->SetIce(IceParameters(ice_ufrag, ice_pwd, false));
                 ice_pwd.clear();
                 ice_ufrag.clear();
             }
         }else if(attribute == "ice-pwd"){
             ice_pwd = value;
             if(!ice_ufrag.empty() && !ice_pwd.empty()){
                 transport_desc->SetIce(IceParameters(ice_ufrag, ice_pwd, false));
                 ice_pwd.clear();
                 ice_ufrag.clear();
             }
         }else if(attribute == "ice-lite"){
             transport_desc->SetIceMode(ICEMODE_LITE);
         }else if(attribute == "ice-options"){
             auto fields = Util::Split(value, " ");
             for(const auto& option : fields){
                 transport_desc->AddOption(option);
             }
         }else if(attribute == "fmtp"){
            if(media_type != MEDIA_TYPE_VIDEO && media_type != MEDIA_TYPE_AUDIO){
                continue;
            }

            std::string::size_type space_pos = value.find(" ");
            if(space_pos == std::string::npos){
                ERROR("parse fmtp failed (%s)", line.c_str());
                return false;
            }

            std::string paload, params;
            paload = value.substr(0, space_pos);
            params = value.substr(space_pos+1);

            auto vParams = Util::Split(params, ";");
            for(const auto& param: vParams){
                std::string::size_type equal_pos = param.find("=");
                if(equal_pos == std::string::npos){
                    ERROR("parse fmtp failed (%s)", line.c_str());
                    return false;
                }

                if(media_type == MEDIA_TYPE_VIDEO){
                    VideoCodec* videoCodec = media_desc->AsVideo()->FindCodecById(std::atoi(paload.c_str()));
                    if(videoCodec){
                        videoCodec->AddCodecParamter(param.substr(0, equal_pos), param.substr(equal_pos+1));
                    }else{
                        VideoCodec codec(std::atoi(paload.c_str()));
                        codec.AddCodecParamter(param.substr(0, equal_pos), param.substr(equal_pos+1));
                        media_desc->AsVideo()->AddCodec(codec);
                    }
                }else if(media_type == MEDIA_TYPE_AUDIO){
                    AudioCodec* audioCodec = media_desc->AsAudio()->FindCodecById(std::atoi(paload.c_str()));
                    if(audioCodec){
                        audioCodec->AddCodecParamter(param.substr(0, equal_pos), param.substr(equal_pos+1));
                    }else{
                        AudioCodec codec(std::atoi(paload.c_str()));
                        codec.AddCodecParamter(param.substr(0, equal_pos), param.substr(equal_pos+1));
                        media_desc->AsAudio()->AddCodec(codec);
                    }
                }

            }

         }else if(attribute == "fingerprint"){
             auto fingerprintParam = Util::Split(value, " ");
             if(fingerprintParam.size() != 2){
                 ERROR("parse fingerprint failed (%s)", line.c_str());
                 return false;
             }
             transport_desc->SetFingerprint(FingerprintParameters(fingerprintParam[0], fingerprintParam[1]));
         }else if(attribute == "setup"){
             transport_desc->SetConnectionRole(TransportDescription::StringToConnectionRole(value));
         }else if(media_type == MEDIA_TYPE_DATA){
            if(attribute == "sctp-port"){
                media_desc->AsSctp()->SetPort(std::atoi(value.c_str()));
            }else if(attribute == "max-message-size"){
                media_desc->AsSctp()->SetMaxMessageSize(std::atoi(value.c_str()));
            }else if(attribute == "sctpmap"){
                continue;
            }
         }else{ //media type
             if(attribute == "rtcp-mux"){
                 media_desc->SetRtcpMux(true);
             }else if(attribute == "rtcp-rsize"){
                 media_desc->SetRtcpReducedSize(true);
             }else if(attribute == "remote-net-estimate"){
                 continue;
             }else if(attribute == "ssrc-group"){
                 //暂时不解析
             }else if(attribute == "ssrc"){
                 //暂时不解析
             }else if(attribute == "crypto"){
                 continue;
             }else if(attribute == "rtpmap"){
                 std::string::size_type space_pos = value.find(" ");
                 if(space_pos == std::string::npos){
                     ERROR("parse rtpmap failed (%s)", line.c_str());
                     return false;
                 }

                 std::string paload, params;
                 paload = value.substr(0, space_pos);
                 params = value.substr(space_pos+1);

                 auto vParams = Util::Split(params, "/");

                 if(vParams.size() < 2){
                     ERROR("parse rtpmap failed (%s)", line.c_str());
                     return false;
                 }

                 if(media_type == MEDIA_TYPE_VIDEO && vParams.size() == 2){
                     VideoCodec* videoCodec = media_desc->AsVideo()->FindCodecById(std::atoi(paload.c_str()));
                     if(videoCodec){
                         videoCodec->mName = vParams[0];
                         videoCodec->mClockrate = std::atoi( vParams[1].c_str());
                     }else{
                         VideoCodec codec(std::atoi(paload.c_str()), vParams[0], std::atoi( vParams[1].c_str()));
                         media_desc->AsVideo()->AddCodec(codec);
                     }
                 }else if(media_type == MEDIA_TYPE_AUDIO && vParams.size() == 3){
                     AudioCodec* audioCodec = media_desc->AsAudio()->FindCodecById(std::atoi(paload.c_str()));
                     if(audioCodec){
                         audioCodec->mName = vParams[0];
                         audioCodec->mClockrate = std::atoi( vParams[1].c_str());
                         audioCodec->mChannels = std::atoi(vParams[2].c_str());
                     }else{
                         AudioCodec codec(std::atoi(paload.c_str()), vParams[0], std::atoi( vParams[2].c_str()), std::atoi( vParams[1].c_str()));
                         media_desc->AsAudio()->AddCodec(codec);
                     }
                 }

             }else if(attribute == "maxptime"){
                 continue;
             }else if(attribute == "packetization"){
                 continue;
             }else if(attribute == "rtcp-fb"){
                 auto vParams = Util::Split(value, " ");

                 if(vParams.size() < 2){
                     ERROR("parse rtcp-fb failed (%s)", line.c_str());
                     return false;
                 }

                 int codec_id = std::atoi(vParams[0].c_str());
                 std::string feedback_id = vParams[1];
                 std::string feedback_param = vParams.size()==3 ? vParams[2] : "";

                 if(media_type == MEDIA_TYPE_VIDEO){
                     VideoCodec* videoCodec = media_desc->AsVideo()->FindCodecById(codec_id);
                     if(videoCodec){
                         videoCodec->AddFeedbackParam(feedback_id, feedback_param);
                     }else{
                         VideoCodec codec(codec_id);
                         codec.AddFeedbackParam(feedback_id, feedback_param);
                         media_desc->AsVideo()->AddCodec(codec);
                     }
                 }else if(media_type == MEDIA_TYPE_AUDIO){
                     AudioCodec* audioCodec = media_desc->AsAudio()->FindCodecById(codec_id);
                     if(audioCodec){
                         audioCodec->AddFeedbackParam(feedback_id, feedback_param);
                     }else{
                         AudioCodec codec(codec_id);
                         codec.AddFeedbackParam(feedback_id, feedback_param);
                         media_desc->AsAudio()->AddCodec(codec);
                     }
                 }
             }else if(attribute == "ptime"){
                 continue;
             }else if(attribute == "sendonly"){
                 media_desc->SetMediaDirection(MediaSendOnly);
             }else if(attribute == "recvonly"){
                 media_desc->SetMediaDirection(MediaRecvOnly);
             }else if(attribute == "sendrecv"){
                 media_desc->SetMediaDirection(MediaSendRecv);
             }else if(attribute == "inactive"){
                 media_desc->SetMediaDirection(MediaInactive);
             }else if(attribute == "extmap-allow-mixed"){
                 continue;
             }else if(attribute == "extmap"){
                 Extmap extmap;
                 if(!ParseExtmap(value, &extmap)){
                     ERROR("parse extmap failed (%s)", line.c_str());
                     return false;
                 }
                 media_desc->AddExtMap(extmap);
             }else if(attribute == "x-google-flag"){
                 continue;
             }else if(attribute == "msid"){
                 continue;
             }else if(attribute == "rid"){
                 continue;
             }else if(attribute == "simulcast"){
                 continue;
             }else if(attribute == "rtcp"){
                 continue;
             }
         }


    }

    return true;
}

bool SessionDescriptionFactory::GetLineWithType(const std::string &message, size_t *pos, std::string *line, const char type)
{
    if(!IsLineType(message, type, *pos)){
        return false;
    }

    if(!GetLine(message, pos, line)){
        return false;
    }

    return true;
}

bool SessionDescriptionFactory::IsLineType(const std::string &message, const char type, size_t line_start)
{
    if(message.size() < line_start + 2){ // a=   v=  ...
        return false;
    }

    return (message[line_start] == type && message[line_start+1] == '=');
}

bool SessionDescriptionFactory::GetLine(const std::string &message, size_t *pos, std::string *line)
{
    size_t line_begin = *pos;
    size_t line_end = message.find("\n", line_begin);

    if(line_end == std::string::npos){
        return false;
    }

    *pos = line_end + 1;

    if(line_end > 0 && (message.at(line_end - 1) == '\r')){  //去除'\r'
        --line_end;
    }

    *line = message.substr(line_begin, (line_end - line_begin));

    return true;
}

bool SessionDescriptionFactory::HasAttribute(const std::string &line, const std::string &attribute)
{
    if(line.compare(2, attribute.size(), attribute) == 0){
        if((2 + attribute.size()) == line.size() ||
                line[2+attribute.size()] == ':' ||
                line[2+attribute.size()] == ' ' ){
            return true;
        }
    }

    return false;
}
