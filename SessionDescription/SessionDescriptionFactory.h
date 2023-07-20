#ifndef SIMALESESSIONDESCRIPTION_H
#define SIMALESESSIONDESCRIPTION_H

#include <string>
#include <vector>
#include <memory>
#include "SessionDescriptionOption.h"
#include "SessionDescription.h"

class SessionDescriptionFactory
{
public:
    SessionDescriptionFactory();

    std::unique_ptr<SessionDescription> CreateOffer(const SessionDescriptionOption* option);

//    bool SdpDeSerilaize(const std::string& sdp);


private:
    bool ParseConnectionInfo(const std::string& message);

    std::string mSessionID;
    std::string mSessionVersion;

    std::vector<std::string> mGroup;

    //1、ICE候选地址，媒体服务器只收集host类型的地址
    //2、lite ice端(服务端)不主动进行连通性测试，由full ice端(客户端)发起
    bool mIceLite = true;

    std::string mIceUfrag;
    std::string mIcePwd;
    std::string mIceOption;

    std::string mFingerPrintType;
    std::string mFingerPrint;

    std::string mSetup;  //表示dtls的协商过程中角色的问题，谁是客户端，谁是服务器 actpass\active(客户端)\passive(服务端)   rfc4145

    std::string mMsidSemantic;
    std::vector<std::string> mMsids;

    bool mExtMapMixed = false;;

};

#endif // SIMALESESSIONDESCRIPTION_H
