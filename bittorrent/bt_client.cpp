#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h> //ip hdeader library (must come before ip_icmp.h)
#include <netinet/ip_icmp.h> //icmp header
#include <arpa/inet.h> //internet address library
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>

bool VERBOSE = false;

#include "bt_lib.h"
#include "bt_setup.h"
#include "CLog.h"
int main (int argc, char * argv[]){
  bt_args_t bt_args;
  int i;

  parse_args(&bt_args, argc, argv);
  CLog.Init(bt_args.log_file, LOG_NOTIFY);

  // set global variable
  if(bt_args.verbose){
    VERBOSE = true;
  }

  //read and parse the torrent file here
  // LOG("Starting to parse torrent file...", LOG_NOTIFY);
  bt_info_t torrent = parse_torrent(bt_args.torrent_file);
  
// // print args, not require in the milestone
//   printMSG("Args:\n");
//   printMSG("verbose: %d\n", bt_args.verbose);
//   printMSG("save_file: %s\n", bt_args.save_file);
//   printMSG("log_file: %s\n", bt_args.log_file);
//   printMSG("torrent_file: %s\n", bt_args.torrent_file);

  // print out the torrent file arguments here
  printMSG("Torrent INFO:\n");
  printMSG("name: %s\n", torrent.name);
  printMSG("piece_length: %d bytes\n", torrent.piece_length);
  printMSG("length: %d bytes\n", torrent.length);
  printMSG("num_pieces: %d\n", torrent.num_pieces);
  printMSG("\n");


  if(VERBOSE){
    for(i=0; i<MAX_CONNECTIONS; i++){
      if(bt_args.peers[i] != NULL)
        print_peer(bt_args.peers[i]);
    }
  }


  



  //main client loop, not required in the milestone
  // printMSG("Starting Main Loop\n");
  while(false){

    //try to accept incoming connection from new peer
       
    
    //poll current peers for incoming traffic
    //   write pieces to files
    //   udpdate peers choke or unchoke status
    //   responses to have/havenots/interested etc.
    
    //for peers that are not choked
    //   request pieaces from outcoming traffic

    //check livelenss of peers and replace dead (or useless) peers
    //with new potentially useful peers
    
    //update peers, 

  }

  // release mememery of torrent
  // freeTorrent(torrent);
  return 0;
}
