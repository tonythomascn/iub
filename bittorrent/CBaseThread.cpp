//
//  CBaseThread.cpp
//  bittorrent
//
//  Created by Tony Liu on 9/28/14.
//  Copyright (c) 2014 Tony Liu. All rights reserved.
//

#include "CBaseThread.h"
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <iostream>
#include <unistd.h>
CBaseThread::CBaseThread(const std::string &ThreadName)
:m_hThread(0)
,m_FlagRunning(false)
,m_RunCounter(0)
{
    m_ThreadName = ThreadName;
    m_tid = 0;
}

CBaseThread::~CBaseThread()
{
    if(0 != m_hThread)
    {
        fprintf(stderr, "ERROR: thread --%s-- was not terminated %s,%s,%d\n", m_ThreadName.c_str(), __FILE__,__PRETTY_FUNCTION__,__LINE__);
    }
}

bool CBaseThread::RunThread(int Policy,int Priority)
{
    int retval;
    if(0 != m_hThread)
    {
        return false;
    }
    //initialize thread attribute, set policy and piority
    pthread_attr_t	*p_thread_attr = NULL;
    if(Policy != SCHED_OTHER)
    {
        p_thread_attr = new pthread_attr_t;
        pthread_attr_init(p_thread_attr);
        sched_param sp;
        pthread_attr_setschedpolicy(p_thread_attr,Policy);
        sp.sched_priority = Priority;
        pthread_attr_setschedparam(p_thread_attr,&sp);
    }
    retval = pthread_create(&m_hThread,p_thread_attr,ThreadRuntimeFunc,this);
    
    if(p_thread_attr != NULL)
    {
        pthread_attr_destroy(p_thread_attr);
        delete p_thread_attr;
    }
    
    if (retval != 0)
    {
        fprintf(stderr, "ERROR: thread --%s-- create failed %s,%s,%d\n", m_ThreadName.c_str(), __FILE__,__PRETTY_FUNCTION__,__LINE__);
        return false;
    }
    
    m_Policy = Policy;
    m_Priority = Priority;
    m_RunCounter ++;
    
    return true;
    
}

bool CBaseThread::SetThreadParam ( int Policy, int Priority )
{
    if(0 == m_hThread)
    {
        return false;
    }
    sched_param sp;
    sp.sched_priority = Priority;
    pthread_setschedparam(m_hThread,Policy,&sp);
    m_Policy = Policy;
    m_Priority = Priority;
    
    return true;
}

void CBaseThread::WaitThreadExit()
{
    if(0 != m_hThread)
    {
        void * retval=NULL;
        //close the thread
        pthread_join(m_hThread,&retval);
        m_hThread = 0;
    }
    else
    {
        //
    }
}

void * CBaseThread::ThreadRuntimeFunc(void *pParams)
{
    CBaseThread *This = (CBaseThread *)pParams;
    This->m_FlagRunning= true;
    //get the thread id
    This->m_tid = syscall(SYS_gettid);

    for(;;)
    {
        //subclass's thread function
            This->OnBaseThreadProc();
            break;
    }
    
    This->m_FlagRunning= false;
    This->m_tid = 0;
    return NULL;
}

//=====================================================================
CEvent::CEvent(const std::string Name,bool bInitSet,bool bWaitAutoReset)
{
    m_bSeted = bInitSet;
    m_bWaitAutoReset = bWaitAutoReset;
    m_Name = Name;
    
    pthread_cond_init(&m_hCond,NULL);
    pthread_mutex_init(&m_hMutex,NULL);

}


CEvent::~CEvent()
{
    pthread_cond_destroy(&m_hCond);
    pthread_mutex_destroy(&m_hMutex);
}

void CEvent::Set()
{
    pthread_mutex_trylock(&m_hMutex);
    m_bSeted = true;
    pthread_cond_signal(&m_hCond);
    m_Counter_Set ++;
    pthread_mutex_unlock(&m_hMutex);
    
}
void CEvent::Reset()
{
    pthread_mutex_trylock(&m_hMutex);
    m_bSeted = false;
    m_Counter_Reset ++;
    pthread_mutex_unlock(&m_hMutex);
}

