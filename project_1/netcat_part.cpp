#include <iostream>
#include <stdlib.h>
#include "Client.h"

#include "Server.h"

using namespace std;


/**
 * class to hold all relevant state
 **/

// class NetcatArgs{
// public:
//     struct sockaddr_in destaddr; //destination/server address
//     unsigned short port; //destination/listen port
//     bool listen; //listen flag
//     int n_bytes; //number of bytes to send
//     int offset; //file offset
//     int verbose; //verbose output info
//     bool message_mode; // retrieve input to send via command line
//     std::string message; // if message_mode is activated, this will store the message
//     std::string filename; //input/output file
// };





int main(int argc, char * argv[]){
  //  NetcatArgs nc_args;
  
  // initialize the arguments
  // parseArgs(&nc_args, argc, argv);
  // take action based on the args
  string addr = "127.0.0.1";
  int port = 6767;
  
  if (argc != 2) {
    exit(1);
  }

  if (string(argv[1]) == "0") {
    Server sv = Server(addr, port);
    int sock = sv.acceptClient();
    sv.processClient(sock);
  }
  else {
    Client c;
    c.connectServer(addr, 6767);
    c.sendData("Hello World!\n");
  }
  return 0;
}
