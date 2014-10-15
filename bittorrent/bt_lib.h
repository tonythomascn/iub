#ifndef _BT_LIB_H
#define _BT_LIB_H

//standard stuff
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <string>
//networking stuff
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <fcntl.h>
#include "bt_lib.h"

// control the verbose output
// defined in bt_client
extern bool VERBOSE;

/*Maximum file name size, to make things easy*/
#define FILE_NAME_MAX 1024
/*Maxium torrent file size 1MB*/
#define TORRENT_FILE_MAX_SIZE 1024 * 1024
/*Maxium TORRENT item size 5000*/
#define MAX_ITEM_SIZE  5000
/*Maxium number of connections*/
#define MAX_CONNECTIONS 5

/* size of handshake message */
#define HANDESHAKE_SIZE (20 + 8 + 20 + 20)

/* max buffer size */
#define MAX_BUF_SZIE (1024 * 1024)

/* max # of pieces */
#define MAX_PIECES_NUM 1000

/* max length for an ip*/
#define MAX_IP 40


/*Different BitTorrent Message Types*/
#define BT_CHOKE 0
#define BT_UNCHOKE 1
#define BT_INTERSTED 2
#define BT_NOT_INTERESTED 3
#define BT_HAVE 4
#define BT_BITFILED 5
#define BT_REQUEST 6
#define BT_PIECE 7
#define BT_CANCEL 8

/*size (in bytes) of id field for peers*/
#define ID_SIZE 20


//holds information about a peer
typedef struct _peer_t{
  unsigned char id[ID_SIZE]; //the peer id
  unsigned short port; //the port to connect n
  struct sockaddr_in sockaddr; //sockaddr for peer
  int choked; //peer choked?
  int interested; //peer interested?
    _peer_t(){
        port = 0;
        memset(id, ID_SIZE, 0);
    }
    ~_peer_t(){
        port = 0;
        memset(id, ID_SIZE, 0);
    }
}peer_t;


//holds information about a torrent file
typedef struct _bt_info_t{
  char name[FILE_NAME_MAX]; //name of file
  long piece_length; //number of bytes in each piece
  long length; //length of the file in bytes
  long num_pieces; //number of pieces, computed based on above two values
  char ** piece_hashes; //pointer to 20 byte data buffers containing the sha1sum of each of the pieces
    _bt_info_t(){
       memset(name, FILE_NAME_MAX, 0);
        piece_length = 0;
        length = 0;
        num_pieces = 0;
        piece_hashes = NULL;
    }
    ~_bt_info_t(){
        memset(name, FILE_NAME_MAX, 0);
        piece_length = 0;
        length = 0;
        num_pieces = 0;
        if (NULL != piece_hashes)//it is released in releaseInfo
            piece_hashes = NULL;
    }
} bt_info_t;


//holds all the agurments and state for a running the bt client
typedef struct _bt_args_t{
  int verbose; //verbose level
  char mode;
  char save_file[FILE_NAME_MAX];//the filename to save to
  FILE * f_save;
  char log_file[FILE_NAME_MAX];//the log file
  char torrent_file[FILE_NAME_MAX];// *.torrent file
    int n_peers;
  peer_t * peers[MAX_CONNECTIONS]; // array of peer_t pointers
  char id[ID_SIZE]; // id for this client
  //  unsigned int id; //this bt_clients id
  char ip[MAX_IP]; // to keep the string of an ip
  int port; // port for this client
  /* set once torrent is parsed */
  bt_info_t * bt_info; //the parsed info for this torrent
    _bt_args_t(){
        memset(save_file, FILE_NAME_MAX, 0);
        memset(log_file, FILE_NAME_MAX, 0);
        memset(torrent_file, FILE_NAME_MAX, 0);
        f_save = NULL;
        n_peers = 0;
        memset(ip, MAX_IP, 0);
        port = 0;
        bt_info = NULL;
    }
    ~_bt_args_t(){
        memset(save_file, FILE_NAME_MAX, 0);
        memset(log_file, FILE_NAME_MAX, 0);
        memset(torrent_file, FILE_NAME_MAX, 0);
        if (NULL != f_save){
            fclose(f_save);
            f_save = NULL;
        }
        if (NULL != peers){
            int i;
            for (i = 0; i < n_peers; i++)
                free(peers[i]);
        }
        n_peers = 0;
        memset(ip, MAX_IP, 0);
        port = 0;
        if (NULL != bt_info)
            bt_info = NULL;
    }
} bt_args_t;


