//
//  CBaseThread.h
//  bittorrent
//
//  Created by Tony Liu on 9/28/14.
//  Copyright (c) 2014 Tony Liu. All rights reserved.
//

#ifndef __bittorrent__CBaseThread__
#define __bittorrent__CBaseThread__

#include <pthread.h>
#include <string>
#include <map>
#define ZTHREAD_NS 1000000000LL


class CBaseThread
{
public:
    CBaseThread ( const std::string &ThreadName );
    virtual ~CBaseThread();
    
    /**
     * start thread
     * @param scheduling policy for a thread can either be SCHED_FIFO (first in, first out) or SCHED_RR (round-robin).
     * @param thread priority (accessed via param->sched_priority) must be at least PTHREAD_MIN_PRIORITY (0) and no more than PTHREAD_MAX_PRIORITY (31).
     * @return true/false
     */
    virtual bool RunThread ( int Policy = SCHED_OTHER, int Priority = 0 );
    
    /**
     * set thread attribuate, modify thread priority
     * @param Policy
     * @param Priority 
     * @return true/false
     */
    virtual bool SetThreadParam ( int Policy, int Priority );
    /**
     * wait thread to exit
     */
    virtual void WaitThreadExit();
private:
    /**
     * pure virtual function, inhereted by the subclass, contain the real thread
     */
    virtual void OnBaseThreadProc() = 0;
    static void * ThreadRuntimeFunc ( void *pParams );
    
    std::string m_ThreadName;
    pthread_t m_hThread;
    unsigned int m_tid;
    bool m_FlagRunning;///< thread running flag
    unsigned int m_RunCounter;///< thread running counter
    int m_Policy;
    int m_Priority;
};

class CEvent
{
public:
    CEvent(const std::string Name,bool bInitSet = false,bool bWaitAutoReset = true);
    ~CEvent();
    
    void Set();
    void Reset();
    bool Wait(unsigned int Timeout_ms = 0);
    
private:
    
    pthread_mutex_t m_hMutex;
    pthread_cond_t m_hCond;
    bool m_bSeted;
    bool m_bWaitAutoReset;
    int m_Counter_Set;
    int m_Counter_Reset;
    int m_Counter_Wait;
    std::string m_Name;
    
};

/**
 * Work thread, sleep after every loop, wake until Wake() was called
 */
class CWorkThread
: public CBaseThread
{
public:
    CWorkThread(const std::string &ThreadName);
    ~CWorkThread();
    
    /**
     * @brief wake the work thread
     * @return true/false
     */
    bool Wake();
    /**
     * @brief see CBaseThread::RunThread()
     * @param Policy
     * @param Priority
     * @return true/false
     */
    virtual bool RunThread ( int Policy = SCHED_OTHER, int Priority = 0 );
    /**
     * brief see CBaseThread::WaitThreadExit()
     */
    virtual void WaitThreadExit();
    
protected:
    /**
     * pure virtal function, realize by subclass completely
     */
    virtual void OnWorkThreadProc() = 0;
    /**
     *
     */
    virtual void OnBaseThreadProc();
    
private:
    bool m_bRunThread;
    CEvent  m_Event;
    
    unsigned int m_WakeCount;
    bool m_IsWorking;
};


/**
 * 定时线程
 */
class CTimer
{
    friend class CTimerThread;
public:
    CTimer(){};
    virtual ~CTimer(){};
    
protected:
    virtual void OnTimerProc() = 0;
};

typedef struct _ZTIMER_STRU_T
{
    unsigned int TimerPeriodMs;
    unsigned long long LastTimerMs;
    unsigned int ExecCounter;
}ZTIMER_STRU_T;

typedef std::map<CTimer*, ZTIMER_STRU_T> MAP_TIMER;
typedef MAP_TIMER::iterator IT_MAP_TIMER;

/**
 * Timer thread, process timely, could process several ctimer at almost same time
 */
class CTimerThread
: public CBaseThread
{
public:
    CTimerThread(const std::string &ThreadName,bool bAutoRun = false);
    virtual ~CTimerThread();
    /**
     * Add a CTimer object
     * If already has one, update it
     * @param pTimer
     * @param TimePeriodMs
     */
    void AddTimer(CTimer *pTimer,unsigned int TimePeriodMs);
    /**
     * Delete a CTimer pointer
     * If already has one, update it
     * @param pTimer
     */
    void DelTimer(CTimer *pTimer);
    /**
     * @brief see CBaseThread::RunThread()
     * @param Policy
     * @param Priority
     * @return true/false
     */
    virtual bool RunThread ( int Policy = SCHED_OTHER, int Priority = 0 );
    /**
     * @brief see CBaseThread::WaitThreadExit()
     */
    virtual void WaitThreadExit();
protected:
    virtual void OnBaseThreadProc();
private:

    pthread_mutex_t m_Mutex;
    MAP_TIMER m_mapTimer;
    unsigned int m_ExecCounter;
    bool m_bRunThread;
};



#endif /* defined(__bittorrent__CBaseThread__) */
