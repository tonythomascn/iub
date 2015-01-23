//
//  CMasterThread.cpp
//  portscanner
//
//  Created by Tony Liu on 11/10/14.
//  Copyright (c) 2014 Tony Liu. All rights reserved.
//
#include <sstream>
#include "CMasterThread.h"
#include "CWorkerThread.h"

CMasterThread::CMasterThread(const int nworker, const std::string &ThreadName)
:CBaseThread(ThreadName),
m_strThreadName(ThreadName),
m_iNWorker(nworker),
m_bRunFlag(true){
    if (!InitMutex())
        throw "Mutex initialize failed!";
    
    this->InitWorkerThreads(m_iNWorker);
    this->RunThread();
}

CMasterThread::~CMasterThread(){
    m_bRunFlag = false;
    this->WaitThreadExit();
    this->DestroyWorkerThreads(m_iNWorker);
    if (0 != pthread_mutex_destroy(&m_Mutex)){
        fprintf(stderr, "ERROR: mutex destroy %s,%s,%d\n", __FILE__,__PRETTY_FUNCTION__,__LINE__);
        perror("mutex_destroy");
    }
}

void CMasterThread::OnBaseThreadProc(){
    while (m_bRunFlag) {
        printf("%s\n", m_strThreadName.c_str());
        break;
    }
}

bool CMasterThread::InitMutex()
{
    if (0 == pthread_mutex_init(&m_Mutex,NULL)){
        //fprintf(stdout, "mutex init %s,%s,%d\n", __FILE__,__PRETTY_FUNCTION__,__LINE__);
        return true;
    }
    else{
        fprintf(stderr, "ERROR: mutex init failed %s,%s,%d\n", __FILE__,__PRETTY_FUNCTION__,__LINE__);
        perror("mutex_init");
    }
    return false;
}

bool CMasterThread::InitWorkerThreads(int nthread){
    m_pCWorkerThread = new CWorkerThread*[nthread];
    std::string strthreadname = "worker_thread_";
    std::stringstream sstm;
    for (int i = 1; i <= nthread; i++){
        sstm << strthreadname << i;
        m_pCWorkerThread[i] = new CWorkerThread(this, sstm.str());
        sstm.str("");
    }
    return true;
}

bool CMasterThread::DestroyWorkerThreads(int nthread){
    for (int i = 1; i <= nthread; i++){
        if (NULL != m_pCWorkerThread[i]){
            delete m_pCWorkerThread[i];
            m_pCWorkerThread[i] = NULL;
        }
    }
    if (NULL != m_pCWorkerThread){
        delete m_pCWorkerThread;
        m_pCWorkerThread = NULL;
    }
    return true;
}

int CMasterThread::GetTaskFromQueue(){
    if (0 == pthread_mutex_trylock(&m_Mutex))
    {
        //TODO get task from queue
        
        pthread_mutex_unlock(&m_Mutex);
    }
    else
    {
        fprintf(stderr, "ERROR: lock %s,%s,%d\n", __FILE__,__PRETTY_FUNCTION__,__LINE__);
        perror("mutex_trylock");
    }
    return 0;
}