bool CEvent::Wait(unsigned int Timeout_ms)
{
    bool retval = true;
    pthread_mutex_trylock(&m_hMutex);
    
    if(!m_bSeted)
    {
        if(Timeout_ms == 0)
        {
            pthread_cond_wait(&m_hCond,&m_hMutex);
            retval = true;
        }
        else
        {
            timeval now;
            timespec  timeout;
            gettimeofday(&now,NULL);
            long long ns_count = (long long )now.tv_sec * ZTHREAD_NS + now.tv_usec*1000;
            ns_count += ((long long)Timeout_ms) *1000000;
            
            timeout.tv_sec = ns_count/ZTHREAD_NS;
            timeout.tv_nsec =ns_count %ZTHREAD_NS;
            if(pthread_cond_timedwait(&m_hCond,&m_hMutex,&timeout) == ETIMEDOUT)
                retval = false;
            else
                retval = true;
        }
    }
    
    if(m_bWaitAutoReset)
        m_bSeted = false;
    
    m_Counter_Wait ++;
    
    pthread_mutex_unlock(&m_hMutex);
    return retval;
}


//=====================================================
CWorkThread::CWorkThread(const std::string &ThreadName)
:CBaseThread(ThreadName)
,m_bRunThread(false)
,m_Event(ThreadName)
{
    m_WakeCount = 0;
    
}
CWorkThread::~CWorkThread()
{
    
}
bool CWorkThread::RunThread ( int Policy, int Priority )
{
    m_bRunThread = true;
    return CBaseThread::RunThread(Policy,Priority);
}

void CWorkThread::WaitThreadExit()
{
    m_bRunThread = false;
    Wake();
    return CBaseThread::WaitThreadExit();
}


bool CWorkThread::Wake()
{
    m_Event.Set();
    return true;
}
void CWorkThread::OnBaseThreadProc()
{
    while(m_bRunThread)
    {
        m_IsWorking= false;
        m_WakeCount ++;
        if(m_Event.Wait())
        {
            if(!m_bRunThread)
                break;
            
            m_IsWorking = true;
            OnWorkThreadProc();
        }
    }
}



//======================================================================
CTimerThread::CTimerThread(const std::string &ThreadName,bool bAutoRun )
:CBaseThread(ThreadName)
{
    m_bRunThread = false;
    
    if(bAutoRun )
    {
        RunThread();
    }
}

CTimerThread::~CTimerThread()
{
    WaitThreadExit();
}


void CTimerThread::OnBaseThreadProc()
{
    IT_MAP_TIMER it;
    struct timeval tv;
    unsigned long long tm_ms;
    while(m_bRunThread)
    {
        gettimeofday(&tv,NULL);
        tm_ms = (unsigned long long)tv.tv_sec*1000 + tv.tv_usec/1000;
        pthread_mutex_trylock(&m_Mutex);
        for(it = m_mapTimer.begin();it != m_mapTimer.end();it ++)
        {
            if(tm_ms > (it->second.LastTimerMs + it->second.TimerPeriodMs))
            {
                it->second.LastTimerMs = tm_ms;
                it->second.ExecCounter ++;
                it->first->OnTimerProc();
            }
        }
        pthread_mutex_unlock(&m_Mutex);
        m_ExecCounter++;
        usleep(50*1000);
    }
}

void CTimerThread::AddTimer(CTimer *pTimer, unsigned int TimePeriodMs)
{
    struct timeval tv;
    ZTIMER_STRU_T tp;
    
    tp.ExecCounter = 0;
    tp.TimerPeriodMs = TimePeriodMs;
    gettimeofday(&tv,NULL);
    tp.LastTimerMs  = (unsigned long long)tv.tv_sec*1000 + tv.tv_usec/1000;
    
    pthread_mutex_trylock(&m_Mutex);
    m_mapTimer[pTimer] = tp;
    pthread_mutex_unlock(&m_Mutex);
}


void CTimerThread::DelTimer(CTimer *pTimer)
{
    IT_MAP_TIMER it;
    
    pthread_mutex_trylock(&m_Mutex);
    if((it = m_mapTimer.find(pTimer)) != m_mapTimer.end())
    {
        m_mapTimer.erase(it);
    }
    pthread_mutex_unlock(&m_Mutex);
    
}

bool CTimerThread::RunThread ( int Policy, int Priority )
{
    m_bRunThread = true;
    return CBaseThread::RunThread(Policy,Priority);
}

void CTimerThread::WaitThreadExit()
{
    m_bRunThread = false;
    return CBaseThread::WaitThreadExit();
}




