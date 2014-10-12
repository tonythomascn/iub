#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Peer.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "bt_lib.h"
#include <openssl/sha.h>
#include <sstream>
// TODO:
//   + in seeder, map a sock to a peer




SeederManager::SeederManager(bt_args_t *btArg) {
  // set default values
  // n_sockets = 0;



  // set temp peer to represent this seeder
  peer_t thisPeer;
  // assign 0 as its port to automatically assign a port later
  init_peer(&thisPeer, btArg->id, btArg->ip, 0);
  sockaddr_in seederAddr = thisPeer.sockaddr;
  socklen_t socklen = sizeof(seederAddr);
  

  // create a reliable stream socket using TCP
  if ((sockid = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    std::cerr << "Failed to open the socket!\n";
    exit(1);
  }
  printMSG("Opening the socket ... OK!\n");
  // // add this sockid into sockets[0]
  // sockets[n_sockets++] = sockid;

  // // add this sockid into poll_sockets[]
  // poll_sockets[0].fd = sockid;
  // poll_sockets[0].events = POLLIN;



  // bind the sock
  int status = bind(sockid, (struct sockaddr *)&seederAddr, sizeof(struct sockaddr_in));
  if (status < 0) {
    std::cerr << "Failed to bind the port, check you input!\n";
    if (-1 != sockid)
      close(sockid);
    exit(1);
  }
  else {
    // make as non-block socket
    if (make_socket_non_blocking(sockid) == -1) {
      std::cerr << "Failed to make non-blocking socket!" << std::endl;
      exit(1);
    }
    

    // obtain the port number
    if (getsockname(sockid, (struct sockaddr *)&seederAddr, &socklen) < 0) {
      std::cerr << "Failed to obtain ip:port for this client!\n";
      exit(0);
    };
    btArg->port = seederAddr.sin_port;
    // recalc the id using new port number
    calc_id(btArg->ip, btArg->port, btArg->id);
    printMSG("Binding to the socket ... OK!\n");
    
    //  do not modify the following, it must be printed out to the user
    //  user then can use the ip:port to run a leecher
    std::cout << "\n>>>> Binding to IP:Port: " << btArg->ip 
	      << ":" << ntohs(seederAddr.sin_port) << "\n" << std::endl;

    // set listen to up to  MAX_CONNECTIONS queued connection
    if ( listen(sockid, MAX_CONNECTIONS) < 0 ) {
      std::cerr << "Failed to listen on server socket.\n";
      if (-1 != sockid)
	close(sockid);
      exit(1);
    }
    printMSG("Listening on the server socket ... OK!\n");
  }

  //copy the parsed bt_args
  args = btArg;
}



// accept a new leecher
int SeederManager::acceptLeecher() {
  struct sockaddr_in leecherAddr;
  socklen_t addrLen = sizeof(leecherAddr);
  printMSG("Waiting for a new connection ...\n");
  int leecherSock = accept(sockid, (struct sockaddr *) &leecherAddr, &addrLen);
  if (leecherSock < 0) {
    std::cerr << "Failed to accept a new leecher!\n";
    exit(1);
  }
  
  //mark leecherSock as not yet handshaked
  this->handshaked[leecherSock] = false;
  // sockets[n_sockets++] = leecherSock; // add leecherSock into sockets
  // poll_sockets[n_sockets - 1].fd = leecherSock; // save descriptor
  // poll_sockets[n_sockets - 1].events = POLLIN; // set ..
 

  return leecherSock;
}


bool SeederManager::sendHandshake(int leecherSock) {
  char buf[HANDESHAKE_SIZE];
  createHandshakeMsg(buf, args->bt_info, args->id);
  bool status = sendData(leecherSock, buf, HANDESHAKE_SIZE);
  if (status) {
    printMSG("Sending handshake msg to leecher ... OK!\n");
  }
  return status;
}



bool SeederManager::recvHandshake(int leecherSock) {
  char buf[MAX_BUF_SZIE];
  int t;
  bzero(buf, MAX_BUF_SZIE);
  t = read(leecherSock, buf, MAX_BUF_SZIE);
  if (t < 0) {
    std::cerr << "Failed to handshake!" << std::endl;
    return false;
  }
  buf[20] 
= '\0';
  if (t > 0) {
    // now mark leecherSock as handshaked
    this->handshaked[leecherSock] = true;
    printMSG("Recv handshake msg from with %d... OK! received: %s...\n", leecherSock, buf);
  }
  return true;
}


// TODO
bool SeederManager::sendBitfield(int sock) {
  char buf[MAX_BUF_SZIE];
  int len;
  if (! createBitfield(buf, len) ) {
    return false;
  }
  if (! sendData(sock, buf, len)) {
    std::cerr << "Failed to send bitfield!" << std::endl;
    exit(1);
  }
  return true;
}

bool SeederManager::createBitfield(char *buf, int &len) {
  bt_msg_t *msg = (bt_msg_t *) buf;
  bzero(buf, MAX_BUF_SZIE);
  len = sizeof(bt_msg_t);
  msg->length = len - sizeof(int);
  msg->bt_type = (unsigned char) 5;
  msg->payload.bitfield.size = this->args->bt_info->num_pieces;
  // fill the bitfield somehow
  // TODO
  return true;
}




bool SeederManager::processSock(int sock) {
  char buf[MAX_BUF_SZIE];
  if (!readMSG(sock, buf)) {
    std::cerr << "Failed to read msg from leecher!" << std::endl;
    exit(1);
  }
  bt_msg_t *msg = (bt_msg_t *) buf;
  
  return true;
}







// bool SeederManager::processSock(int sock) {
//   read data from sock and take actions
//   char buf[HANDESHAKE_SIZE];
//   recv(sock, buf, HANDESHAKE_SIZE, 0)
// }






LeecherManager::LeecherManager(bt_args_t *btArg) {
  // set # of sockets as 0
  n_sockets = 0;
  args = btArg;
}

bool LeecherManager::connectSeeders() {
  for (int i = 0; i < MAX_CONNECTIONS; ++i) 
    if (args->peers[i] != NULL) {
      peer_t *peerP = args->peers[i];
      if (! connectSeeder(peerP->sockaddr)) {
	std::cerr << "Failed to connecet to ..." << std::endl;
	exit(1);
      }
    }
  return true;
}

bool LeecherManager::connectSeeder(struct sockaddr_in seederAddr) {
  // create socket
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
      std::cerr << "Failed to create socket" << std::endl;
      return false;
  }
  // add socket into the array of sockets
  sockets[n_sockets++] = sockfd;

  
  // connect to a seeder
  int status = connect(sockfd, (struct sockaddr *)&seederAddr, sizeof(seederAddr));
  if (status < 0) {
    std::cerr << "Failed to connect to the seeder!" << std::endl;
    return false;
  }
  printMSG("Connecting to the seeder %d... OK!\n", sockfd);
  return true;
}


