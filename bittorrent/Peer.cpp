#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Peer.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "bt_lib.h"
#include <openssl/sha.h>
#include <sstream>
// TODO:
//   + in seeder, map a sock to a peer
//   + add desctructor for all classed, very important




SeederManager::SeederManager(bt_args_t *btArg) {
  // set default values
  // n_sockets = 0;



  // set temp peer to represent this seeder
  peer_t thisPeer;
  // assign 0 as its port to automatically assign a port later
  init_peer(&thisPeer, btArg->id, btArg->ip, 0);
  sockaddr_in seederAddr = thisPeer.sockaddr;
  socklen_t socklen = sizeof(seederAddr);
  

  // create a reliable stream socket using TCP
  if ((sockid = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    std::cerr << "Failed to open the socket!\n";
    exit(1);
  }
  printMSG("Opening the socket ... OK!\n");
  // // add this sockid into sockets[0]
  // sockets[n_sockets++] = sockid;

  // // add this sockid into poll_sockets[]
  // poll_sockets[0].fd = sockid;
  // poll_sockets[0].events = POLLIN;



  // bind the sock
  int status = bind(sockid, (struct sockaddr *)&seederAddr, sizeof(struct sockaddr_in));
  if (status < 0) {
    std::cerr << "Failed to bind the port, check you input!\n";
    if (-1 != sockid)
      close(sockid);
    exit(1);
  }
  else {
    // make as non-block socket
    if (make_socket_non_blocking(sockid) == -1) {
      std::cerr << "Failed to make non-blocking socket!" << std::endl;
      exit(1);
    }
    

    // obtain the port number
    if (getsockname(sockid, (struct sockaddr *)&seederAddr, &socklen) < 0) {
      std::cerr << "Failed to obtain ip:port for this client!\n";
      exit(0);
    };
    btArg->port = seederAddr.sin_port;
    // recalc the id using new port number
    calc_id(btArg->ip, btArg->port, btArg->id);
    printMSG("Binding to the socket ... OK!\n");
    
    //  do not modify the following, it must be printed out to the user
    //  user then can use the ip:port to run a leecher
    std::cout << "\n>>>> Binding to IP:Port: " << btArg->ip 
	      << ":" << ntohs(seederAddr.sin_port) << "\n" << std::endl;

    // set listen to up to  MAX_CONNECTIONS queued connection
    if ( listen(sockid, MAX_CONNECTIONS) < 0 ) {
      std::cerr << "Failed to listen on server socket.\n";
      if (-1 != sockid)
	close(sockid);
      exit(1);
    }
    printMSG("Listening on seeder socket ... OK!\n");
  }

  //copy the parsed bt_args
  args = btArg;
  //open the resource file as "rb"
  //note, for a seeder, resource file is read-only
  args->f_save = fopen(args->bt_info->name, "rb");
  if (args->f_save != NULL) {
    std::cerr << "Opening resource file " 
	      << args->bt_info->name << " ... OK!" << std::endl;
  }
}



// accept a new leecher
int SeederManager::acceptLeecher() {
  struct sockaddr_in leecherAddr;
  socklen_t addrLen = sizeof(leecherAddr);
  printMSG("Waiting for a new connection ...\n");
  int leecherSock = accept(sockid, (struct sockaddr *) &leecherAddr, &addrLen);
  if (leecherSock < 0) {
    std::cerr << "Failed to accept a new leecher!\n";
    exit(1);
  }
  
  //mark leecherSock as not yet handshaked
  this->handshaked[leecherSock] = false;
  // sockets[n_sockets++] = leecherSock; // add leecherSock into sockets
  // poll_sockets[n_sockets - 1].fd = leecherSock; // save descriptor
  // poll_sockets[n_sockets - 1].events = POLLIN; // set ..
 

  return leecherSock;
}


bool SeederManager::sendHandshake(int leecherSock) {
  char buf[HANDESHAKE_SIZE];
  createHandshakeMsg(buf, args->bt_info, args->id);
  bool status = sendData(leecherSock, buf, HANDESHAKE_SIZE);
  if (status) {
    printMSG("Sending handshake msg to leecher XXX ... OK!\n");
  }
  return status;
}



bool SeederManager::recvHandshake(int leecherSock) {
  char buf[HANDESHAKE_SIZE];
  int t;
  bzero(buf, HANDESHAKE_SIZE);
  t = read(leecherSock, buf, HANDESHAKE_SIZE);
  if (t < 0) {
    std::cerr << "Failed to handshake with XXX!" << std::endl;
    return false;
  }
  if (t > 0) {
    // now mark leecherSock as handshaked
    this->handshaked[leecherSock] = true;
    printMSG("Recv handshake msg from leecher XXX ... OK!\n");
    // TODO: verify the handshake message, if failed, return false
    bool integ = integrityVerify(args->bt_info, sizeof(bt_info_t) - 10, buf + 20 + 8);
    if (integ) {
      printMSG("Verifying handshake msg from leecher XXX ... OK!\n");
    } else {
      printMSG("Verifying handshake msg from leecher XXX ... Failed!\n");
      return false;
    }
  }
  return true;
}


// TODO
// in our case, a seeder will always have the complete file
// thus bitfield is actually useless
// but we still implement it
bool SeederManager::sendBitfield(int sock) {
  char buf[MAX_BUF_SZIE];
  int len;
  if (! createBitfield(buf, len) ) {
    std::cerr << "Failed to create bitfield!" << std::endl;
    return false;
  }
  std::cerr << "Creating bitfield for XXX ... OK!" << std::endl;
  if (! sendData(sock, buf, len)) {
    std::cerr << "Failed to send bitfield to XXX!" << std::endl;
    exit(1);
  }
  std::cerr << "Sending bitfield to XXX ... OK!" << std::endl;
  return true;
}

bool SeederManager::createBitfield(char *buf, int &len) {
  bt_msg_t *msg = (bt_msg_t *) buf;
  bzero(buf, MAX_BUF_SZIE);
  len = sizeof(bt_msg_t);
  msg->length = len - sizeof(int);
  msg->bt_type = (unsigned char) 5;
  msg->payload.bitfield.size = this->args->bt_info->num_pieces;
  // fill the bitfield somehow
  // TODO
  return true;
}






bool SeederManager::processSock(int sock) {
  char buf[MAX_BUF_SZIE];
  int len;
  int status = readMSG(sock, buf, len);
  if (status < 0) { // error happened
    std::cerr << "Failed to read msg from leecher XXX!" << std::endl;
    exit(1);
  }
  else if (status == 0) { // this sock is disconnected
    close(sock);
    return true;
  }

  bt_msg_t *msg = (bt_msg_t *) buf;
  
  // process different types of msg
  std::cerr << "\nProcessing msg with msg_type [" << (int) (msg->bt_type) 
	    <<  "] from XXX ... " << std::endl;
  switch (msg->bt_type) {
  case 6: // msg:request
    // TODO: send piece
    bt_request_t request = msg->payload.request;
    char buf[MAX_BUF_SZIE];
    int len;
    // calculate the offset
    int offset = request.index * args->bt_info->piece_length + request.begin;
    createPieceMsg(this->args->f_save, buf, len, offset, request); 
    if (sendData(sock, buf, len)) {
      std::cerr << "Sending piece [" << len << " bytes] to leecher XXX ... OK!" << std::endl;
    }
    break;

  // case 4: msg:have
  //     //as a seeder always a seeder, do nothing here
  //   break;

  
  }

  std::cerr << "Processing msg with msg_type [" << (int) (msg->bt_type) 
	    <<  "] from XXX ... OK!\n" << std::endl;
  return true;
}


//TODO: 
//  + release all memory
//  + close all handles
SeederManager::~SeederManager() {
  releaseInfo(this->args->bt_info);
  fclose(this->args->f_save);
}



// bool SeederManager::processSock(int sock) {
//   read data from sock and take actions
//   char buf[HANDESHAKE_SIZE];
//   recv(sock, buf, HANDESHAKE_SIZE, 0)
// }






LeecherManager::LeecherManager(bt_args_t *btArg) {
  // set # of sockets as 0
  n_sockets = 0;
  args = btArg;
  // set downloaded all 0
  memset(downloaded, 0, MAX_PIECES_NUM);
  n_downloaded = 0;

  // open a save_file to save our data
  if ( (args->f_save = fopen(args->save_file, "wb")) == NULL) {
    perror("Can not create save_file!\n");
    exit(1);
  };
  FILE *tmp = args->f_save;
  // reserve the file size as large as the total download size
  // use tmp, do not modify f_save directly
  fseek(tmp, args->bt_info->length, SEEK_SET);
}

bool LeecherManager::connectSeeders() {
  for (int i = 0; i < MAX_CONNECTIONS; ++i) 
    if (args->peers[i] != NULL) {
      peer_t *peerP = args->peers[i];
      if (! connectSeeder(peerP->sockaddr)) {
	std::cerr << "Failed to connecet to XXX ..." << std::endl;
	exit(1);
      }
    }
  return true;
}

bool LeecherManager::connectSeeder(struct sockaddr_in seederAddr) {
  // create socket
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
      std::cerr << "Failed to create socket" << std::endl;
      return false;
  }
  // add socket into the array of sockets
  sockets[n_sockets++] = sockfd;

  
  // connect to a seeder
  int status = connect(sockfd, (struct sockaddr *)&seederAddr, sizeof(seederAddr));
  if (status < 0) {
    std::cerr << "Failed to connect to the seeder XXX!" << std::endl;
    return false;
  }
  printMSG("Connecting to the seeder XXX... OK!\n");
  return true;
}


