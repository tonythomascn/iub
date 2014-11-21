//
//  CWorkerThread.cpp
//  portscanner
//
//  Created by Tony Liu on 11/09/14.
//  Copyright (c) 2014 Tony Liu. All rights reserved.
//

#include "CWorkerThread.h"
#include "CMasterThread.h"

CWorkerThread::CWorkerThread(CMasterThread * pCMasterThread, const std::string &ThreadName)
:CBaseThread(ThreadName),
m_strThreadName(ThreadName),
m_pCMasterThread(pCMasterThread),
m_bRunFlag(true){
    
    this->RunThread();
}
CWorkerThread::~CWorkerThread(){
    m_bRunFlag = false;
    this->WaitThreadExit();
}
void CWorkerThread::OnBaseThreadProc(){
    int iret = 0;
    printf("%s\n", m_strThreadName.c_str());
    while (m_bRunFlag) {
        //fetch task from master thread
        iret = m_pCMasterThread->GetTaskFromQueue();
        if (iret) {
            //TODO deal task
            
        } else if (0 == iret){
            //all task is done, quit
            m_bRunFlag = false;
        } else if (-1 == iret) {
            //error handling
            m_bRunFlag = false;
        }
    }
}