#ifndef _UTILS_H_
#define _UTILS_H_

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

/**
   @args: 
      addr - string
      port - int
   @return
      resolved addr_port - struct sockaddr_in
*/
struct sockaddr_in setupAddr(std::string addr, int port) {
  struct sockaddr_in addr_port;
  addr_port.sin_addr.s_addr = inet_addr( addr.c_str() );
  addr_port.sin_family = AF_INET;
  addr_port.sin_port = htons( port );

  return addr_port;
} 






#endif












