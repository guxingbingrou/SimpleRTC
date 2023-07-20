#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <string>

enum VideoCodecType{
    SimpleH264 = 0,
    SimpleH265,
    SimpleNone
};

enum VideoFrameFormat{
    SimpleFrameFormatRGBA = 0,
    SimpleFrameFormatI420,
    SimpleFrameFormatNV12,
    SimpleFrameFormatNV21,
    SimpleFrameFormatNone
};

class Util
{
public:

    static std::vector<std::string> Split(std::string src, std::string delim);

private:
    Util();
};

#endif // UTIL_H
