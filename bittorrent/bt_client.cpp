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
#include "Peer.h"
#include "bt_lib.h"
#include <sys/epoll.h>

// TODO:
//    - release memeory of bt_args
//    - update the Readme file




bool VERBOSE = false;


void tests() { // run  tests
}


#include "bt_lib.h"
#include "bt_setup.h"
#include "CLog.h"
int main (int argc, char * argv[]){
  bt_args_t bt_args;
  
  // run test
  tests();


  parse_args(&bt_args, argc, argv);
  // parse the torrent file
  
  CLog.Init(bt_args.log_file, LOG_NOTIFY);
  
  //read and parse the torrent file here
  bt_info_t torrent = parse_torrent(bt_args.torrent_file);
  bt_args.bt_info = &torrent;
  
  // set global variable
  if(bt_args.verbose){
    VERBOSE = true;
  }


  
  // // LOG("Starting to parse torrent file...", LOG_NOTIFY);
  
  
  //  printMSG("Parsing .torrent file ...  DONE!\n");
  printMSG("-------------------- Args -----------------------\n");
  printMSG("verbose: %d\n", bt_args.verbose);
  printMSG("save_file: %s\n", bt_args.save_file);
  printMSG("log_file: %s\n", bt_args.log_file);
  printMSG("torrent_file: %s\n", bt_args.torrent_file);
  printMSG("ip: %s\n", bt_args.ip);
  // print out the torrent file arguments here
  printMSG("\nTorrent INFO:\n");
  printMSG("name: %s\n", torrent.name);
  printMSG("piece_length: %ld bytes\n", torrent.piece_length);
  printMSG("length: %ld bytes\n", torrent.length);
  printMSG("num_pieces: %ld\n", torrent.num_pieces);
  printMSG("\n");
  
  

  // if(VERBOSE){
  //   for(int i=0; i<MAX_CONNECTIONS; i++){
  //     if(bt_args.peers[i] != NULL)
  //       print_peer(bt_args.peers[i]);
  //   }
  // }


  // now create manager for peer
  if (bt_args.mode == 's') { // run as seeder
    SeederManager MSeeder (&bt_args);
    struct epoll_event event;
    struct epoll_event events[MAX_CONNECTIONS + 5];
    int sfd = MSeeder.sockid;

    // create epoll
    int efd = epoll_create1(0);
    if (efd == -1) {
      std::cerr << "Failed to create epoll!" << std::endl;
      exit(1);
    }
    
    // add to sfd to efd
    event.data.fd = sfd;
    event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &event) < 0) {
      std::cerr << "Failed to add sfd to efd!" << std::endl;
      exit(1);
    }

    // the event loop
    while (true) {
      int n = epoll_wait(efd, events, MAX_CONNECTIONS, -1);
      //      std::cerr << "n = " << n << std::endl;

      // handle all events
      for (int i = 0; i < n; ++i) { 
	// TODO: if err happened 
	if (sfd == events[i].data.fd) { // if == sfd
	  int sock = MSeeder.acceptLeecher();
	  if (make_socket_non_blocking(sock) < 0) {
	    std::cerr << "Failed to make sock non-block!" << std::endl;
	    exit(1);
	  }

	  // add sock into efd
	  event.data.fd = sock;
	  event.events = EPOLLIN | EPOLLET;
	  if (epoll_ctl(efd, EPOLL_CTL_ADD, sock, &event) < 0) {
	    std::cerr << "Failed to add event to efd!" << std::endl;
	    exit(1);
	  }

	  MSeeder.sendHandshake(sock); // send handshake msg to leecher
	}// if (sfd = ..
	else { // if != sfd
	  int sock = events[i].data.fd;
	  if (!MSeeder.handshaked[sock]) {
	    MSeeder.recvHandshake(sock); // recieve handshake msg from leecher
	    MSeeder.handshaked[sock] = true; // marked as handshaked
	    MSeeder.sendBitfield(sock); // send its bitfield to sock
	  }

	  // work with handshake leecher
	  else if (!MSeeder.processSock(sock)) {
	    std::cerr << "Failed to process the leecher!" << std::endl;
	    exit(1);
	  } // else if
	} // else
      }//  for
    } // while
  }



  else {  // run as leecher
    LeecherManager MLeecher (&bt_args);
    MLeecher.connectSeeders();
    struct epoll_event event;
    struct epoll_event events[MAX_CONNECTIONS + 5];

    // create epoll
    int efd = epoll_create1(0);   
    if (efd == -1) {
      std::cerr << "Failed to create epoll!" << std::endl;
      exit(1);
    }

    // add all sockfd into efd
    for (int i = 0; i < MLeecher.n_sockets; ++i) {
      int sock = MLeecher.sockets[i];
      MLeecher.handshaked[sock] = false; // marked as not yet handshaked
      event.data.fd = sock;
      event.events = EPOLLIN | EPOLLET;
      if (make_socket_non_blocking(sock) < 0) {
	std::cerr << "Failed to make sock non-block!" << std::endl;
	exit(1);
      }

      if (epoll_ctl(efd, EPOLL_CTL_ADD, sock, &event) < 0) {
	std::cerr << "Failed to add sfd to efd!" << std::endl;
	exit(1);
      }
    }

    // the event loop
    while (true) {
      int n = epoll_wait(efd, events, MAX_CONNECTIONS, 2000);
      if (MLeecher.isDownloadComplete()) {
	std::cerr << "File downloaded, now leecher is stoping ..." << std::endl;
	break;
      }
      // handle all events
      for (int i = 0; i < n; ++i) {
	if ((events[i].events | EPOLLIN) <= 0) continue;
	int sock = events[i].data.fd;

	if (!MLeecher.handshaked[sock]) { // if is some un-handshaked seeder
	  MLeecher.recvHandshake(sock);
	  MLeecher.sendHandshake(sock);
	  MLeecher.handshaked[sock] = true;
	} // if

	// process handshaked seeder
	else if (!MLeecher.processSock(sock)) {
	    std::cerr << "Failed to process the seeder XXX!" << std::endl;
	    exit(1);
	} // else if	

      } // for
    }// while
  } // else run as a leecher
  
  
  



  releaseInfo(bt_args.bt_info);
  return 0;
}




