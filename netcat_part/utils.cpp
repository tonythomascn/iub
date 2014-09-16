#include "utils.h"
#include "common.h"
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <openssl/hmac.h>

using namespace std;

/**
   use `addr` and `port` to construct
   `addr_port` with type `struct sockaddr_in`
*/
struct sockaddr_in setupAddr(string addr, int port) {
  struct sockaddr_in addr_port;
  addr_port.sin_addr.s_addr = inet_addr( addr.c_str() );
  addr_port.sin_family = AF_INET;
  addr_port.sin_port = htons( port );

  return addr_port;
} 


struct sockaddr_in setupAddr(struct sockaddr_in ska) {
  return ska;
}


bool printMSG(string msg) {
  if (! VERBOSE) 
    return false;
  cerr << msg;
  return true;
}




void usage(FILE * file){
  fprintf(file,
         "netcat_part [OPTIONS]  dest_ip [file] \n"
         "\t -h           \t\t Print this help screen\n"
         "\t -v           \t\t Verbose output\n"
	 "\t -m \"MSG\"   \t\t Send the message specified on the command line. \n"
	 "                \t\t Warning: if you specify this option, you do not specify a file. \n"
          "\t\t Warning: if you specify this option, you do not specify offset or bytes. \n"
         "\t -p port      \t\t Set the port to connect on (dflt: 6767)\n"
         "\t -n bytes     \t\t Number of bytes to send, defaults whole file\n"
         "\t -o offset    \t\t Offset into file to start sending\n"
         "\t -l           \t\t Listen on port instead of connecting and write output to file\n"
         "                \t\t and dest_ip refers to which ip to bind to (dflt: localhost)\n"
         );
}




/**
 * Given a pointer to a nc_args struct and the command line argument
 * info, set all the arguments for nc_args to function use getopt()
 * procedure.
 *
 * Return:
 *     void, but nc_args will have return results
 **/
void parse_args(nc_args_t * nc_args, int argc, char * argv[]){
  int ch;
  struct hostent * hostinfo;
  //set defaults
  nc_args->n_bytes = MAX_SIZE;
  nc_args->offset = 0;
  nc_args->listen = 0;
  nc_args->port = 6767;
  nc_args->verbose = 0;
  nc_args->message_mode = 0;
 
  while ((ch = getopt(argc, argv, "lm:hvp:n:o:")) != -1) {
    switch (ch) {
    case 'h': //help
      usage(stdout);
      exit(0);
      break;
    case 'l': //listen
      nc_args->listen = 1;
      break;
    case 'p': //port
      nc_args->port = atoi(optarg);
      break;
    case 'o'://offset
      nc_args->offset = atoi(optarg);
      break;
    case 'n'://bytes
      nc_args->n_bytes = atoi(optarg);
      break;
    case 'v':
      nc_args->verbose = 1;
      break;
    case 'm':
      nc_args->message_mode = 1;
      nc_args->message = (char *) malloc(strlen(optarg)+1);
      strncpy(nc_args->message, optarg, strlen(optarg)+1);
      break;
    default:
      fprintf(stderr,"ERROR: Unknown option '-%c'\n",ch);
      usage(stdout);
      exit(1);
    }
  }
 
  argc -= optind;
  argv += optind;
 
  if (argc < 2 && nc_args->message_mode == 0){
    fprintf(stderr, "ERROR: Require ip and file\n");
    usage(stderr);
    exit(1);
  } else if (argc != 1 && nc_args->message_mode == 1) {
    fprintf(stderr, "ERROR: Require ip send/recv from when in message mode\n");
    usage(stderr);
    exit(1);
  }
    if (nc_args->message_mode == 1 && (nc_args->offset != 0 || nc_args->n_bytes != 0))
    {
        fprintf(stderr, "ERROR: message mode do NOT support offset and n_bytes\n");
        usage(stderr);
        exit(1);
    }
 
  if(!(hostinfo = gethostbyname(argv[0]))){
    fprintf(stderr,"ERROR: Invalid host name %s",argv[0]);
    usage(stderr);
    exit(1);
  }

  nc_args->destaddr.sin_family = hostinfo->h_addrtype;
  bcopy((char *) hostinfo->h_addr,
        (char *) &(nc_args->destaddr.sin_addr.s_addr),
        hostinfo->h_length);
   
  nc_args->destaddr.sin_port = htons(nc_args->port);
   
  /* Save file name if not in message mode */
  if (nc_args->message_mode == 0) {
    nc_args->filename = (char *) malloc(strlen(argv[1])+1);
    strncpy(nc_args->filename,argv[1],strlen(argv[1])+1);
  }
  return;
}


// return hashlen + hash + data
char * prependDigest(char *message, int msg_len) {
  char *hashlen = message - EVP_MAX_MD_SIZE - sizeof(int);
  char *hash = message - EVP_MAX_MD_SIZE;
  HMAC(EVP_sha1(), key, KEY_LEN, (unsigned char *) message, 
       msg_len, (unsigned char *) hash, (unsigned int *) hashlen );
  return message - EVP_MAX_MD_SIZE - sizeof(int);
}

bool verifyMessage(char * c_msg, int msg_len) {
  int totpre = EVP_MAX_MD_SIZE + sizeof(int);
  char buf[totpre];
  memcpy(&buf, c_msg, totpre);
  int hashlen;
  memcpy(&hashlen, buf, sizeof(int));
  char * new_msg = prependDigest(c_msg + totpre , msg_len);
  return ( memcmp(new_msg + sizeof(int), buf + sizeof(int), hashlen) == 0 );
}


char * extractMessage(char * c_msg) {
  return c_msg + EVP_MAX_MD_SIZE + sizeof(int);
}
















