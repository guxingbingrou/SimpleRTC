#ifndef TRANSPORTDESCRIPTION_H
#define TRANSPORTDESCRIPTION_H

#include <string>
#include <vector>
// ICE RFC 5245 implementation type.
enum IceMode {
  ICEMODE_FULL,  // As defined in http://tools.ietf.org/html/rfc5245#section-4.1
  ICEMODE_LITE   // As defined in http://tools.ietf.org/html/rfc5245#section-4.2
};

// RFC 4145 - http://tools.ietf.org/html/rfc4145#section-4
// 'active':  The endpoint will initiate an outgoing connection.
// 'passive': The endpoint will accept an incoming connection.
// 'actpass': The endpoint is willing to accept an incoming
//            connection or to initiate an outgoing connection.
enum ConnectionRole {
  CONNECTIONROLE_NONE = 0,
  CONNECTIONROLE_ACTIVE,
  CONNECTIONROLE_PASSIVE,
  CONNECTIONROLE_ACTPASS,
  CONNECTIONROLE_HOLDCONN,
};

constexpr auto* ICE_OPTION_TRICKLE = "trickle";
constexpr auto* ICE_OPTION_RENOMINATION = "renomination";

struct IceParameters{
    IceParameters() = default;
    IceParameters(const std::string& ice_ufrag,
                  const std::string& ice_pwd,
                  bool ice_renomination)
        : mUfrag(ice_ufrag), mPwd(ice_pwd), mRenomination(ice_renomination) {}
    std::string mUfrag = "jMOp";
    std::string mPwd = "c9A8m6UYP55CG8EYlp8FnKMG";
    bool mRenomination = false;
};


struct FingerprintParameters{
    std::string mAlgorithm = "sha-256";
    std::string mFingerPrint = "1A:F4:66:52:47:29:14:5A:7E:07:E1:E9:2E:04:C2:CD:6B:65:E5:66:C3:29:B0:75:C5:6B:17:3E:F2:CB:CF:B3";
};


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

class TransportDescription
{
public:
    TransportDescription();
    void AddOption(const std::string& option){
        mTranportOptions.push_back(option);
    }

    static std::string ConnectionRoleToString(const ConnectionRole& role);

    void AddCandidate(const std::string& ip, const int port, const std::string& type);

    IceParameters ice() const;
    void SetIce(const IceParameters &ice);

    std::vector<std::string> tranportOptions() const;
    void SetTranportOptions(const std::vector<std::string> &tranportOptions);

    FingerprintParameters fingerprint() const;
    void SetFingerprint(const FingerprintParameters &fingerprint);

    ConnectionRole connectionRole() const;
    void SetConnectionRole(const ConnectionRole &connectionRole);

    std::vector<CandidateParam> candidates() const;
    void SetCandidates(const std::vector<CandidateParam> &candidates);

private:
    std::vector<std::string> mTranportOptions;
    IceParameters mIce;
    FingerprintParameters mFingerprint;
    ConnectionRole mConnectionRole;

    std::vector<CandidateParam> mCandidates;

};

#endif // TRANSPORTDESCRIPTION_H
