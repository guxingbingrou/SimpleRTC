#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <vector>
#include <stdio.h>
#include <memory>
#include <mutex>


enum LoggerLevel{
    kTrace = 0,
    kDebug,
    kInfo,
    kWarning,
    kError
};

#define TRACE(fmt, ...) (Logger::GetInstance()->DoLog(kTrace, fmt, __VA_ARGS__))

#define DEBUG(fmt, ...) (Logger::GetInstance()->DoLog(kDebug, fmt, __VA_ARGS__))

#define INFO(fmt, ...) (Logger::GetInstance()->DoLog(kInfo, fmt, __VA_ARGS__))

#define WARNING(fmt, ...) (Logger::GetInstance()->DoLog(kWarning, fmt, __VA_ARGS__))

#define ERROR(fmt, ...) (Logger::GetInstance()->DoLog(kError, fmt, ##__VA_ARGS__))

class Logger
{
public:
    static std::shared_ptr<Logger> GetInstance();

    Logger();
    ~Logger();

    void InitLogger(const std::string& path = "simpleLog.log");

    void SetLogLevel(LoggerLevel level);

    void DoLog(LoggerLevel level, const char* fmt, ...);


private:
    LoggerLevel mLevel = kWarning;
    std::vector<const char*> mLevelString;

    FILE* mLogFile = nullptr;

    static std::mutex sMutex;
    static std::shared_ptr<Logger> sLogger;

};

#endif // LOGGER_H
