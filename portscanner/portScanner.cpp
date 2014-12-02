//
//  main.cpp
//  portscanner
//
//  Created by Tony Liu on 11/4/14.
//  Copyright (c) 2014 Tony Liu. All rights reserved.
//

#include <iostream>
#include <unistd.h>
#include "utils.h"
#include "CFileOperation.h"
#include "CMasterThread.h"

// control the verbose output
// defined in bt_client
bool VERBOSE;

int main(int argc, char * argv[]) {
    //parse arguments
    ps_args_t ps_args;
    parse_args(&ps_args, argc, argv);
    VERBOSE = ps_args.verbose;
    if ("\0" != ps_args.ipListFile){
        //parse ip list file if there is one
        void * buffer = new void*[FILE_MAX_SIZE];
        CFileOperation cfileoperation(ps_args.ipListFile);
        int ibufsize = cfileoperation.ReadFile(buffer, 0, FILE_MAX_SIZE);
        parse_ip_file(ps_args.ipList, buffer, ibufsize);
    }
    
    //print arguments
    printMSG("portScanner arguments:\n");
    printMSG("Init %d scanning thread.\n", ps_args.nthread);
    printMSG("\n--- IP address list ---\n");
    for (std::list<std::string>::iterator it = ps_args.ipList.begin(); it != ps_args.ipList.end(); it++)
        printMSG(*it);
    printMSG("\n--- Ports ---\n");
    for (std::list<int>::iterator it = ps_args.portList.begin(); it != ps_args.portList.end(); it++)
        printMSG("%d\n", *it);
    printMSG("\n--- Scan flags ---\n");
    for (std::list<std::string>::iterator it = ps_args.flagList.begin(); it != ps_args.flagList.end(); it++)
        printMSG(*it);
    
    try {
        //build a task queue from the args
        
        //create workers here
        CMasterThread * pcmasterthread = new CMasterThread(ps_args.nthread, "master_thread");
        while (true) {
            //each worker fetches a task from the task queue
            sleep(1);
            //main thread exit until the task queue is empty
            break;
        }
        //after everything is done, kill all workers here
        if (NULL != pcmasterthread){
            delete pcmasterthread;
            pcmasterthread = NULL;
        }
    } catch (std::string msg) {
        printMSG(msg);
    }
    catch (const char * msg) {
        printMSG(msg);
    }
    catch (char * msg) {
        printMSG(msg);
    } 

    return 0;
}


