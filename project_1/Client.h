#ifndef _CLINET_H_
#define _CLINET_H_

#include <iostream>
#include <netinet/in.h>

/**
   Client class using TCP
*/
class Client {

public:
  // constructor
  Client();
  // connect to a server
  bool connectServer(std::string address, int port);
  bool connectServer(struct sockaddr_in);
  // send data to server
  bool sendData(std::string data);
  bool sendData(std::string fileName, int offset, int n_bytes);
  // receive data 
    std::string receiveData(int);
  ~Client();
private:
  // socket file description
  int sockfd;
  struct sockaddr_in server;

};



#endif










