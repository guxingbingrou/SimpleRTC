#ifndef SIMPLEPEERCONNECTION_H
#define SIMPLEPEERCONNECTION_H

#include <string>
#include <memory>
#include "SessionDescription/SessionDesciptionOption.h"
#include "SessionDescription/simplesessiondescription.h"
class SimplePeerConnection
{
public:
    SimplePeerConnection();

    std::string CreateOffer(const SessionDescriptionOption* option);
    std::string CreateAnswer(const SessionDescriptionOption* option);

    bool SetLocalDescription(std::shared_ptr<SimpleSessionDescription> session_description);
    bool SetRemoteDescription(std::shared_ptr<SimpleSessionDescription> session_description);


private:


};

#endif // SIMPLEPEERCONNECTION_H
