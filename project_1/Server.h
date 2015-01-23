#ifndef _SERVER_H_
#define _SERVER_H_

#include <netinet/in.h>
#include <iostream>
#include "CFileOperation.h"
/**
   Server class using TCP
 */
class Server {
    
public:
    Server(std::string address, int port, std::string ofile);
    Server(struct sockaddr_in serverAddr, std::string ofile);
    ~Server();
    // accept a client, return the sock
    int acceptClient();
    // process the accepted client
    bool processClient(int sock);
    // process the accepted client, write buffer to file
    bool ProcessClient(int Socket);
    
private:
    int serverSock, clientSock;
    struct sockaddr_in serverAddr, clientAddr;
    std::string outfile;
    CFileOperation *m_pCFileOperation;
};

#endif










