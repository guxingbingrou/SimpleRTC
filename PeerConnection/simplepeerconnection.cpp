#include "simplepeerconnection.h"

SimplePeerConnection::SimplePeerConnection()
{

}

std::string SimplePeerConnection::CreateOffer(const SessionDescriptionOption *option)
{
    std::shared_ptr<SimpleSessionDescription> session_description = std::make_shared<SimpleSessionDescription>();

    return "";
}

std::string SimplePeerConnection::CreateAnswer(const SessionDescriptionOption *option)
{
    return "";
}
