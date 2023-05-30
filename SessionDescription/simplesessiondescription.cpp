#include "simplesessiondescription.h"
#include <sstream>

SimpleSessionDescription::SimpleSessionDescription()
{

}

void SimpleSessionDescription::GenerateMediaDescriptions(const SessionDescriptionOption *option)
{
    int midCount = 0;

    std::string ice_ufrag = "jMOp";
    std::string ice_pwd = "c9A8m6UYP55CG8EYlp8FnKMG";
    std::string ice_option = "trickle";

    std::string fingerprint_type = "sha-256";
    std::string fingerprint = "1A:F4:66:52:47:29:14:5A:7E:07:E1:E9:2E:04:C2:CD:6B:65:E5:66:C3:29:B0:75:C5:6B:17:3E:F2:CB:CF:B3";

    std::string setup = "actpass";

    std::string twccExt = "http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01";
    int twccID = 3;

    if(option->has_audio || option->recieve_audio){
        auto media_session = std::make_shared<MediaDescription>("audio");
        mMediaDescriptions.push_back(media_session);

        media_session->GenerateMediaProto("UDP/TLS/RTP/SAVPF");
        media_session->GenerateMediaFmts("111");  //opus

        media_session->InsertRtcpFb("111", "transport-cc");

        if(option->has_audio && option->recieve_audio){
            media_session->GenereateMediaDirection(MediaSendRecv);
        }else if(option->has_audio){
            media_session->GenereateMediaDirection(MediaSendOnly);
        }else{
            media_session->GenereateMediaDirection(MediaRecvOnly);
        }

        media_session->GenerateRtcpMux(true);
        media_session->GenerateRtcpRsize(true);

        media_session->GenerateMid("audio");

        media_session->GenerateIceParams(ice_ufrag, ice_pwd, ice_option);

        media_session->GenerateFingerPrint(fingerprint_type, fingerprint);

        media_session->GenerateSetup(setup);

        media_session->InsertCandidate("127.0.0.1", 60000, "host");

        media_session->InsertExtmap(twccID, twccExt);

        media_session->GenerateSsrc("master", "4vJSbSDqMvFZSh1D", "1234567890");
    }

    if(option->has_video || option->recieve_video){
        auto media_session = std::make_shared<MediaDescription>("video");
        mMediaDescriptions.push_back(media_session);

        media_session->GenerateMediaProto("UDP/TLS/RTP/SAVPF");
        media_session->GenerateMediaFmts("106");  //h264

        media_session->InsertRtcpFb("106", "transport-cc");
        media_session->InsertRtcpFb("106", "nack");
        media_session->InsertRtcpFb("106", "nack pli");

        media_session->InsertFmtp("106", "level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f");

        if(option->has_video && option->recieve_video){
            media_session->GenereateMediaDirection(MediaSendRecv);
        }else if(option->has_video){
            media_session->GenereateMediaDirection(MediaSendOnly);
        }else{
            media_session->GenereateMediaDirection(MediaRecvOnly);
        }

        media_session->GenerateRtcpMux(true);
        media_session->GenerateRtcpRsize(true);

        media_session->GenerateMid("video");

        media_session->GenerateIceParams(ice_ufrag, ice_pwd, ice_option);

        media_session->GenerateFingerPrint(fingerprint_type, fingerprint);

        media_session->GenerateSetup(setup);

        media_session->InsertCandidate("127.0.0.1", 60000, "host");

        media_session->InsertExtmap(twccID, twccExt);

        media_session->GenerateSsrc("master", "4vJSbSDqMvFZSh1D", "1234567891");
    }

    if(option->enable_data){
        auto media_session = std::make_shared<MediaDescription>("application");
        mMediaDescriptions.push_back(media_session);

        media_session->GenerateMediaProto("UDP/TLS/RTP/SAVPF");
        media_session->GenerateMediaFmts("webrtc-datachannel");  //opus


        media_session->GenerateRtcpMux(false);
        media_session->GenerateRtcpRsize(false);

        media_session->GenerateMid("data");

        media_session->GenerateSctpPort(true);

        media_session->GenerateIceParams(ice_ufrag, ice_pwd, ice_option);

        media_session->GenerateFingerPrint(fingerprint_type, fingerprint);

        media_session->GenerateSetup(setup);

        media_session->InsertCandidate("127.0.0.1", 60000, "host");
    }
}

std::string SimpleSessionDescription::SdpSerilalize()
{
    std::string message;

    //session description
    message.append("v=0\r\n");

    std::stringstream ss;
    long long session_id = 94320512513472;
    ss << "o=simpleRTC " << session_id << " 2 " << "IN IP4 0.0.0.0" << "\r\n";

    message.append(ss.str());

    message.append("s=SimpleRTCSession\r\n");

    message.append("t=0 0\r\n");

    message.append("a=ice-lite\r\n");

    // BUNDLE Groups
    ss.str("");
    ss << "a=group:BUNDLE";
    for(auto& md : mMediaDescriptions){
        ss << " " << md->GetMid();
    }
    ss << "\r\n";
    message.append(ss.str());

    // MediaStream semantics
    message.append("a=msid-semantic: WMS\r\n");


    //media description
    for(auto& mds : mMediaDescriptions){
        message.append(mds->Serilalize());
    }


    return message;
}
