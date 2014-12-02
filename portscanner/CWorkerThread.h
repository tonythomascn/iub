//
//  CWorkerThread.h
//  portscanner
//
//  Created by Tony Liu on 11/09/14.
//  Copyright (c) 2014 Tony Liu. All rights reserved.
//

#ifndef __portscanner__CWorkerThread__
#define __portscanner__CWorkerThread__

#include <stdio.h>
#include "CBaseThread.h"

class CMasterThread;
class CWorkerThread: public CBaseThread{
public:
    /*
     * @brief Master thread constructor
     * @param [in] the pointer to the master thread
     * @param [in] thread name
     */
    CWorkerThread(CMasterThread * pCMasterThread, const std::string ThreadName);
    ~CWorkerThread();
    
private:
    /*
     * @brief Virtual funciton inherenced from the base class
     * @return null
     */
    void OnBaseThreadProc();
    
private:
    //current thread name
    std::string m_strThreadName;
    //pointer to the master thread
    CMasterThread * m_pCMasterThread;
    //thread runtime flag
    bool m_bRunFlag;
};
#endif /* defined(__portscanner__CWorkerThread__) */
