#ifndef _UTILS_H_
#define _UTILS_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

using namespace std;


/**
 * Structure to hold all relevant state
 **/
typedef struct nc_args{
  struct sockaddr_in destaddr; //destination/server address
  unsigned short port; //destination/listen port
  unsigned short listen; //listen flag
  int n_bytes; //number of bytes to send
  int offset; //file offset
  int verbose; //verbose output info
  int message_mode; // retrieve input to send via command line
  char * message; // if message_mode is activated, this will store the message
  char * filename; //input/output file
}nc_args_t;


/**
 * usage(FILE * file) -> void
 *
 * Write the usage info for netcat_part to the give file pointer.
 */
void usage(FILE * file);


struct sockaddr_in setupAddr(string addr, int port);
struct sockaddr_in setupAddr(struct sockaddr_in ska);

// default output stream = std::cerr
bool printMSG(string msg);

void parse_args(nc_args_t * nc_args, int argc, char * argv[]);

// make sure that message has at least (sizeof(int) + EVP_MAX_MD_SIZE) pre-avaible bytes
char * prependDigest(char *message, int msg_len);
bool verifyMessage(char * c_msg, int msg_len);
char * extractMessage(char * c_msg);
#endif



















