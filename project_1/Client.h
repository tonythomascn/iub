#ifndef _CLINET_H_
#define _CLINET_H_

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
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
  // send data to server
  bool sendData(string data);
  // receive data 
  string receiveData(int);
  ~Client();
private:
  // socket file description
  int sockfd;
  struct sockaddr_in server;

};



#endif










