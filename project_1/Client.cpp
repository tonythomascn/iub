#include "Client.h"
#include "utils.h"
#include <iostream> 
#include <string.h>
using namespace std;

Client::Client() {
  sockfd = -1; // set as no connection
}


/**
   connect to a given address and port
*/
bool Client::connectServer(string address, int port) {
  // create socket
  if (sockfd == -1) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
      cerr << "Failed to create socket" << endl;
      return false;
    }
  }
  
  // setup server address
  server = setupAddr(address, port);

  // connect to server
  int status = connect(sockfd, (struct sockaddr *)&server, sizeof(server));
  if (status < 0) {
    cerr << "Connect failed." << endl;
    return false;
  }
  
  return true;
}

/**
   send data to the connected host
*/

bool Client::sendData(string data) {
  int status = send(sockfd, data.c_str(), strlen( data.c_str()), 0);
  if (status < 0) {
    cerr << "Failed to send data." << endl;
    return false;
  }
  return true;
}


/**
   receive data from the connected host
*/
string Client::receiveData(int dataSize = 1024) {
  char buffer[dataSize];
  int status = recv(sockfd, buffer, sizeof(buffer), 0);
  if (status < 0) {
    cerr << "Failed to receive data!" << endl;
  }
  return string(buffer);
}




Client::~Client() {
  close(sockfd);
}




















