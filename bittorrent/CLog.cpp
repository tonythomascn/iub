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
#include "CLog.h"
#include "CFileOperation.h"

CLog::CLog():
m_pCFileOperation(NULL),
m_strLogName(""),
m_iLogLevel(LOG_LEVEL)
{
    //m_pCFileOperation = new CFileOperation("./.log/", GetCurTimeStr() + ".log");
}

CLog::CLog(std::string strLogName):
m_iLogLevel(LOG_LEVEL)
{
    m_strLogName = strLogName;
    m_pCFileOperation = new CFileOperation(strLogName);
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
   m_pCFileOperation = new CFileOperation(strLogName);
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

bool CLog::Log(std::string strMessage)
{
    bool breturn = true;
    if (!strMessage.empty())
    {
        //add time in the log
        std::string strtmpmessage = GetCurTimeStr() + "\n" + strMessage.c_str() + "\n";
        void * message = (void*)strtmpmessage.c_str();
        if (!m_pCFileOperation->WriteFile(message, static_cast<unsigned int>(strtmpmessage.length())))
        {
            fprintf(stderr, "ERROR: log write failed %s,%s,%d\n", __FILE__,__PRETTY_FUNCTION__,__LINE__);
            perror(strMessage.c_str());
            breturn = false;
        }
    }
    else
    {
        fprintf(stderr, "ERROR: message is NULL %s,%s,%d\n", __FILE__,__PRETTY_FUNCTION__,__LINE__);
        perror(strMessage.c_str());
        breturn = false;
    }
    return  breturn;
}


void CLog::Log(std::string strMessage, LogLevel emLogLevel)
{
    if (!strMessage.empty())
    {
        std::string strcurtime = GetCurTimeStr();
        switch (emLogLevel)
        {
                //every level has different color on the screen
            case LOG_INFO:
                fprintf(stdout, "\33[0m");
                fprintf(stdout, "%s LOG_INFO: %s\t%s,%s,%d\33[0m\n", strcurtime.c_str(), strMessage.c_str(), __FILE__,__PRETTY_FUNCTION__,__LINE__);
                break;
            case LOG_NOTIFY:
                fprintf(stdout, "\33[1m\33[34m");
                fprintf(stdout, "%s LOG_NOTIFY: %s\t%s,%s,%d\33[0m\n", strcurtime.c_str(), strMessage.c_str(), __FILE__,__PRETTY_FUNCTION__,__LINE__);
                break;
            case LOG_WARNING:
                fprintf(stdout, "\33[1m\33[33m");
                fprintf(stdout, "%s LOG_WARNING: %s\t%s,%s,%d\33[0m\n", strcurtime.c_str(), strMessage.c_str(), __FILE__,__PRETTY_FUNCTION__,__LINE__);
                break;
            case LOG_ERROR:
                fprintf(stdout, "\33[1m\33[31m");
                fprintf(stdout, "%s LOG_ERROR: %s\t%s,%s,%d\33[0m\n", strcurtime.c_str(), strMessage.c_str(), __FILE__,__PRETTY_FUNCTION__,__LINE__);
                break;
            default:
                fprintf(stdout, "\33[1m\33[35wm");
                fprintf(stdout, "\33[0m");
                break;
        }
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
        fprintf(stderr, "ERROR: CLog message is NULL %s,%s,%d\n", __FILE__,__PRETTY_FUNCTION__,__LINE__);
        perror(strMessage.c_str());
    }
}
