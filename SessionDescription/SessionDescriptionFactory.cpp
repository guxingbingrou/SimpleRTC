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

        audioContent->AddCodec(AudioCodec(startCodecId++, "opus", 2));

        if(audioContent->Direction() & MediaSendOnly)
            audioContent->GenerateSsrc();

        audioContent->AddExtMap(twccExtMap);

        audioContent->SetProtocol("UDP/TLS/RTP/SAVPF");

        TransportDescription transportDesc;
        transportDesc.AddOption(ICE_OPTION_TRICKLE);
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

        videoContent->AddCodec(VideoCodec(startCodecId++, "H264"));

        if(videoContent->Direction() & MediaSendOnly)
            videoContent->GenerateSsrc();

        videoContent->AddExtMap(twccExtMap);

        videoContent->SetProtocol("UDP/TLS/RTP/SAVPF");

        TransportDescription transportDesc;
        transportDesc.AddOption(ICE_OPTION_TRICKLE);
        transportDesc.SetConnectionRole(CONNECTIONROLE_ACTPASS);
        transportDesc.AddCandidate("127.0.0.1", 60000, "host");

        offer->AddContentInfo("video" , std::move(videoContent));
        offer->AddTransportInfo("video", transportDesc);
    }
    if(option->enable_data){
        auto dataContent = std::make_unique<SctpDataContentDescription>();

        dataContent->SetProtocol("UDP/TLS/RTP/SAVPF");

        TransportDescription transportDesc;
        transportDesc.AddOption(ICE_OPTION_TRICKLE);
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


//bool SessionDescriptionFactory::SdpDeSerilaize(const std::string &sdp)
//{
//    std::vector<std::string> split = Util::Split(sdp, "\r\n");
//    int line = 0;

//    auto JudgeSize = [=](){
//        if( line >= split.size()){
//            WARNING("line bigger than split size");
//            return false;
//        }else if(split[line].size() < 2){
//            ERROR("line size less than 2");
//            return false;
//        }else{
//            return true;
//        }
//    };

//    //parse version
//    if(!JudgeSize()) return false;
//    // v=  (protocol version)
//    if(split[line][0] != 'v' || split[line][1] != '='){
//        ERROR("parse sdp version failed (%s)", split[line].c_str());
//        return false;
//    }
//    ++line;


//    //parse origin
//    if(!JudgeSize()) return false;
//    // o=<username> <sess-id> <sess-version> <nettype> <addrtype> <unicast-address>
//    if(split[line][0] != 'o' || split[line][1] != '='){
//        ERROR("parse sdp origin failed (%s)", split[line].c_str());
//        return false;
//    }
//    auto origin = Util::Split(split[line].substr(2), " ");
//    if(origin.size() != 6){
//        ERROR("parse sdp origin failed (%s)", split[line].c_str());
//        return false;
//    }
//    mSessionID = origin[1];
//    mSessionVersion = origin[2];
//    ++line;


//    //parse session name
//    if(!JudgeSize()) return false;
//    // s=  (session name)
//    if(split[line][0] != 's' || split[line][1] != '='){
//        ERROR("parse sdp session name failed (%s)", split[line].c_str());
//        return false;
//    }
//    ++line;


//    //parse session information
//    if(!JudgeSize()) return false;
//    // i=* (session information)
//    if(split[line][0] == 'i' && split[line][1] == '='){
//        ++line;
//    }

//    //parse session uri
//    if(!JudgeSize()) return false;
//    // u=* (URI of description)
//    if(split[line][0] == 'u' && split[line][1] == '='){
//        ++line;
//    }

//    //parse session email
//    if(!JudgeSize()) return false;
//    // e=* (email address)
//    if(split[line][0] == 'e' && split[line][1] == '='){
//        ++line;
//    }

//    //parse session phone number
//    if(!JudgeSize()) return false;
//    // p=* (phone number)
//    if(split[line][0] == 'p' && split[line][1] == '='){
//        ++line;
//    }

//    //parse session connection information
//    if(!JudgeSize()) return false;
//    // c=* (connection information -- not required if included in all media)
//    if(split[line][0] == 'c' && split[line][1] == '='){
//        ParseConnectionInfo(split[line]);
//        ++line;
//    }

//    //parse session bandwidth
//    // b=* (zero or more bandwidth information lines)
//    while(JudgeSize() && split[line][0] == 'b' && split[line][1] == '='){
//        ++line;
//    }


//    // One or more time descriptions ("t=" and "r=" lines; see below)
//    //parse time
//    if(!JudgeSize()) return false;
//    // t=  (time the session is active)
//    // r=* (zero or more repeat times)
//    // Ensure there's at least one time description
//    if(split[line][0] != 't' || split[line][1] != '='){
//        ERROR("parse sdp session name failed (%s)", split[line].c_str());
//        return false;
//    }
//    ++line;
//    while(JudgeSize() && split[line][0] == 'r' && split[line][1] == '='){
//        ++line;
//    }

//    while(JudgeSize() && split[line][0] == 't' && split[line][1] == '='){
//        ++line;
//        while(JudgeSize() && split[line][0] == 'r' && split[line][1] == '='){
//            ++line;
//        }
//    }


//    //parse time zone
//    if(!JudgeSize()) return false;
//    // z=* (time zone adjustments)
//    if(split[line][0] == 'z' && split[line][1] == '='){
//        ++line;
//    }

//    //parse encryption key
//    if(!JudgeSize()) return false;
//    // k=* (encryption key)
//    if(split[line][0] == 'k' && split[line][1] == '='){
//        ++line;
//    }


//    //parse attributes
//    while(JudgeSize() && split[line][0] == 'a' && split[line][1] == '='){
//        // a=<attribute>
//        // a=<attribute>:<value>
//        auto line_str = split[line].substr(2);
//        std::string attribute, value;
//        std::string::size_type pos = line_str.find(":");
//        if(pos != std::string::npos){
//            attribute = line_str.substr(0, pos);
//            value = line_str.substr(pos+1);
//        }else{
//            attribute = line_str;
//        }

//        if(attribute == "group"){
//            mGroup = Util::Split(value, " ");
//        }else if(attribute == "ice-ufrag"){
//            mIceUfrag = value;
//        }else if(attribute == "ice-pwd"){
//            mIcePwd = value;
//        }else if(attribute == "ice-lite"){
//            mIceLite = true;
//        }else if(attribute == "ice-options"){
//            mIceOption = value;
//        }else if(attribute == "fingerprint"){
//            auto fingerprintParam = Util::Split(value, " ");
//            if(fingerprintParam.size() != 2){
//                ERROR("parse fingerprint failed (%s)", split[line].c_str());
//                return false;
//            }
//            mFingerPrintType = fingerprintParam[0];
//            mFingerPrint = fingerprintParam[1];
//        }else if(attribute == "setup"){
//            mSetup = value;
//        }else if(attribute == "msid-semantic"){
//            auto msid_param = Util::Split(value, " ");
//            mMsidSemantic = msid_param[0];
//            for(int i=1; i<msid_param.size();++i){
//                mMsids.push_back(msid_param[i]);
//            }
//        }else if(attribute == "extmap-allow-mixed"){
//            mExtMapMixed = true;
//        }else if(attribute == "extmap"){
//            //TODO parse extmap
//        }


//        ++line;
//    }

//    //parse media session
//    // m=<media> <port> <proto> <fmt>
//    while(line>=0 && JudgeSize() && split[line][0] == 'm' && split[line][1] == '='){
//        auto mdp = std::make_shared<MediaDescription>();
//        mMediaDescriptions.push_back(mdp);
//        line = mdp->Deserialize(split, line);
//    }



//    return true;
//}


bool SessionDescriptionFactory::ParseConnectionInfo(const std::string &message)
{
    return true;
}
