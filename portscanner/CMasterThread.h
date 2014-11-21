//
//  CMasterThread.h
//  portscanner
//
//  Created by Tony Liu on 11/10/14.
//  Copyright (c) 2014 Tony Liu. All rights reserved.
//

#ifndef __portscanner__CMasterThread__
#define __portscanner__CMasterThread__

#include <stdio.h>
#include <pthread.h>
#include "CBaseThread.h"

class CWorkerThread;
class CMasterThread: public CBaseThread{
public:
    /*
     * @brief Master thread constructor
     * @param [in] how many workers needed to be created
     * @param [in] thread name
     */
    CMasterThread(const int nworker, const std::string &ThreadName = "");
    /*
     * @brief Master thread destructor
     */
    ~CMasterThread();
    
    /*
     * @brief Get task from the task queue
     * @param [in]
     * @return 1: successful, 0: empty queue, -1: unsuccessful
     */
    int GetTaskFromQueue();
private:
    /*
     * @brief Virtual funciton inherenced from the base class
     * @return null
     */
    void OnBaseThreadProc();
    /*
     * @brief Initialize the mutex
     * @return true/false
     */
    bool InitMutex();
    /*
     * @brief Initialize worker threads
     * @param [in] how many workers need to be created
     * @return true/false
     */
    bool InitWorkerThreads(int nthread);
    /*
     * @brief Destroy worker threads
     * @param [in] how many workers need to be destroyed
     * @return true/false
     */
    bool DestroyWorkerThreads(int nthread);
private:
    //worker thread pool
    CWorkerThread ** m_pCWorkerThread;
    //how many workers need to be created
    int m_iNWorker;
    //current thread name
    std::string m_strThreadName;
    //mutex to protect the task queue
    pthread_mutex_t m_Mutex;
    //runtime flag
    bool m_bRunFlag;
};
#endif /* defined(__portscanner__CMasterThread__) */