bool LeecherManager::recvHandshake(int sockfd) {
  char buf[HANDESHAKE_SIZE];
  int t;
  bzero(buf, HANDESHAKE_SIZE);
  t = read(sockfd, buf, HANDESHAKE_SIZE);
  if (t < 0) {
    std::cerr << "Failed to handshake!" << std::endl;
    return false;
  }
  if (t > 0)
    std::cerr << "Recv handshake msg from XXX ... OK!" << std::endl;

  // TODO: verify the handshake message, if failed, return false
  bool integ = integrityVerify(args->bt_info, sizeof(bt_info_t) - 10, buf + 20 + 8);
  if (integ) {
    printMSG("Verifying handshake msg from seeder XXX ... OK!\n");
  } else {
    printMSG("Verifying handshake msg from seeder XXX ... Failed!\n");
    return false;
  }
  return true;
}

bool LeecherManager::sendHandshake(int sockfd) {
  char buf[MAX_BUF_SZIE];
  bzero(buf, MAX_BUF_SZIE);
  createHandshakeMsg(buf, args->bt_info, args->id);
  bool status = sendData(sockfd, buf, HANDESHAKE_SIZE);
  if (status) {
    printMSG("Response handshake msg from seeder XXX ... OK!\n");
  }
  return status;
}


