//
//  CLog.h
//  bittorrent
//
//  Created by Tony Liu on 9/20/14.
//  Copyright (c) 2014 Tony Liu. All rights reserved.
//

#ifndef __bittorrent__CLog__
#define __bittorrent__CLog__

#include <iostream>
#include <stdarg.h>
enum LogLevel
{
    LOG_INFO = 1,
    LOG_NOTIFY = 2,
    LOG_WARNING = 3,
    LOG_ERROR = 4
};
//control which level and beyond to be logged
#define LOG_LEVEL LOG_NOTIFY

class CFileOperation;
class CLog
{
public:
    /*
     * @brief Constructor, do nothing.
     * @return null
     */
    CLog();
    /*
     * @brief Constructor. Two ways to init: CLog() and Init(); or CLog(strLogName).
     * @param [in] log name
     * @return null
     */
    CLog(std::string);
    /*
     * @brief destructor, destroy everything.
     */
    ~CLog();
    /**
     * @brief Initialize log file and set log level
     * @param [in] log file name
     * @param [in] log level
     * @return null
     */
    void Init(std::string, LogLevel);
    /**
     * @brief Set which level and beyond the log needed to be shown on screen
     * @param [in] loglevel
     * @return null
     */
    void SetLogLevel(LogLevel);
    
    /**
     * @brief Print message on the screen and write message to log
     * @param [in] message
     * @param [in] message's log level
     * @return null
     */
    void Log(std::string, LogLevel);
private:
    /**
     * @brief Write message to log
     * @param [in] message
     * @return true/false
     */
    bool Log(std::string);
    /**
     * @brief Get current local time
     * @return current local time string
     */
    std::string GetCurTimeStr();
    /**
     * @brief Get relative time
     * @return relative time string
     */
    std::string GetRelativeTimeStr();
private:
    CFileOperation * m_pCFileOperation;
    std::string m_strLogName;
    int m_iLogLevel;
    struct timeval m_timevalStart;
};

static CLog CLOG;
void Init(std::string strLogName, LogLevel emLogLevel);
///Example: LOG(LOG_WARNING, "hello again %d", hello);
void LOG(LogLevel level, const char*args, ...);

#endif /* defined(__bittorrent__CLog__) */
