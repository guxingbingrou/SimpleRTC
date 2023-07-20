#include "SimplePeerConnection.h"

SimplePeerConnection::SimplePeerConnection()
{

}

std::string SimplePeerConnection::CreateOffer(const SessionDescriptionOption *option)
{
    auto session_description_factory = std::make_unique<SessionDescriptionFactory>();
    session_description_factory->CreateOffer(option);

    return "";
}

std::string SimplePeerConnection::CreateAnswer(const SessionDescriptionOption *option)
{
    return "";
}