bool LeecherManager::createRequest(char *buf, int &len, int index, int begin, int length) {
  bt_msg_t *msg = (bt_msg_t *) buf;
  bt_request_t request;
  int file_size = args->bt_info->length;
  int offset = index * args->bt_info->piece_length + begin;
  if (length > file_size - offset) {
    length = file_size - offset;
  }
  request.index = index;
  request.begin = begin;
  request.length = length;

  msg->payload.request = request;
  // add other field
  msg->bt_type = (unsigned char) 6;

  msg->length = sizeof(bt_msg_t) - sizeof(int);
  printMSG("Creating request (index %d, begin %d, length %d)\n", index, begin, length);
  len = sizeof(bt_msg_t);
  return true;
}


bool LeecherManager::sendRequest(int sock, int index, int begin, int length) {
  char buf[MAX_BUF_SZIE];
  int len;
  createRequest(buf, len, index, begin, length);
  if (!sendData(sock, buf, len)) {
    std::cerr << "Failed to send request data to seeder XXX!" << std::endl;
    exit(1);
  }
  return true;
}



bool LeecherManager::sendRequest(int sock) {
  int index = pickNeedPiece(downloaded, this->args->bt_info->num_pieces);
  if (index < 0) {
    // download complete
    // take some action
    return false; // no request sent, all pieces are either completed or in progress
  }
  else { // some valid indx
    this->downloaded[index] = 1; // set as in progress
    if (this->sendRequest(sock, index, 0, this->args->bt_info->piece_length)) {
      printMSG("Send request with (index %d, begin %d, length %d) to XXX ... OK!\n", 
	       index, 0, this->args->bt_info->piece_length);
      return true;
    } else {return false;}
  }

}

