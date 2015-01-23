//
//  CLog.cpp
//  bittorrent
//
//  Created by Tony Liu on 9/20/14.
//  Copyright (c) 2014 Tony Liu. All rights reserved.
//
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <sstream>
#include <cstdio>
#include "CLog.h"
#include "CFileOperation.h"

CLog::CLog():
m_pCFileOperation(NULL),
m_strLogName(""),
m_iLogLevel(LOG_LEVEL)
{
    //fprintf(stderr, "%s,%s,%d\n", __FILE__,__PRETTY_FUNCTION__,__LINE__);
    //m_pCFileOperation = new CFileOperation("./.log/", GetCurTimeStr() + ".log");
    gettimeofday(&m_timevalStart, NULL);
}

CLog::CLog(std::string strLogName):
m_iLogLevel(LOG_LEVEL)
{
    fprintf(stderr, "%s,%s,%d\n", __FILE__,__PRETTY_FUNCTION__,__LINE__);
    m_strLogName = strLogName;
    m_pCFileOperation = new CFileOperation(strLogName);
    if (NULL == m_pCFileOperation)
        throw "log init failed!";
    gettimeofday(&m_timevalStart, NULL);
}

CLog::~CLog()
{
    if (NULL != m_pCFileOperation)
    {
        delete m_pCFileOperation;
        m_pCFileOperation = NULL;
    }
}

void CLog::Init(std::string strLogName, LogLevel loglevel)
{
    m_strLogName = strLogName;
    m_iLogLevel = loglevel;
   m_pCFileOperation = new CFileOperation(strLogName);
    if (NULL == m_pCFileOperation)
        throw "log init failed!";
}
void CLog::SetLogLevel(LogLevel loglevel)
{
    m_iLogLevel = loglevel;
}

std::string CLog::GetCurTimeStr()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    time_t currenttime = tv.tv_sec;
    struct tm curlocaltime;
    localtime_r(&currenttime, &curlocaltime);
    
    char datetime[30] = "\0";
    strftime(datetime, 30, "%Y-%m-%dT%H:%M:%S", &curlocaltime);
    
    return datetime;
}

std::string CLog::GetRelativeTimeStr()
{
    struct timeval current;
    gettimeofday(&current, NULL);
    char time[30] = "\0";
    snprintf(time, 30, "%.2f", (double)((current.tv_sec - m_timevalStart.tv_sec) * 1000000 + current.tv_usec - m_timevalStart.tv_usec) / 1000000);
    
    return time;
}

bool CLog::Log(std::string strMessage)
{
    bool breturn = true;
    if (!strMessage.empty())
    {
        //add time in the log
        std::string strtmpmessage = "[" + GetRelativeTimeStr() + "]\t" + strMessage.c_str() + "\n";
        void * message = (void*)strtmpmessage.c_str();
        if (NULL == m_pCFileOperation)
        {
            m_pCFileOperation = new CFileOperation(m_strLogName);
            if (NULL == m_pCFileOperation)
                throw "log init failed!";
        }
        if (!m_pCFileOperation->WriteFile(message, static_cast<unsigned int>(strtmpmessage.length())))
        {
            fprintf(stderr, "ERROR: log write failed %s,%s,%d\n", __FILE__,__PRETTY_FUNCTION__,__LINE__);
            perror(strMessage.c_str());
            breturn = false;
        }
    }
    else
    {
        fprintf(stderr, "ERROR: message is empty %s,%s,%d\n", __FILE__,__PRETTY_FUNCTION__,__LINE__);
        perror(strMessage.c_str());
        breturn = false;
    }
    return  breturn;
}


void CLog::Log(std::string strMessage, LogLevel emLogLevel)
{
    if (!strMessage.empty())
    {
        std::string strcurtime = GetRelativeTimeStr();
//        std::string strcolor = "";
//        char msg[10240] = "\0";
//        switch (emLogLevel)
//        {
//                //every level has different color on the screen
//            case LOG_INFO:
//                fprintf(stdout, "\33[0m");
//                //fprintf(stdout, "%s LOG_INFO: [%s]\t%s,%s,%d\33[0m\n", strcurtime.c_str(), strMessage.c_str(), __FILE__,__PRETTY_FUNCTION__,__LINE__);
//                strcolor = "\33[0m";
//                break;
//            case LOG_NOTIFY:
//                fprintf(stdout, "\33[1m\33[34m");
//                //fprintf(stdout, "%s LOG_NOTIFY: [%s]\t%s,%s,%d\33[0m\n", strcurtime.c_str(), strMessage.c_str(), __FILE__,__PRETTY_FUNCTION__,__LINE__);
//                strcolor = "\33[1m\33[34m";
//                break;
//            case LOG_WARNING:
//                fprintf(stdout, "\33[1m\33[33m");
//                //fprintf(stdout, "%s LOG_WARNING: [%s]\t%s,%s,%d\33[0m\n", strcurtime.c_str(), strMessage.c_str(), __FILE__,__PRETTY_FUNCTION__,__LINE__);
//                strcolor = "\33[1m\33[33m";
//                break;
//            case LOG_ERROR:
//                fprintf(stdout, "\33[1m\33[31m");
//                //fprintf(stdout, "%s LOG_ERROR: [%s]\t%s,%s,%d\33[0m\n", strcurtime.c_str(), strMessage.c_str(), __FILE__,__PRETTY_FUNCTION__,__LINE__);
//                strcolor = "\33[1m\33[31m";
//                break;
//            default:
//                fprintf(stdout, "\33[1m\33[35wm");
//                strcolor = "\33[1m\33[35wm";
//                fprintf(stdout, "\33[0m");
//                break;
//        }
//        //output msg no tiemstamp
//        strcolor += "%s\33[0m\n";
//        snprintf (msg, 10240, strcolor.c_str(), strMessage.c_str());
//        //fprintf (stdout, "%s", msg);
        if (emLogLevel >= m_iLogLevel)
        {
            if (LOG_WARNING <= emLogLevel)
            {
                std::stringstream ss;
                ss << emLogLevel;
                Log(ss.str() + " " + strMessage);
            }
            else
                Log(strMessage);
        }
    }
    else
    {
        fprintf(stderr, "ERROR: CLog message is empty %s,%s,%d\n", __FILE__,__PRETTY_FUNCTION__,__LINE__);
        perror(strMessage.c_str());
    }
}
void Init(std::string strLogName, LogLevel emLogLevel)
{
    CLOG.Init(strLogName, emLogLevel);
}
///Example: LOG(LOG_WARNING, "hello again %d", hello);
void LOG(LogLevel level, const char*args, ...)
{
    char msg[10240] = "\0";
    va_list ap;
    va_start(ap, args);
    vsnprintf(msg, 10240, args, ap);
    va_end(ap);
    CLOG.Log(msg, level);
}