/**
 * Message structures
 **/

typedef struct _bt_bitfield_t{
  char bitfield[MAX_PIECES_NUM]; //bitfield where each bit represents a piece that
                   //the peer has or doesn't have
  size_t size;//size of the bitfiled
} bt_bitfield_t;

typedef struct _bt_request_t{
  long index; //which piece index
  long begin; //offset within piece
  long length; //amount wanted, within a power of two
} bt_request_t;

typedef struct _bt_piece_t{
  long index; //which piece index
  long begin; //offset within piece
  char piece[0]; //pointer to start of the data for a piece
} bt_piece_t;



typedef struct bt_msg{
  long length; //length of remaining message,
              //0 length message is a keep-alive message
  unsigned int bt_type;//type of bt_mesage

  //payload can be any of these
  union { 
    bt_bitfield_t bitfield;//send a bitfield
    int have; //what piece you have
    bt_piece_t piece; //a peice message
    bt_request_t request; //request messge
    bt_request_t cancel; //cancel message, same type as request
    char data[0];//pointer to start of payload, just incase
  }payload;

} bt_msg_t;


// print msg to std::cout when VERBOSE is true
// for error, should use perror etc.
bool printMSG(std::string msg);
// take the same args as printf
// print to standard output when VERBOSE is true
bool printMSG(const char *fmt, ...);



int parse_bt_info(bt_info_t * bt_info);

/*choose a random id for this node*/
unsigned int select_id();

/*propogate a peer_t struct and add it to the bt_args structure*/
int add_peer(peer_t *peer, bt_args_t *bt_args, char * hostname, unsigned short port);

/*drop an unresponsive or failed peer from the bt_args*/
int drop_peer(peer_t *peer, bt_args_t *bt_args);

/* initialize connection with peers */
int init_peer(peer_t *peer, char * id, char * ip, unsigned short port);


/*calc the peer id based on the string representation of the ip and
  port*/
void calc_id(char * ip, unsigned short port, char * id);

/* print info about this peer */
void print_peer(peer_t *peer);

/* check status on peers, maybe they went offline? */
int check_peer(peer_t *peer);

/*check if peers want to send me something*/
int poll_peers(bt_args_t *bt_args);

/*send a msg to a peer*/
int send_to_peer(peer_t * peer, bt_msg_t * msg);

/*read a msg from a peer and store it in msg*/
int read_from_peer(peer_t * peer, bt_msg_t *msg);


/* save a piece of the file */
int save_piece(bt_args_t * bt_args, bt_piece_t * piece);

/*load a piece of the file into piece */
int load_piece(bt_args_t * bt_args, bt_piece_t * piece);

/*load the bitfield into bitfield*/
int get_bitfield(bt_args_t * bt_args, bt_bitfield_t * bitfield);

/*compute the sha1sum for a piece, store result in hash*/
int sha1_piece(bt_args_t * bt_args, bt_piece_t * piece, unsigned char * hash);


/*Contact the tracker and update bt_args with info learned, 
  such as peer list*/
int contact_tracker(bt_args_t * bt_args);
/*Parse torrent and get all the information needed
 */
bt_info_t parse_torrent(char * torrent_file);




// buf -> "5:abcdefg..."
// return a point to abcde
// set length be 5
// move buf -> "fg..."
char *getIntChars(char *& buf, long &length);

// buf -> "i12345e6:sf..."
// return 12345
// move buf -> "6:sf..."
long get_iNUMe(char *& buf);

bt_info_t parse_torrent_content_new(char * buf, int bufSize);

// clean the memory of bt_info_t info
void releaseInfo(bt_info_t *);


// by lgp8819@gmail.com
int make_socket_non_blocking(int sfd);

//get id from the socket
std::string getIdfromPeer(char*ip, unsigned short port);

#endif



