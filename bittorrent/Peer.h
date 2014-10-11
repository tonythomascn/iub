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
  bool handshake(int leecherSock);
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
  bool handshake();
private:
  int sockfd;
  bt_args_t *args;
  bool connectSeeder(struct sockaddr_in);
  //  bool sendData(int seederSock, char *buf, int n_bytes);
  //  bool recvData(int seederSock, char *buf, int &n_bytes);
}; 


//wrapper for socket api

//send data in buf to seederSock with length n_bytes
bool sendData(int seederSock, char *buf, int n_bytes);  
// //recieve data into buf, with length at most n_bytes
// bool recvData(int leecherSock, char *buf, int &n_bytes);

/* create handshake message */
bool createHandshakeMsg(char *buf, bt_info_t *info, char *id);





#endif













