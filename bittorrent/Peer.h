#ifndef _PEER_H_
#define _PEER_H_
#include <netinet/in.h>
#include "bt_lib.h"
#include <map>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
// global variables

//std::map <int, std::string> sock2peer; // map a sock to a description of a peer
//std::string getPeerDesc(char *ip, unsigned short port); // return a description of a peer
//std::string getPeerDesc(struct sockaddr_in); // return a description of a peer

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
  bool sendBitfield(int sock); // send msg of bitfield to sock
  std::map <int, bool> handshaked; // to mark if a sock has handshaked or not
  ~SeederManager();
private:
  bt_args_t *args; 
  bool createBitfield(char *buf, int &len); // create a msg for bitfield 
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
  bool sendRequest(int sock);
  bool processSock(int sock); // process a sok
  bool isDownloadComplete();
  std::map <int, bool> handshaked;
  ~LeecherManager();
private:
  bt_args_t *args;
  bool connectSeeder(struct sockaddr_in);
  bool sendRequest(int sock, int index, int begin, int length); // send request msg to sock
  bool createRequest(char *buf, int &len, int index, int begin, int length); // create request msg
  int downloaded[MAX_PIECES_NUM]; // 0 - not download, 1 - in process, 2 - downloaded
  int n_downloaded; // record how many piece has been downloaded
  
}; 


//wrapper for socket api

//send data in buf to seederSock with length n_bytes
bool sendData(int seederSock, char *buf, int n_bytes);  

// read a msg from sock to buf, return type of msg, if failed, return -1 
// if nothing to read, return 0 (no error)
// the size of buf should be no smaller than MAX_BUF_SZIE
 int readMSG(int sock, char *buf, int &len);


/* create handshake message */
bool createHandshakeMsg(char *buf, bt_info_t *info, char *id);

// pick a free piece index, return -1 if no free index
int pickNeedPiece(int *indexes, int len);

// create type-7 msg based on request, save in buf, set len, read data from fp.
bool createPieceMsg(FILE *fp, char *buf, int &len, int offset,  bt_request_t);

// save 'length' bytes from src to fp, return the actuall # of saved bytes
// if failed, return -1
int saveToFile(FILE *fp, char *src, int offset, int length);

#endif


















