#ifndef SIMPLEPEERCONNECTION_H
#define SIMPLEPEERCONNECTION_H

#include <string>
#include <memory>
#include "SessionDescription/SessionDescriptionOption.h"
#include "SessionDescription/SessionDescriptionFactory.h"
class SimplePeerConnection
{
public:
    SimplePeerConnection();

    std::string CreateOffer(const SessionDescriptionOption* option);
    std::string CreateAnswer(const SessionDescriptionOption* option);

    bool SetLocalDescription(std::shared_ptr<SessionDescriptionFactory> session_description);
    bool SetRemoteDescription(std::shared_ptr<SessionDescriptionFactory> session_description);


private:


};

#endif // SIMPLEPEERCONNECTION_H
