#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h> //internet address library
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <sys/stat.h>
#include <arpa/inet.h>

#include <openssl/sha.h> //hashing pieces
#include <stack>
#include <map>
#include "bt_lib.h"
#include "bt_setup.h"
#include "CFileOperation.h"
#include "CLog.h"
#include <iostream>


bool printMSG(std::string msg) {
  if (VERBOSE) {
    std::cout << msg;
  }
  return VERBOSE && !msg.empty();
}

bool printMSG(const char *fmt, ...) {
  if (!VERBOSE) return false;
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
  return true;
}


void calc_id(char * ip, unsigned short port, char *id){
    char data[256];
    int len;
    
    //format print
    len = snprintf(data,256,"%s%u",ip,port);
    
    //id is just the SHA1 of the ip and port string
    SHA1((unsigned char *) data, len, (unsigned char *) id); 
    
    return;
}


/**
 * init_peer(peer_t * peer, int id, char * ip, unsigned short port) -> int
 *
 *
 * initialize the peer_t structure peer with an id, ip address, and a
 * port. Further, it will set up the sockaddr such that a socket
 * connection can be more easily established.
 *
 * Return: 0 on success, negative values on failure. Will exit on bad
 * ip address.
 *   
 **/
int init_peer(peer_t *peer, char * id, char * ip, unsigned short port){
    
    struct hostent * hostinfo;
    //set the host id and port for referece
    memcpy(peer->id, id, ID_SIZE);
    peer->port = port;
    
    //get the host by name
    if((hostinfo = gethostbyname(ip)) ==  NULL){
        perror("gethostbyname failure, no such host?");
        herror("gethostbyname");
        exit(1);
    }
    
    //zero out the sock address
    bzero(&(peer->sockaddr), sizeof(peer->sockaddr));
    
    //set the family to AF_INET, i.e., Iternet Addressing
    peer->sockaddr.sin_family = AF_INET;
    
    //copy the address to the right place
    bcopy((char *) (hostinfo->h_addr), 
          (char *) &(peer->sockaddr.sin_addr.s_addr),
          hostinfo->h_length);
    
    //encode the port
    peer->sockaddr.sin_port = htons(port);
    
    return 0;
    
}

/**
 * print_peer(peer_t *peer) -> void
 *
 * print out debug info of a peer
 *
 **/
void print_peer(peer_t *peer){
    int i;
    
    if(peer){
        printf("peer: %s:%u ",
               inet_ntoa(peer->sockaddr.sin_addr),
               peer->port);
        printf("id: ");
        for(i=0;i<ID_SIZE;i++){
            printf("%02x",peer->id[i]);
        }
        printf("\n");
    }
}



bt_info_t parse_torrent(char * torrent_file)
{
    if (NULL == torrent_file)
    {
        exit(EXIT_FAILURE);
    }
    //copy torrent file over
    std::string strtorrent = torrent_file;
    //strncpy(torrent_info.name, torrent_file, FILE_NAME_MAX);
    if (!strtorrent.find(".torrent"))
    {
        LOG("It is NOT a torrent.", LOG_ERROR);
        exit(EXIT_FAILURE);
    }
    //read torrent
    char buffer[TORRENT_FILE_MAX_SIZE];
    int buffer_size = 0;
    CFileOperation cfileoperation;
    buffer_size = cfileoperation.ReadFile(torrent_file, buffer, 0, TORRENT_FILE_MAX_SIZE);
    if (0 >= buffer_size)
    {
        LOG("Read nothing from the torrent.", LOG_WARNING);
    }
    //analyze torrent content
    return parse_torrent_content_new(buffer, buffer_size);
}





// buf -> "5:abcdefg..."
// return a point to abcde
// set length be 5
// move buf -> "fg..."
char *getIntChars(char*& buf, long &length) {
  char *st = buf;
  while (buf[0] != ':') buf++;
  buf[0] = '\0';
  ++buf;
  length = atoi(st);
  st = buf;
  buf += length;
  return st;
}

// buf -> "i12345e6:sf..."
// return 12345
// move buf -> "6:sf..."
long get_iNUMe(char*& buf) {
  char *st = ++buf;
  while (buf[0] != 'e') buf++;
  buf[0] = '\0';
  buf++;
  return strtol(st, NULL, 0);//atoi(st);
}

// use to keep key / value structure
struct HeadValue {
  char *pValue;
  long length;
  long iValue;
  //char ctype;
};
 
void recusiveParse(char*& buf, HeadValue &hv, bt_info_t& info) {
  // if skip the key -> value (i.e. do not store)
  static bool infoQ = false;
  if (buf[0] == 'e') return;

  if (buf[0] == 'd') {
    ++buf;
    while (buf[0] != 'e') {
      recusiveParse(buf, hv, info);

      // convert hv.pValue to string
      char head[MAX_ITEM_SIZE];
      memcpy(head, hv.pValue, hv.length);
      head[hv.length] = '\0';
      std::string sHead(head);
      if (!infoQ && sHead == "info" && buf[0] == 'd') {
      	infoQ = true;
      }
      if (infoQ && sHead == "files" && buf[0] == 'l') {
	hv.length = 0;
      }
      // get the value part
      recusiveParse(buf, hv,  info);

      if (infoQ) {
	// keep record
	if (sHead == "length") {
	    info.length += hv.iValue;
	}
	if (sHead == "name") {
	  memcpy(info.name, hv.pValue, hv.length);
	  info.name[hv.length] = '\0';
	}
	if (sHead == "piece length") {
	  info.piece_length = hv.iValue;
	}
	if (sHead == "pieces") {
	  info.num_pieces =  (info.length + info.piece_length - 1) / info.piece_length;
	  if (info.num_pieces * ID_SIZE != hv.length) {
	    perror("Error .torrent file!\n");
	    // exit(1);
	    return;
	  }
	  info.piece_hashes = (char **) malloc(info.num_pieces * sizeof(char *));
	  // now read piece_hashes
	  for (int i = 0; i < info.num_pieces; ++i) {
	    char *hashBuf = (char *) malloc(2 + ID_SIZE * sizeof(char));
	    memcpy(hashBuf, i * ID_SIZE + hv.pValue, ID_SIZE);
	    hashBuf[ID_SIZE] = '\0';
	    info.piece_hashes[i] = hashBuf;
	  }
	}
      }// if
    }// while
    buf++;
  } 

  else if (buf[0] == 'l') {
    ++buf;
    while (buf[0] != 'e') recusiveParse(buf, hv, info);
    ++buf;
  }

  else if (buf[0] == 'i') {
    hv.iValue = get_iNUMe(buf);
  }

  else if (buf[0] <= '9' && buf[0] >= '0') {
    hv.pValue = getIntChars(buf, hv.length);
  }
}


bt_info_t parse_torrent_content_new(char * buf, int bufSize) {
  bt_info_t info;
  HeadValue hv;
  recusiveParse(buf, hv, info);
  return info;
}


// clean the memory of info
void releaseInfo(bt_info_t *info) {
  for (int i = 0; i < info->num_pieces; ++i)
    free(info->piece_hashes[i]);
  free(info->piece_hashes);
}




















