#ifndef STREAMPARAMS_H
#define STREAMPARAMS_H
#include <string>
#include <vector>

struct SsrcGroup{
    std::string mSemantics;
    std::vector<uint32_t> mSsrcs;
};

struct SsrcInfo{
    std::string mCname = "SimpleRTC";
    uint32_t mSsrc;
    std::string mMsid;
    std::string mMsidTracker;
    std::string mMslabel;
    std::string mLabel = "master";
};

class SsrcGenerator
{
public:
    static uint32_t GenerateSsrc();
private:
    static uint32_t ssrcBase;
};

#endif // STREAMPARAMS_H
