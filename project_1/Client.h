#ifndef _CLINET_H_
#define _CLINET_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>
#include <unistd.h>
#include "common.h"
using namespace std;


/**
   Client class using TCP
*/
class Client {

public:
  // constructor
  Client();
  // connect to a server
  bool connectServer(string address, int port);
  bool connectServer(struct sockaddr_in);
  // send data to server
  bool sendData(string data);
  bool sendData(string fileName, int offset, int n_bytes);
  // receive data 
  string receiveData(int);
  ~Client();
private:
  // socket file description
  int sockfd;
  struct sockaddr_in server;

};



#endif










