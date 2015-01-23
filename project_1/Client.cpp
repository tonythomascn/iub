#include <unistd.h>
#include <string.h>
#include "Client.h"
#include "utils.h"
#include "common.h"

Client::Client() {
    sockfd = -1; // set as no connection
    m_pCFileOperation = new CFileOperation();
    if (NULL == m_pCFileOperation)
    {
        fprintf(stderr, "ERROR: Server construct %s,%s,%d\n", __FILE__,__PRETTY_FUNCTION__,__LINE__);
        exit(EXIT_FAILURE);
    }
}


/**
 connect to a given address and port
 */
bool Client::connectServer(std::string address, int port) {
    // // create socket
    // if (sockfd == -1) {
    //   sockfd = socket(AF_INET, SOCK_STREAM, 0);
    //   if (sockfd == -1) {
    //     cerr << "Failed to create socket" << endl;
    //     return false;
    //   }
    // }
    
    // setup server address
    server = setupAddr(address, port);
    return connectServer(server);
    // // connect to server
    // int status = connect(sockfd, (struct sockaddr *)&server, sizeof(server));
    // if (status < 0) {
    //   cerr << "Connect failed." << endl;
    //   return false;
    // }
    // printMSG("Connecting to server ... OK!\n");
    // return true;
}

bool Client::connectServer(struct sockaddr_in server) {
    // create socket
    if (sockfd == -1) {
        sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sockfd == -1) {
            std::cerr << "Failed to create socket" << std::endl;
            perror("socket");
            return false;
        }
    }
    
    // setup server address
    //server = setupAddr(address, port);
    
    // connect to server
    int status = connect(sockfd, (struct sockaddr *)&server, sizeof(server));
    if (status < 0) {
        std::cerr << "Connect failed." << std::endl;
        perror("connect");
        return false;
    }
    printMSG("Connecting to server ... OK!\n");
    return true;
}



// For hamc, reserve fixed length for hashlen + hash
// e.g. fixed length = sizeof(int) + EVP_MAX_MD_SIZE

/**
 send data to the connected host
 */

bool Client::sendData(std::string data) {
    int msg_len = strlen( data.c_str() );
    char buf[msg_len + PRE_SIZE];
    memcpy(buf + PRE_SIZE, data.c_str(), msg_len);
    char *new_msg = prependDigest(buf + PRE_SIZE, msg_len);
    int status = send(sockfd, new_msg, msg_len + PRE_SIZE, 0);
    if (status < 0) {
        std::cerr << "Failed to send data." << std::endl;
        return false;
    }
    printMSG("Sending data ... OK!\n");
    return true;
}

bool Client::sendData(std::string fileName, int offset, int n_bytes) {
    char buffer[BUF_SIZE];
    int t = 0;
    char *msg = buffer + PRE_SIZE;
    int avalSize = BUF_SIZE - PRE_SIZE;
    while ( (t = m_pCFileOperation->ReadFile(fileName, buffer, offset + t, avalSize)) && (n_bytes > 0)  ) {
        if (t <= n_bytes) {
            n_bytes -= t;
        }
        else {
            t = n_bytes;
            n_bytes = 0;
        }
        // now send data ...
        int status = send(sockfd, prependDigest(msg, t), t + PRE_SIZE, 0);
        if (status < 0) {
            std::cerr << "Failed to send data." << std::endl;
            return false;
        }
    }
    printMSG("Sending data ... OK!\n");
    return true;
}


/**
 receive data from the connected host
 */
std::string Client::receiveData(int dataSize = 1024) {
    char buffer[dataSize];
    int status = recv(sockfd, buffer, sizeof(buffer), 0);
    if (status < 0) {
        std::cerr << "Failed to receive data!" << std::endl;
    }
    printMSG("Receiving data ... OK!\n");
    return std::string(buffer);
}




Client::~Client() {
    if (-1 != sockfd)
        close(sockfd);
    if (NULL != m_pCFileOperation)
        delete m_pCFileOperation;
}

