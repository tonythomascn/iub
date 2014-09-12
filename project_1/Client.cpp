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
  // // create socket
  // if (sockfd == -1) {
  //   sockfd = socket(AF_INET, SOCK_STREAM, 0);
  //   if (sockfd == -1) {
  //     cerr << "Failed to create socket" << endl;
  //     return false;
  //   }
  // }
  
  // setup server address
  server = setupAddr(address, port);
  return connectServer(server);
  // // connect to server
  // int status = connect(sockfd, (struct sockaddr *)&server, sizeof(server));
  // if (status < 0) {
  //   cerr << "Connect failed." << endl;
  //   return false;
  // }
  // printMSG("Connecting to server ... OK!\n");
  // return true;
}

bool Client::connectServer(struct sockaddr_in server) {
  // create socket
  if (sockfd == -1) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
      cerr << "Failed to create socket" << endl;
      return false;
    }
  }
  
  // setup server address
  //server = setupAddr(address, port);

  // connect to server
  int status = connect(sockfd, (struct sockaddr *)&server, sizeof(server));
  if (status < 0) {
    cerr << "Connect failed." << endl;
    return false;
  }
  printMSG("Connecting to server ... OK!\n");
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
  printMSG("Sending data ... OK!\n");
  return true;
}

bool Client::sendData(string fileName, int offset, int n_bytes) {
  FILE *fp = fopen(fileName.c_str(), "rb");
  if (fp == NULL) {
    cerr << "Failed to open file." << endl;
    return false;
  }
  if (offset > 0) offset--;
  fseek(fp, offset, SEEK_SET);
  char buffer[BUF_SIZE];
  int t;
  while ( (t = fread(buffer, 1, BUF_SIZE, fp)) && (n_bytes > 0)  ) {
    if (t <= n_bytes) {
      n_bytes -= t;
    }
    else {
      t = n_bytes;
      n_bytes = 0;
    }
    // now send data ...
    int status = send(sockfd, buffer, t, 0);
    if (status < 0) {
      cerr << "Failed to send data." << endl;
      fclose(fp);
      return false;
    }
  }
  fclose(fp);
  printMSG("Sending data ... OK!\n");
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
  printMSG("Receiving data ... OK!\n");
  return string(buffer);
}




Client::~Client() {
  close(sockfd);
}




















