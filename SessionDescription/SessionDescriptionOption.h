#ifndef SESSIONDESCIPTIONOPTION_H
#define SESSIONDESCIPTIONOPTION_H


struct SessionDescriptionOption{
    bool recieve_audio = true;
    bool recieve_video = true;
    bool has_audio = true;
    bool has_video = true;
    bool enable_data = true;
    bool enable_dtls = false;
};

#endif // SESSIONDESCIPTIONOPTION_H
