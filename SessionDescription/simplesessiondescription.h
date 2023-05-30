#ifndef SIMALESESSIONDESCRIPTION_H
#define SIMALESESSIONDESCRIPTION_H

#include <string>
#include <vector>
#include <memory>
#include "mediadescription.h"
#include "SessionDesciptionOption.h"

class SimpleSessionDescription
{
public:
    SimpleSessionDescription();

    void GenerateMediaDescriptions(const SessionDescriptionOption* option);

    std::string SdpSerilalize();


private:
    std::vector< std::shared_ptr<MediaDescription> > mMediaDescriptions;

};

#endif // SIMALESESSIONDESCRIPTION_H
