#ifndef _SERVER_H_
#define _SERVER_H_

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
#include "common.h"
using namespace std;

const int MAX_QUE = 1;

/**
   Server class using TCP
*/
class Server {

public:
  Server(string address, int port, string ofile);
  Server(struct sockaddr_in, string);
  // accept a client, return the sock
  int acceptClient();
  // process the accepted client
  bool processClient(int sock);


private:
  int serverSock, clientSock;
  struct sockaddr_in serverAddr, clientAddr;
  string outfile;

};



#endif










