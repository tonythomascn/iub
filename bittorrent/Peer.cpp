#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Peer.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "bt_lib.h"


SeederManager::SeederManager(bt_args_t *btArg) {
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


  // bind the sock
  int status = bind(sockid, (struct sockaddr *)&seederAddr, sizeof(struct sockaddr_in));
  if (status < 0) {
    std::cerr << "Failed to bind the port, check you input!\n";
    if (-1 != sockid)
      close(sockid);
    exit(1);
  }
  else {
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
    std::cout << "\nBinding to IP:Port: " << btArg->ip 
	      << ":" << ntohs(seederAddr.sin_port) << std::endl;

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
  return leecherSock;
}





LeecherManager::LeecherManager(bt_args_t *btArg) {
  sockfd = -1;
  args = btArg;
}

bool LeecherManager::connectSeeder() {
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
  if (sockfd == -1) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
      std::cerr << "Failed to create socket" << std::endl;
      return false;
    }
  }
  
  
  // connect to server
  int status = connect(sockfd, (struct sockaddr *)&seederAddr, sizeof(seederAddr));
  if (status < 0) {
    std::cerr << "Failed to connect to the seeder!" << std::endl;
    return false;
  }
  printMSG("Connecting to the seeder ... OK!\n");
  return true;
}
















