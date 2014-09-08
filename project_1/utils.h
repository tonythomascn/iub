#ifndef _UTILS_H_
#define _UTILS_H_

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>
using namespace std;


struct sockaddr_in setupAddr(string addr, int port);




#endif