bool LeecherManager::isDownloadComplete() {
  return (n_downloaded == args->bt_info->num_pieces);
}


bool LeecherManager::processSock(int sock) {
  char buf[MAX_BUF_SZIE];
  int len;
  if (readMSG(sock, buf, len) < 0) {
    std::cerr << "Failed to read msg from seeder XXX!" << std::endl;
    exit(1);
  }
  bt_msg_t *msg = (bt_msg_t *) buf;
  std::cerr << "\nProcessing msg with msg_type [" << (int) (msg->bt_type) <<  "] from XXX ... " << std::endl;
  switch (msg->bt_type) { // process diff types
    
  case 5: // recv  a bitfield
    // need to send a request
    puts("bitfield recv!\n");
    if (!this->isDownloadComplete())
      this->sendRequest(sock);
    break; 


  case 7: // recieve a piece
    bt_piece_t piece = msg->payload.piece; // assign the piece
    // write to file based on the info in piece
    this->downloaded[piece.index] = 2; // set this piece as download
    n_downloaded++; // ++ # of downloaded pieces
    int offset = piece.index * args->bt_info->piece_length + piece.begin;
    int piece_length = len - sizeof(bt_msg_t); // calc the size of the piece
    bool integ = integrityVerify(buf + sizeof(bt_msg_t), piece_length, 
			          args->bt_info->piece_hashes[piece.index]);
    if (!integ) {
      std::cerr << "Recieved broken piece from XXX at index [" << piece.index << "]!" << std::endl;
      // resend
      this->downloaded[piece.index] = 0; // set from 1 [in progress] to 2 [in need]
      this->sendRequest(sock); // randomly resend a request
      break;
    } else {
      std::cerr << "Verifying recieved piece from XXX at index ["
		<< piece.index << "] ... OK!" << std::endl;
    }


    if (saveToFile(args->f_save, buf + sizeof(bt_msg_t), offset, piece_length) > 0) { // save to file
      printMSG("Piece %d from XXX, offset [%d], length [%d] downloaded and saved to file!\n", 
	       piece.index, offset, piece_length);
    } else {
      printMSG("Failed to save piece %d:  offset [%d], length [%d]!\n", 
	       piece.index, offset, piece_length);
      }
    // TODO: save this piece to file
    // TODO: send have msg
    // TODO: send another request
    if (!this->isDownloadComplete())
      this->sendRequest(sock);
    break;
  } // switch
  std::cerr << "Msg with msg_type [" << (int) (msg->bt_type) <<  "] from XXX ... processed!\n" << std::endl;
  return true;
}



LeecherManager::~LeecherManager() {
  fclose(args->f_save); // close f_save handle
  for (int i = 0; i < this->n_sockets; ++i) { // close all connect sockets
    close(this->sockets[i]); 
  }
}


bool sendData(int seederSock, char *buf, int n_bytes) {
  int sent_size = write(seederSock, buf, n_bytes);
  char *tmp = buf;
  while (sent_size > 0 && n_bytes > sent_size) { // not yet finish the sending
    tmp += sent_size;
    n_bytes -= sent_size; // remaining size
    sent_size = write(seederSock, tmp, n_bytes);
  }
  if (sent_size < 0) {
    std::cerr << "Failed to send data." << std::endl;
    return false;
  }
  
  return true;
}


