#include "TransportDescription.h"

TransportDescription::TransportDescription()
{

}

std::string TransportDescription::ConnectionRoleToString(const ConnectionRole &role)
{
    switch (role) {
        case CONNECTIONROLE_ACTIVE:
            return "active";
        case CONNECTIONROLE_PASSIVE:
            return "passive";
        case CONNECTIONROLE_ACTPASS:
            return "actpass";
        case CONNECTIONROLE_HOLDCONN:
            return "holdconn";
        case CONNECTIONROLE_NONE:
            return "";
    }
}

void TransportDescription::AddCandidate(const std::string &ip, const int port, const std::string &type)
{
    mCandidates.emplace_back(port, ip, type);
}

IceParameters TransportDescription::ice() const
{
    return mIce;
}

void TransportDescription::SetIce(const IceParameters &ice)
{
    mIce = ice;
}

std::vector<std::string> TransportDescription::tranportOptions() const
{
    return mTranportOptions;
}

void TransportDescription::SetTranportOptions(const std::vector<std::string> &tranportOptions)
{
    mTranportOptions = tranportOptions;
}

FingerprintParameters TransportDescription::fingerprint() const
{
    return mFingerprint;
}

void TransportDescription::SetFingerprint(const FingerprintParameters &fingerprint)
{
    mFingerprint = fingerprint;
}

ConnectionRole TransportDescription::connectionRole() const
{
    return mConnectionRole;
}

void TransportDescription::SetConnectionRole(const ConnectionRole &connectionRole)
{
    mConnectionRole = connectionRole;
}

std::vector<CandidateParam> TransportDescription::candidates() const
{
    return mCandidates;
}

void TransportDescription::SetCandidates(const std::vector<CandidateParam> &candidates)
{
    mCandidates = candidates;
}
