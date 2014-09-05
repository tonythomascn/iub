#include <iostream>
#include "Client.h"

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
  
  Client c;
  string addr = "149.165.180.113";
  c.connectServer(addr, 80);
  c.sendData("GET / HTTP/1.1\r\n\r\n");
  cout << c.receiveData(1024) << endl;
  return 0;
}
