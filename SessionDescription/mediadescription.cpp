#include "mediadescription.h"
#include <sstream>
#include <QDebug>
MediaDescription::MediaDescription(std::string type)
    :mMediaType(type)
{
    InitMediaDescription();
}

void MediaDescription::GenerateMediaProto(std::string proto)
{
    mProto = proto;
}

void MediaDescription::GenerateMediaFmts(std::string fmt)
{
    mFmt = fmt;
    if(mFmt == "111"){
        mCodecName = "opus";
        mClockRate = 48000;
        mChannels = 2;
    }else if(mFmt == "106"){
        mCodecName = "H264";
        mClockRate = 90000;
    }

    std::vector<std::string> v_ftcp_fb;
    std::vector<std::string> v_fmtp;
    mFtcpFbMap[mFmt] = v_ftcp_fb;
    mFmtpMap[mFmt] = v_fmtp;
}

void MediaDescription::GenereateMediaDirection(MediaDirection direction)
{
    mMediaDirection = direction;
}

void MediaDescription::GenerateRtcpMux(bool rtcp_mux)
{
    mRtcpMux = rtcp_mux;
}

void MediaDescription::GenerateRtcpRsize(bool rtcp_rsize)
{
    mRtcpRsize = rtcp_rsize;
}

void MediaDescription::GenerateMid(std::string mid)
{
    mMid = mid;
}

void MediaDescription::GenerateSctpPort(bool sctp_port)
{
    mSctpPort = sctp_port;
}

void MediaDescription::GenerateIceParams(std::string ufrg, std::string pwd, std::string option)
{
    mIceUfrag = ufrg;
    mIcePwd = pwd;
    mIceOption = option;
}

void MediaDescription::GenerateFingerPrint(std::string type, std::string fingerprint)
{
    mFingerPrintType = type;
    mFingerPrint = fingerprint;
}

void MediaDescription::GenerateSetup(std::string setup)
{
    mSetup = setup;
}

void MediaDescription::GenerateSsrc(std::string label, std::string cname, std::string ssrc)
{
    mSsrc = ssrc;
    mSsrcCname = cname;
    mSsrcLabel = label;
}

void MediaDescription::InsertCandidate(std::string ip, int port, std::string type)
{
    mCandidates.emplace_back(port, ip, type);
}

void MediaDescription::InsertExtmap(int id, std::string extmap)
{
    mExtmaps.emplace_back(id, extmap);
}

void MediaDescription::InsertRtcpFb(std::string fmt, std::string rtcp_fb)
{
    if(mFtcpFbMap.find(fmt) != mFtcpFbMap.end()){
        auto& v = mFtcpFbMap.at(fmt);
        v.push_back(rtcp_fb);
    }
}

void MediaDescription::InsertFmtp(std::string fmt, std::string fmtp)
{
    if(mFmtpMap.find(fmt) != mFmtpMap.end()){
        auto& v = mFmtpMap.at(fmt);
        v.push_back(fmtp);
    }
}

const std::string MediaDescription::GetMid()
{
    return mMid;
}

std::string MediaDescription::Serilalize()
{
    std::string message;
    std::stringstream ss;
    // RFC 4566
    // m=<media> <port> <proto> <fmt>
    ss << "m=" << mMediaType << " " << mPort << " " << mProto << " " << mFmt << "\r\n";
    message.append(ss.str());

    message.append("c=IN IP4 0.0.0.0\r\n");

    //session info
    ss.str("");
    ss << "a=ice-ufrag:" << mIceUfrag << "\r\n";
    message.append(ss.str());

    ss.str("");
    ss << "a=ice-pwd:" << mIcePwd << "\r\n";
    message.append(ss.str());

    ss.str("");
    ss << "a=ice-options:" << mIceOption << "\r\n";
    message.append(ss.str());

    ss.str("");
    ss << "a=fingerprint:" << mFingerPrintType << " " << mFingerPrint << "\r\n";
    message.append(ss.str());

    ss.str("");
    ss << "a=setup:" << mSetup << "\r\n";
    message.append(ss.str());


    //mid
    ss.str("");
    ss << "a=mid:" << mMid << "\r\n";
    message.append(ss.str());


    //extmap
    ss.str("");
    for(auto& map : mExtmaps){
        ss << "a=extmap:" << map.id << " " << map.extmap << "\r\n";
    }

    message.append(ss.str());

    if(mMediaType != "application"){
        //direction
        ss.str("");
        ss << "a=";
        switch (mMediaDirection) {
            case MediaSendOnly:
                ss << "sendonly" << "\r\n";
            break;
            case MediaRecvOnly:
                ss << "recvonly" << "\r\n";
            break;
            case MediaSendRecv:
                ss << "sendrecv" << "\r\n";
            break;
            case MediaInactive:
                ss << "inactive" << "\r\n";
            break;
        }
        message.append(ss.str());
    }

    //rtcp-mux
    if(mRtcpMux)
        message.append("a=rtcp-mux\r\n");

    //rtcp-rsize
    if(mRtcpRsize)
        message.append("a=rtcp-rsize\r\n");

    //rtpmap
    if(mMediaType != "application"){
        ss.str("");
        ss << "a=rtpmap:" << mFmt << " " << mCodecName << "/" << mClockRate;
        if(mChannels != 0)
            ss << "/" << mChannels;
        ss << "\r\n";
        message.append(ss.str());
    }

    //rtcp-fb
    ss.str("");
    for(auto& fb : mFtcpFbMap){
        for(auto& fb_str : fb.second){
            ss << "a=rtcp-fb:" << fb.first << " " << fb_str << "\r\n";
        }
    }
    message.append(ss.str());

    //fmtp
    ss.str("");
    for(auto& fmtp : mFmtpMap){
        for(auto& fmtp_str : fmtp.second){
            ss << "a=fmtp:" << fmtp.first << " " << fmtp_str << "\r\n";
        }
    }
    message.append(ss.str());

    //ssrc
    if(mMediaType != "application" && (mMediaDirection == MediaSendRecv || mMediaDirection == MediaSendOnly)){

        ss.str("");
        ss << "a=ssrc:" << mSsrc << " " << "cname:" << mSsrcCname << "\r\n";
        ss << "a=ssrc:" << mSsrc << " " << "label:" << mSsrcCname << "\r\n";
        message.append(ss.str());
    }

    //candidate
    int32_t foundation = 0;
    int32_t component_id = 1; /* RTP */
    uint32_t priority = (1 << 24) * (126) + (1 << 8) * (65535)
            + (1) * (256 - component_id);
    ss.str("");
    for(auto& candidate: mCandidates){
        ss << "a=candidate:" << foundation++ << " " << component_id << " " << "udp" << " "
           << priority << " " << candidate.candidateIp << " " << candidate.candidatePort
           << " typ " << candidate.candidateType << " generation 0" << "\r\n";
    }
    message.append(ss.str());

    qDebug() << QString::fromStdString(ss.str());

    if(mSctpPort){
        message.append("a=sctp-port:5000\r\n");
    }

    return message;

}

void MediaDescription::InitMediaDescription()
{
    mPort = 9;
}
