//
//  utils.h
//  portscanner
//
//  Created by Tony Liu on 11/2/14.
//  Copyright (c) 2014 Tony Liu. All rights reserved.
//

#ifndef portscanner_utils_h
#define portscanner_utils_h
#include <list>
#include <string>

/*Maximum file name size*/
#define FILE_NAME_MAX 1024
//maximum file size
#define FILE_MAX_SIZE 10240
//default maximum scan port
#define MAX_SCAN_PORT 1024
// control the verbose output
// defined in bt_client
extern bool VERBOSE;

typedef struct _ps_args_t{
    //ip list
    std::list<std::string> ipList;
    //ip prefix
    std::string subnetIp;
    int prefixMask;
    //ip list file
    std::string ipListFile;
    //port list
    std::list<int> portList;
    //thread number, default is one thread
    int nthread;
    //scan flags list
    std::list<std::string> flagList;
    //output verbose flag
    int verbose;
    _ps_args_t():
    nthread(1),
    verbose(0){
        ipListFile = "\0";
    }
    ~_ps_args_t(){
        nthread = 0;
        verbose = 0;
        ipListFile = "\0";
    }
}ps_args_t;
//command line usgage
void usage(FILE * file);
//parse arguments
void parse_args(ps_args_t * ps_args, int argc,  char * argv[]);
//parse the content of the ip list file
void parse_ip_file(std::list<std::string> &ipList, void * buffer, int buffersize);
// print msg to std::cout when VERBOSE is true
// for error, should use perror etc.
bool printMSG(std::string msg);
// take the same args as printf
// print to standard output when VERBOSE is true
bool printMSG(const char *fmt, ...);
//parse ports argument
void parse_ports(std::list<int> &portList, std::string str);
//parse range ports
void parse_range_ports(std::list<int> &portList, std::string str);
//parse prefix argument
void parse_prefix(std::list<std::string> &ipList, std::string str);
void parse_prefix(std::string &subnetIp, int prefixMask, std::string str);

#endif
