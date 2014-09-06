#include "Server.h"
#include "utils.h"
#include <iostream>
#include <cstring>

using namespace std;

Server::Server(string addr, int port) {
  // open the socket
  serverSock = socket(AF_INET, SOCK_STREAM, 0);
  // if failed to open
  if (serverSock < 0) {
    cerr << "Failed to open the socket!" << endl;
  }
  // setup the address structure
  serverAddr = setupAddr(addr, port);

  
  // bind the sock
  bind(serverSock, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_in));

  // set listen to up to 1 queued connection
  if ( listen(serverSock, MAX_QUE) < 0 ) {
    cerr << "Failed to listen on server socket." << endl;
  }
  
}

int Server::acceptClient() {
  unsigned int clientSize = sizeof(clientAddr);
  clientSock = accept(serverSock, (struct sockaddr *) &clientAddr, &clientSize);
  if (clientSock < 0) {
    cerr << "Failed to accept client." << endl;
  }
  return clientSock;
}


bool Server::processClient(int sock) {
  char buffer[BUF_SIZE];
  while( read(sock, buffer, BUF_SIZE) ) {
    cout << string(buffer);
  }  
  close(sock);
  return true;
}

















