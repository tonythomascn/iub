#include "Server.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <stdlib.h>

using namespace std;

Server::Server(string addr, int port, string ofile) {
  // setup the address structure
  serverAddr = setupAddr(addr, port);
  Server(serverAddr, ofile);
  
  // // bind the sock
  // bind(serverSock, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_in));

  // // set listen to up to 1 queued connection
  // if ( listen(serverSock, MAX_QUE) < 0 ) {
  //   printMSG( "Failed to listen on server socket.\n" );
  // }
  
}


Server::Server(struct sockaddr_in serverAddr, string ofile) {
  // open the socket
  serverSock = socket(AF_INET, SOCK_STREAM, 0);
  // if failed to open
  if (serverSock < 0) {
    cerr << "Failed to open the socket!\n";
    exit(1);
  }
  printMSG("Opening the socket ... OK!\n");
  // setup the address structure
  // serverAddr = setupAddr(addr, port);
  // set output file
  outfile = ofile;
  
  // bind the sock
  int status = bind(serverSock, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_in));
  if (status < 0) {
    cerr << "Failed to bind the port, check you input!\n";
    exit(1);
  }
  printMSG("Binding to the socket ... OK!\n");
  // set listen to up to 1 queued connection
  if ( listen(serverSock, MAX_QUE) < 0 ) {
    cerr << "Failed to listen on server socket.\n";
    exit(1);
  }
  printMSG("Listening on the server socket ... OK!\n");
}










int Server::acceptClient() {
  unsigned int clientSize = sizeof(clientAddr);
  clientSock = accept(serverSock, (struct sockaddr *) &clientAddr, &clientSize);
  if (clientSock < 0) {
    printMSG( "Failed to accept client.\n" );
  }
  return clientSock;
}


bool Server::processClient(int sock) {
  char buffer[BUF_SIZE];
  FILE *fp = fopen(outfile.c_str(), "wb");
  int t;
  while((t =  read(sock, buffer, BUF_SIZE)) ) {
    fwrite(buffer, t, 1, fp);
  }  
  close(sock);
  fclose(fp);
  printMSG("Receiving data ... OK!\n");
  return true;
}

















