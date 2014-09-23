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
////holds information about a torrent file
//typedef struct {
//    char name[FILE_NAME_MAX]; //name of file
//    int piece_length; //number of bytes in each piece
//    int length; //length of the file in bytes
//    int num_pieces; //number of pieces, computed based on above two values
//    char ** piece_hashes; //pointer to 20 byte data buffers containing the sha1sum of each of the pieces
//} bt_info_t;
bt_info_t parse_torrent_content(void * torrent_content, unsigned int nbytes){
    bt_info_t torrent_info;
    int ilength = 0;
    int icounter = 1;
    std::string strcontent = static_cast<char*>(torrent_content);
    std::string strtmp = "";
    std::map<int, std::string> metainfomap;
    std::map<int,std::string>::iterator it;
    std::stack<char> metainfostack;
    for (unsigned int i = 0; i < nbytes; i++) {
        metainfostack.push(strcontent[i]);
        if ('e' == strcontent[i])
        {
            //d<contents>e or l<contents>e or i<integer>e
            metainfostack.pop();
            while (1){
                if ('0' <= metainfostack.top() && '9' >= metainfostack.top())
                {
                    strtmp = metainfostack.top() + strtmp;
                    metainfostack.pop();
                }
                else
                    break;
            }
            if ("" != strtmp)
            {
                metainfomap[icounter - 1] += ':' + strtmp;
                ilength = static_cast<int>(strtmp.length());
                //printf("strtmp=%s\n", strtmp.c_str());
                //printf("%s\n", (strcontent.substr(i + 1, strcontent.length() - i - 1)).c_str());
                strtmp = "";
                ilength = 0;
            }
            metainfostack.pop();//pop 'i' or 'd' or 'l'
        }//if ('e' == strcontent[i])
        if (':' == strcontent[i])
        {
            //<length>:<contents>
            metainfostack.pop();
            while (1){
                if ('0' <= metainfostack.top() && '9' >= metainfostack.top())
                {
                    strtmp = metainfostack.top() + strtmp;
                    metainfostack.pop();
                }
                else
                    break;
            }
            if ("" != strtmp)
            {
                ilength = atoi(strtmp.c_str());
                strtmp = strcontent.substr(i + 1, ilength);
                metainfomap.insert(std::pair<int, std::string>(icounter, strtmp));
                icounter++;
                i += ilength;
                //printf("strtmp=%s\n", strtmp.c_str());
                //printf("%s\n", (strcontent.substr(i + 1, strcontent.length() - i - 1)).c_str());
                strtmp = "";
                ilength = 0;
            }
        }//if (':' == strcontent[i])
        //if ('0' <= ch && '9' >= ch){
        //    //printf(" %c is a number\n" ,ch);
        //}
        //else if(('a' <= ch && 'z' >= ch)||('A' <= ch && 'Z' >= ch)){
        //    //printf(" %c is a letter\n" ,ch);
        //    if ('d' == ch)
        //        printf("here comes the dictionary\n");
        //    else if ('l' == ch)
        //        printf("here comes the list\n");
        //    else if ('i' == ch)
        //    {
        //        printf("here comes the interger\n");
        //        
        //    }//else if ('i' == ch)
        //    if ('e' == ch)
        //    {
        //        metainfostack.pop();
        //        while (1){
        //            if ('0' <= metainfostack.top() && '9' >= metainfostack.top())
        //            {
        //                strtmp = metainfostack.top() + strtmp;
        //                metainfostack.pop();
        //            }
        //            else
        //                break;
        //        }
        //        if ("" != strtmp)
        //        {
        //            metainfomap[icounter] += strtmp;
        //            ilength = static_cast<int>(strtmp.length());
        //            printf("strtmp=%s\n", strtmp.c_str());
        //            printf("%s\n", (strcontent.substr(i + 1, strcontent.length() - i - 1)).c_str());
        //            strtmp = "";
        //            ilength = 0;
        //        }
        //        metainfostack.pop();//pop i
        //    }//if ('e' == ch)
        //}
        //else{
        //    if (':' == ch)
        //    {
        //        metainfostack.pop();
        //        while (1){
        //            if ('0' <= metainfostack.top() && '9' >= metainfostack.top())
        //            {
        //                strtmp = metainfostack.top() + strtmp;
        //                metainfostack.pop();
        //            }
        //            else
        //                break;
        //        }
        //        if ("" != strtmp)
        //        {
        //            ilength = atoi(strtmp.c_str());
        //            strtmp = strcontent.substr(i + 1, ilength);
        //            metainfomap.insert(std::pair<int, std::string>(icounter, strtmp));
        //            icounter++;
        //            i += ilength;
        //            printf("strtmp=%s\n", strtmp.c_str());
        //            printf("%s\n", (strcontent.substr(i + 1, strcontent.length() - i - 1)).c_str());
        //            strtmp = "";
        //            ilength = 0;
        //        }
        //    }//if (':' == ch)
        //    //printf(" %c is not a number or letter\n" ,ch) ;
        //}
    }

    for (it = metainfomap.begin(); it != metainfomap.end(); it++)
    {
        if ("name" == it->second)
        {
            strncpy(torrent_info.name, (++it)->second.c_str(), FILE_NAME_MAX);
        }
        else if ("pieces" == it->second)
        {
            //printf("hash = %lu", (++it)->second.length());
            ilength = static_cast<int>((++it)->second.length());
            torrent_info.piece_hashes = (char**)malloc(sizeof(char**));
            torrent_info.num_pieces = ilength / 20 + 1;
            for (int i = 0; i < ilength / 20; i++)
            {
                torrent_info.piece_hashes[i] = (char*)malloc(sizeof(char*)*20);
                strncpy(torrent_info.piece_hashes[i], it->second.c_str(), 20);
            }
        }
        else if (std::string::npos != it->second.find(":"))
        {
            strtmp = it->second.substr(it->second.find(":") + 1, it->second.length() - it->second.find(":") - 1);
            ilength = atoi(strtmp.c_str());
            if (std::string::npos != it->second.find("length") && std::string::npos != it->second.find("piece"))
            {
                torrent_info.piece_length = ilength;
            }
            else if (0 == it->second.find("length"))
            {
                torrent_info.length = ilength;
            }
        }
    }
    return torrent_info;
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
char *getIntChars(char*& buf, int &length) {
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
int get_iNUMe(char*& buf) {
  char *st = ++buf;
  while (buf[0] != 'e') buf++;
  buf[0] = '\0';
  buf++;
  return atoi(st);
}


bt_info_t parse_torrent_content_new(char * buf, int bufSize) {
  bt_info_t info;
  // skip first 'd'
  buf++;
  char *stopSign = buf + bufSize - 2;
  bool valid = false;
  while (buf != stopSign) {
    // in each loop step, get "head -> value"
    // now set head ...
    int length;
    char *p = getIntChars(buf, length);
    char head[MAX_ITEM_SIZE];
    memcpy(head, p, length);
    head[length] = '\0';
    std::string sHead(head);
    //    printMSG(sHead + '\n');
    // set following to skip url, just record info structure
    if (sHead == "info" && buf[0] == 'd' && !valid) {
      buf++;
      valid = true;
      continue;
    }

    // now set value
    int iValue = -23333;
    char *pValue = NULL;
    if (buf[0] == 'i') {
      iValue = get_iNUMe(buf);
    } 
    else if (buf[0] <= '9' && buf[0] >= '0') {
      pValue = getIntChars(buf, length);
    }

  
    if (valid) {
      if (sHead == "length") {
	info.length = iValue;
      }
      if (sHead == "name") {
	memcpy(info.name, pValue, length);
	info.name[length] = '\0';
      }
      if (sHead == "piece length") {
	info.piece_length = iValue;
      }
      if (sHead == "pieces") {
	buf = stopSign;
      }
    }// if (valid) ..
  }// while
  info.num_pieces =  (info.length + info.piece_length - 1) / info.piece_length;
  return info;
}



















