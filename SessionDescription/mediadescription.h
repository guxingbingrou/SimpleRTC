#ifndef MEDIADESCRIPTION_H
#define MEDIADESCRIPTION_H

#include <string>
#include <vector>
#include <unordered_map>

enum MediaDirection{
    MediaSendOnly,
    MediaRecvOnly,
    MediaSendRecv,
    MediaInactive
};

class MediaDescription
{
public:
    MediaDescription(std::string type);

    void GenerateMediaProto(std::string proto);

    void GenerateMediaFmts(std::string fmt);

    void GenereateMediaDirection(MediaDirection direction);

    void GenerateRtcpMux(bool rtcp_mux);

    void GenerateRtcpRsize(bool rtcp_rsize);

    void GenerateMid(std::string mid);

    void GenerateSctpPort(bool sctp_port);

    void GenerateIceParams(std::string ufrg, std::string pwd, std::string option);

    void GenerateFingerPrint(std::string type, std::string fingerprint);

    void GenerateSetup(std::string setup);

    void GenerateSsrc(std::string label, std::string cname, std::string ssrc);

    void InsertCandidate(std::string ip, int port, std::string type);

    void InsertExtmap(int id, std::string extmap);

    void InsertRtcpFb(std::string fmt, std::string rtcp_fb);

    void InsertFmtp(std::string fmt, std::string fmtp);


    const std::string GetMid();

    std::string Serilalize();

private:
    void InitMediaDescription();

    std::string mMediaType;
    int mPort;
    std::string mProto;
    std::string mFmt;
    std::string mCodecName;
    int mChannels = 0;
    int mClockRate = 0;

    MediaDirection mMediaDirection;

    bool mRtcpMux = false;           //Multiplexing RTP Data and Control Packets on a Single Port    RFC 5761
    bool mRtcpRsize = false;         //An a=rtcp-rsize attribute present at the SDP session-level MUST be ignored
    bool mSctpPort = false;

    std::string mIceUfrag;
    std::string mIcePwd;
    std::string mIceOption;

    std::string mFingerPrintType;
    std::string mFingerPrint;

    std::string mSetup;  //表示dtls的协商过程中角色的问题，谁是客户端，谁是服务器 actpass\active(客户端)\passive(服务端)   rfc4145

    std::string mSsrc;
    std::string mSsrcLabel;
    std::string mSsrcCname;

    struct CandidateParam{
        CandidateParam(int port, std::string ip, std::string type){
            candidatePort = port;
            candidateIp = ip;
            candidateType = type;
        }
        int candidatePort;
        std::string candidateIp;
        std::string candidateType;
    };

    std::vector<CandidateParam> mCandidates;

    struct Extmap{
        Extmap(int id, std::string extmap){
            this->id = id;
            this->extmap = extmap;
        }
        int id;
        std::string extmap;
    };

    std::vector<Extmap> mExtmaps;

    std::unordered_map<std::string, std::vector<std::string>> mFtcpFbMap;  //first-fmt, second-ftcpfbs

    std::unordered_map<std::string, std::vector<std::string>> mFmtpMap;  //first-fmt, second-fmtps
    std::string mMid;


};

#endif // MEDIADESCRIPTION_H
