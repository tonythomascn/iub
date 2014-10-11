#ifndef _PEER_H_
#define _PEER_H_
#include <netinet/in.h>
#include "bt_lib.h"


// // class for a seeder
class Seeder {

};


// class for a leecher
class Leecher {
};

// manager of a seeder
class SeederManager {
public:
  //bind to ip:port, and link to the parsed bt_args
  SeederManager(bt_args_t *btArg);
  // try to accept a new leecher, return leecher sockfd 
  int acceptLeecher();
  //handshake with the dest leecherSock
  int handshake(int leecherSock);
  bool sendData(int sockid);
  //recieve data into buf, with n_bytes
  bool recvData(int leecherSock, char *buf, int &n_bytes);
private:
  int sockid;
  bt_args_t *args;
};



// manager of a leecher
class LeecherManager {
public:
  // use bt_args to initialize the class
  LeecherManager(bt_args_t *);
  // try to connect to a seeder
  bool connectSeeder();
  bool sendData(int seederSock, char *buf, int n_bytes);
  bool recvData(int seederSock, char *buf, int &n_bytes);
private:
  int sockfd;
  bt_args_t *args;
  bool connectSeeder(struct sockaddr_in);
}; 


#endif













