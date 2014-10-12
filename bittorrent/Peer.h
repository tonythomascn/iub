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
  SeederManager(bt_args_t *btArg);   //bind to ip:port, and link to the parsed bt_args
  int acceptLeecher();    // try to accept a new leecher, return leecher sockfd 
  bool sendHandshake(int leecherSock); //send handshake Msg to the dest leecherSock
  bool recvHandshake(int leecherSock); //recv handshake Msg from leecherSock
  bool processSock(int sock); // process a given sock, read data and take actions
  //  int n_sockets; // # of sockets
  //int sockets[MAX_CONNECTIONS]; //Array of possible sockets
  //struct pollfd poll_sockets[MAX_CONNECTIONS]; //Array of pollfd for polling for input
  int sockid;
private:
  bt_args_t *args;
};



// manager of a leecher
class LeecherManager {
public:
  // use bt_args to initialize the class
  LeecherManager(bt_args_t *);
  // try to connect to a seeder
  bool connectSeeders();
  bool sendHandshake(int sockfd);
  bool recvHandshake(int sockfd);
  int n_sockets; // # of sockets
  int sockets[MAX_CONNECTIONS]; //Array of possible sockets
  //struct pollfd poll_sockets[MAX_CONNECTIONS]; //Array of pollfd for polling for input
private:
  bt_args_t *args;
  bool connectSeeder(struct sockaddr_in);

}; 


//wrapper for socket api

//send data in buf to seederSock with length n_bytes
bool sendData(int seederSock, char *buf, int n_bytes);  


/* create handshake message */
bool createHandshakeMsg(char *buf, bt_info_t *info, char *id);





#endif