bool LeecherManager::recvHandshake(int sockfd) {
  char buf[MAX_BUF_SZIE];
  int t;
  bzero(buf, MAX_BUF_SZIE);
  t = read(sockfd, buf, MAX_BUF_SZIE);
  if (t < 0) {
    std::cerr << "Failed to handshake!" << std::endl;
    return false;
  }
  std::cerr << "t = " << t << std::endl;
  buf[20] = '\0';
  if (t > 0)
    std::cerr << "Recv handshake msg from seeder ... OK!\n" << std::endl;
  return true;
}

bool LeecherManager::sendHandshake(int sockfd) {
  char buf[MAX_BUF_SZIE];
  bzero(buf, MAX_BUF_SZIE);
  createHandshakeMsg(buf, args->bt_info, args->id);
  bool status = sendData(sockfd, buf, HANDESHAKE_SIZE);
  if (status) {
    printMSG("Sending handshake msg to seeder ... OK!\n");
  }
  return status;
}


bool LeecherManager::createRequest(char *buf, int &len, int index, int begin, int length) {
  bt_request_t request;
  request.index = index;
  request.begin = begin;
  request.length = length;
  bt_msg_t *msg = (bt_msg_t *) buf;
  msg->bt_type = (unsigned char) 6;
  msg->payload.request = request;
  msg->length = sizeof(bt_msg_t) - sizeof(int);
  len = sizeof(bt_request_t);
  return true;
}


bool LeecherManager::sendRequest(int sock, int index, int begin, int length) {
  char buf[MAX_BUF_SZIE];
  int len;
  createRequest(buf, len, index, begin, length);
  if (!sendData(sock, buf, len)) {
    std::cerr << "Failed to send request data to seeder XXX!" << std::endl;
    exit(1);
  }
  return true;
}


bool LeecherManager::processSock(int sock) {
  char buf[MAX_BUF_SZIE];
  if (!readMSG(sock, buf)) {
    std::cerr << "Failed to read msg from leecher!" << std::endl;
    exit(1);
  }
  bt_msg_t *msg = (bt_msg_t *) buf;
  
  return true;
}


bool sendData(int seederSock, char *buf, int n_bytes) {
  int status = write(seederSock, buf, n_bytes);
  if (status < 0) {
    std::cerr << "Failed to send data." << std::endl;
    return false;
  }
  return true;
}


bool createHandshakeMsg(char *buf, bt_info_t *info,  char *id) {
  // clean buf
  memset(buf, 0, HANDESHAKE_SIZE);
  buf[0] = (char) 19;
  strcpy(buf + 1, "BitTorrent Protocol");
  unsigned char *data = (unsigned char *) info;
  unsigned char hash[20];
  SHA1(data, sizeof(data), hash);
  char *md = (char *) hash;
  strncpy(buf + 20 + 8, md, 20);
  strncpy(buf + 20 +8 + 20, id, ID_SIZE);
  return true;
}

// bool recvData(int leecherSock, char *buf, int &n_bytes) {
//   int status = recv(leecherSock, buf, n_bytes, 0);
//   if (status < 0) {
//     std::cerr << "Failed to recieve data." << std::endl;
//     return false;
//   }
//   return true;
// }




bool readMSG(int sock, char *buf) {
  return (read(sock, buf, MAX_BUF_SZIE) > 0);
}


// std::string getPeerDesc(char *ip, unsigned short port) {
//   char id[ID_SIZE];
//   char buf[ID_SIZE * 5];
//   bzero(buf, ID_SIZE  * 5);
  
//   calc_id(ip, port, id);
//   for (int i = 0; i < ID_SIZE; ++i) {
//     sprintf(buf + i, "%02x", id[i]);
//   }
//   std::stringstream ss;
//   ss << ip << ":" << port << "   ID:" << buf;
//   return ss.str();
// }




















