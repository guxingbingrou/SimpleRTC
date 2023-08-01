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

    std::unique_ptr<SessionDescription> SdpDeSerilaize(const std::string& sdp);


private:
    bool ParseSessionDescription(const std::string& sdp, size_t* pos,
                                 TransportDescription* transport_desc, SessionDescription* session_desc, std::vector<Extmap>* extMaps);

    bool ParseMediaDescription(const std::string& sdp, size_t* pos,
                                 TransportDescription* transport_desc, SessionDescription* session_desc, std::vector<Extmap>* extMaps);

    bool ParseExtmap(const std::string& line, Extmap* extMap);

    bool ParseContent(const std::string& sdp, MediaType media_type,
                      size_t* pos, std::string* content_name, MediaContentDescription* media_desc, TransportDescription* transport_desc);

    bool GetLineWithType(const std::string& message, size_t* pos, std::string* line, const char type);
    bool IsLineType(const std::string& message, const char type, size_t line_start);
    bool GetLine(const std::string& message, size_t* pos, std::string* line);

    bool HasAttribute(const std::string& line, const std::string& attribute);

    std::string mSessionID;
    std::string mSessionVersion;

    std::string mSetup;  //表示dtls的协商过程中角色的问题，谁是客户端，谁是服务器 actpass\active(客户端)\passive(服务端)   rfc4145

    std::string mMsidSemantic;
    std::vector<std::string> mMsids;

    bool mExtMapMixed = false;;

};

#endif // SIMALESESSIONDESCRIPTION_H
