#include <iostream>
#include <stdlib.h>
#include "Client.h"
#include "common.h"
#include "Server.h"
#include "utils.h"


using namespace std;

bool VERBOSE = false;

/**
 * class to hold all relevant state
 **/





int main(int argc, char * argv[]){
  //  NetcatArgs nc_args;
  
  // initialize the arguments
  // parseArgs(&nc_args, argc, argv);
  // take action based on the args
  // string addr = "127.0.0.1";
  // int port = 6767;
  
  // if (argc != 2) {
  //   exit(1);
  // }

  // if (string(argv[1]) == "0") {
  //   Server sv = Server(addr, port);
  //   int sock = sv.acceptClient();
  //   sv.processClient(sock);
  // }
  // else {
  //   Client c;
  //   c.connectServer(addr, 6767);
  //   c.sendData("Hello World!\n");
  // }
  nc_args_t nc_args;
  parse_args(&nc_args, argc, argv);
  return 0;
}