bool createHandshakeMsg(char *buf, bt_info_t *info,  char *id) {
  // clean buf
  memset(buf, 0, HANDESHAKE_SIZE);
  buf[0] = (char) 19;
  strcpy(buf + 1, "BitTorrent Protocol");
  unsigned char hash[ID_SIZE + 5];
  bzero(hash, ID_SIZE);
  // the reason to -10 is that info->piece_hashes will not keep same
  SHA1((unsigned char *) info, sizeof(bt_info_t) - 10, hash);
  strncpy(buf + 20 + 8, (char *) hash, ID_SIZE);
  strncpy(buf + 20 +8 + 20, id, ID_SIZE);
  return true;
}

// bool recvData(int leecherSock, char *buf, int &n_bytes) {
//   int status = recv(leecherSock, buf, n_bytes, 0);
//   if (status < 0) {
//     std::cerr << "Failed to recieve data." << std::endl;
//     return false;
//   }
//   return true;
// }




int readMSG(int sock, char *buf, int &len) {
  char *tmp = buf;
  len = 0;
  int t;
  bzero(buf, MAX_BUF_SZIE);
  while (true) {
    t = read(sock, tmp, 100);
    if (t <= 0) { // no further data available
      break;
    }
    else {
      len += t;
      tmp += t;
    }
  }

  bt_msg_t *msg = (bt_msg_t *) buf;
  int need_to_read = msg->length + sizeof(int);
  if (msg->length == 0) need_to_read = 0;
  if (len != need_to_read && msg->length != 0) {
    std::cerr << len << " bytes read,  " << need_to_read << " bytes needed!" << std::endl;
    exit(1);
  }
  //  std::cerr << len << " bytes read,  " << need_to_read << " bytes needed!" << std::endl;
  return (int) msg->bt_type;
}



int pickNeedPiece(int *indexes, int len) {
  for (int i = 0; i < len; ++i) 
    if (indexes[i] == 0) return i;
  return -1;
}



bool createPieceMsg(FILE *fp, char *buf, int &len, int offset,  bt_request_t request) {
  if (fp == NULL) {
    std::cerr << "File can not open when create the piece msg!" << std::endl;
    exit(1);
  }
  // do not directly operate on fp, 
  // fp should not be modified
  FILE *tmp = fp;

  // construct msg
  bt_msg_t *msg =  (bt_msg_t *) buf;
  msg->length = sizeof(bt_msg_t) + request.length - sizeof(int);
  msg->bt_type = (unsigned char) 7;
  bt_piece_t *piece = &(msg->payload.piece); // point to payload
  piece->index = request.index;
  piece->begin = request.begin;
  
  

  printMSG("Reading - offset [%d] - length [%d] \n", offset, request.length);
  fseek(tmp, offset, SEEK_SET);
  len = fread(buf + sizeof(bt_msg_t), 1, request.length, tmp);
  if (len != request.length) {
    std::cerr << "Reading error!" << std::endl;
    exit(1);
  }
  len = request.length + sizeof(bt_msg_t);
  return true;
}




int saveToFile(FILE *fp, char *src, int offset, int length) {
  if (fp == NULL) {
    perror("Invalid file!");
    exit(1);
  }
  FILE *tmp = fp;
  fseek(tmp, offset, SEEK_SET);
  int t = fwrite(src, 1, length, tmp);
  if (t != length) {
    return -1;
  }
  return length;
}


bool integrityVerify(void *buf, int length, char *hash) {
  unsigned char hash_buf[ID_SIZE + 2];
  bzero(hash_buf, ID_SIZE + 2);
  SHA1((unsigned char *) buf, length, hash_buf);
  // printMSG("Verifying data integrity...  OK!\n");
  return (memcmp(hash_buf, hash, ID_SIZE) == 0);
}


// std::string getPeerDesc(char *ip, unsigned short port) {
//   char id[ID_SIZE];
//   char buf[ID_SIZE * 5];
//   bzero(buf, ID_SIZE  * 5);
  
//   calc_id(ip, port, id);
//   for (int i = 0; i < ID_SIZE; ++i) {
//     sprintf(buf + i, "%02x", id[i]);
//   }
//   std::stringstream ss;
//   ss << ip << ":" << port << "   ID:" << buf;
//   return ss.str();
// }




















