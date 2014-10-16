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
#include <sys/epoll.h>
#include "bt_lib.h"
#include "bt_setup.h"
#include "CLog.h"
//
//    - release memeory of bt_args
//    - update the Readme file




bool VERBOSE = false;


void tests() { // run  tests
}



int main (int argc, char * argv[]){
  bt_args_t bt_args;
  //parse cmd
  parse_args(&bt_args, argc, argv);

  //create log file
  Init(bt_args.log_file, LOG_NOTIFY);

  //read and parse the torrent file here
  bt_info_t torrent = parse_torrent(bt_args.torrent_file);
  bt_args.bt_info = &torrent;

  // set global variable
  if(bt_args.verbose){
    VERBOSE = true;
  }

  //  output args
  printMSG("-------------------- Args -----------------------\n");
  printMSG("verbose: %d\n", bt_args.verbose);
  printMSG("save_file: %s\n", bt_args.save_file);
  printMSG("log_file: %s\n", bt_args.log_file);
  printMSG("torrent_file: %s\n", bt_args.torrent_file);
  printMSG("ip: %s\n", bt_args.ip);
  // output the torrent file information here
  printMSG("\nTorrent INFO:\n");
  printMSG("name: %s\n", torrent.name);
  printMSG("piece_length: %ld bytes\n", torrent.piece_length);
  printMSG("length: %ld bytes\n", torrent.length);
  printMSG("num_pieces: %ld\n", torrent.num_pieces);
  printMSG("\n");

  //log torrent file information
  LOG(LOG_NOTIFY, "Torrent INFO:");
  LOG(LOG_NOTIFY, "name: %s", torrent.name);
  LOG(LOG_NOTIFY, "piece_length: %ld bytes", torrent.piece_length);
  LOG(LOG_NOTIFY, "length: %ld bytes", torrent.length);
  LOG(LOG_NOTIFY, "num_pieces: %ld\n", torrent.num_pieces);

  int efd = 0;
  try {
    //epoll variable
    struct epoll_event event;
    struct epoll_event events[MAX_CONNECTIONS + 5];
    int sock = 0;
    int i;
    // now create manager for peer
    if (bt_args.mode == 's') {
      //-------------------seeder----------------------
      SeederManager MSeeder (&bt_args);
      int sfd = MSeeder.sockid;

      // create epoll
      efd = epoll_create1(0);
      if (efd == -1)
	throw "Failed to create epoll!";
            
      // add to sfd to efd
      event.data.fd = sfd;
      event.events = EPOLLIN | EPOLLET;
      if (epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &event) < 0) {
	close(efd);
	throw "Failed to add socket to epoll!";
      }

      // the event loop
      while (true) {
	int n = epoll_wait(efd, events, MAX_CONNECTIONS, -1);
	// handle all events
	for (i = 0; i < n; ++i) {
	  if ((events[i].events & EPOLLERR) || 
	      (events[i].events & EPOLLHUP) ||
	      (!(events[i].events & EPOLLIN))) {
	    sock = events[i].data.fd;
	    // An error has occured on this fd, or the socket is closed /
	    MSeeder.Close(sock);
	    //MSeeder.delSocket();
	    printMSG("Epoll error or a leecher is disconnected!");
	    continue;
	  }
	  // now process events
	  if (sfd == events[i].data.fd) { // if == sfd
	    sock = MSeeder.acceptLeecher();
	    if (0 == sock){
	      printMSG("Accept leecher failed!\n");
	      continue;
	    }

	    // add sock into efd
	    event.data.fd = sock;
	    event.events = EPOLLIN | EPOLLET;
	    if (epoll_ctl(efd, EPOLL_CTL_ADD, sock, &event) < 0) {
	      close(efd);
	      MSeeder.Close(sock);
	      throw "Failed to add socket to epoll!";
	    }

	    if (!MSeeder.sendHandshake(sock)){ // send handshake msg to leecher
	      // can not handshake
	      epoll_ctl(efd, EPOLL_CTL_DEL, sock, NULL);
	      printMSG("Failed to handshake with %s leecher!\n", MSeeder.getIdfromMap(sock).c_str());
	      MSeeder.Close(sock);
	      //MSeeder.delSocket();
	      continue;
	    }
	  }// if (sfd = ..
	  else { // if != sfd
	    int sock = events[i].data.fd;
	    if (!MSeeder.handshaked[sock]) {
	      if (!MSeeder.recvHandshake(sock)) { // if failed to recv msg
		// can not handshake
		epoll_ctl(efd, EPOLL_CTL_DEL, sock, NULL);
		printMSG("Failed to handshake with %s leecher!\n", MSeeder.getIdfromMap(sock).c_str());
		MSeeder.Close(sock);
		//MSeeder.delSocket();
		continue;
	      } 
	      MSeeder.handshaked[sock] = true; // marked as handshaked
	      if (!MSeeder.sendBitfield(sock)){
		// can not handshake
		epoll_ctl(efd, EPOLL_CTL_DEL, sock, NULL);
		MSeeder.Close(sock);
	      }// send its bitfield to sock
	    }
	    else if (!MSeeder.processSock(sock)) {// work with handshake leecher
	      // can not process
	      epoll_ctl(efd, EPOLL_CTL_DEL, sock, NULL);
	      MSeeder.Close(sock);
	    } // else if
	  } // else
	}//  for
      } // while----------seeder end----------------
    }

    else {
      //--------------leecher-----------------
      LeecherManager MLeecher (&bt_args);
      MLeecher.connectSeeders();
      // create epoll
      efd = epoll_create1(0);
      if (efd == -1) {
	throw "Failed to create epoll!";
      }

      // add all sockfd into efd
      for (i = 0; i < MLeecher.getSockNum(); ++i) {
	sock = MLeecher.getSocket(i);
	MLeecher.handshaked[sock] = false; // marked as not yet handshaked
	event.data.fd = sock;
	event.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(efd, EPOLL_CTL_ADD, sock, &event) < 0) {
	  close(efd);
	  throw "Failed to add sfd to efd!";
	}
      }

      // the event loop
      while (true) {
	int n = epoll_wait(efd, events, MAX_CONNECTIONS, 2000);
	if (MLeecher.isDownloadComplete()) {
	  printMSG("File downloaded, now leecher is stoping ...\n");
	  LOG(LOG_NOTIFY, "FILE DOWNLOAD COMPLETE");
	  break;
	}
	if (MLeecher.getSockNum() <= 0) {
	  printMSG("No alive connection, now leecher is stoping ...\n");
	  break;
	}
	// handle all events
	for (i = 0; i < n; ++i) {
	  //if ((events[i].events | EPOLLIN) <= 0) continue;
	  int sock = events[i].data.fd;
	  if (!MLeecher.handshaked[sock]) { // if is some un-handshaked seeder
	    if (!MLeecher.recvHandshake(sock)) {
	      // handshake fail, torrents are diff
	      printMSG("Can not handshake with %s seeder!\n", MLeecher.getIdfromMap(sock).c_str());
	      epoll_ctl(efd, EPOLL_CTL_DEL, sock, NULL);
	      MLeecher.Close(sock);
	      continue;
	    }
	    else{
	      if (MLeecher.sendHandshake(sock))
		MLeecher.handshaked[sock] = true;
	      else{
		epoll_ctl(efd, EPOLL_CTL_DEL, sock, NULL);
		MLeecher.Close(sock);
	      }
	    }
	  } // if

	  // process handshaked seeder
	  else if (!MLeecher.processSock(sock)) {
	    printMSG("Failed to process the seeder %s!", MLeecher.getIdfromMap(sock).c_str());
	    epoll_ctl(efd, EPOLL_CTL_DEL, sock, NULL);
	    MLeecher.Close(sock);
	  } // else if
	} // for
      }// while
    } // else ----------leecher end----------------------

  } catch (char* msg) {
    //catch all fatal error here and output error msg
    printMSG(msg);
    if (-1 != efd)
      close(efd);
  }

  return 0;
}




