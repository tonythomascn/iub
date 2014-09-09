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


bool processArgs(nc_args_t args) {
  // nc_args_t args;
  // set VERBOSE
  if (args.verbose == 1) {
    VERBOSE = true;
  }
  // if listen mode
  if (args.listen == 1) {
    Server s = Server(args.destaddr, args.filename);
    int sock = s.acceptClient();
    return s.processClient(sock);
  }
  // if message_mode
  if (args.message_mode == 1) {
    Client c;
    c.connectServer(args.destaddr);
    c.sendData(args.message);
    return true;
  }
  // now, not message_mode
  return false;
}




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
  processArgs(nc_args);
  return 0;
}















