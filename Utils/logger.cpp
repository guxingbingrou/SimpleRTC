#include "logger.h"
#include <QtDebug>
#include <stdexcept>
#include <time.h>

std::mutex Logger::sMutex;
std::shared_ptr<Logger> Logger::sLogger;


std::shared_ptr<Logger> Logger::GetInstance()
{
    if(!sLogger){
        std::unique_lock<std::mutex> lck(sMutex);
        if(!sLogger){
            sLogger = std::make_shared<Logger>();
        }
    }

    return sLogger;
}

Logger::Logger()
{
    mLevelString.push_back("trace");
    mLevelString.push_back("debug");
    mLevelString.push_back("info");
    mLevelString.push_back("warning");
    mLevelString.push_back("error");

}

Logger::~Logger()
{
    if(mLogFile){
        fclose(mLogFile);
    }
}

void Logger::SetLogLevel(LoggerLevel level)
{
    mLevel = level;
}

void Logger::InitLogger(const std::string &path)
{
    mLogFile = fopen(path.c_str(), "wb");

    if(!mLogFile){
        qDebug() << "failed to open log file: " << QString::fromStdString(path);
        throw std::runtime_error("failed to open log file");
    }

}

void Logger::DoLog(LoggerLevel level, const char* fmt, ...)
{
    if(level < mLevel)
        return;

    char buf[4096];
    memset(buf, 0, 4096);

    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, 4095, fmt, args);
    va_end(args);


    char log_buf[4196];
    memset(log_buf, 0, 4196);

    struct tm* ntm = nullptr;
    time_t now = time(nullptr);
    ntm = localtime(&now);

    size_t len = sprintf(log_buf, "(%02d:%02d:%02d) [%s]: %s\n", ntm->tm_hour, ntm->tm_min, ntm->tm_sec, mLevelString[level], buf);

    if(level == kError){
        qDebug() << log_buf;
    }

    fwrite(log_buf, len, 1, mLogFile);
    fflush(mLogFile);

}
