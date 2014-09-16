#include "Client.h"
#include "utils.h"
#include <iostream> 
#include <string.h>
//#include <openssl/hmac.h>
using namespace std;

Client::Client() {
    sockfd = -1; // set as no connection
}


/**
 connect to a given address and port
 */
bool Client::connectServer(string address, int port) {
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
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            cerr << "Failed to create socket" << endl;
            return false;
        }
    }
    
    // setup server address
    //server = setupAddr(address, port);
    
    // connect to server
    int status = connect(sockfd, (struct sockaddr *)&server, sizeof(server));
    if (status < 0) {
        cerr << "Connect failed." << endl;
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

bool Client::sendData(string data) {
    int msg_len = strlen( data.c_str() );
    char buf[msg_len + PRE_SIZE];
    memcpy(buf + PRE_SIZE, data.c_str(), msg_len);
    char *new_msg = prependDigest(buf + PRE_SIZE, msg_len);
    int status = send(sockfd, new_msg, msg_len + PRE_SIZE, 0);
    if (status < 0) {
        cerr << "Failed to send data." << endl;
        return false;
    }
    printMSG("Sending data ... OK!\n");
    return true;
}

bool Client::sendData(string fileName, int offset, int n_bytes) {
    FILE *fp = fopen(fileName.c_str(), "rb");
    if (NULL == fp) {
        perror("fopen");
        cerr << "Failed to open file." << endl;
        return false;
    }
    //  if (offset > 0) offset--;
    if (-1 == fseek(fp, offset, SEEK_SET))
    {
        perror("fseek");
        cerr << "Failed to seek file." << endl;
        if (NULL != fp)
            fclose(fp);
        return false;
    }
    char buffer[BUF_SIZE] = "\0";
    int t = 0;
    char *msg = buffer + PRE_SIZE;
    int avalSize = BUF_SIZE - PRE_SIZE;
    while ( (t = fread(msg, 1, avalSize, fp)) && (n_bytes > 0)  ) {
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
            cerr << "Failed to send data." << endl;
            if (NULL != fp)
                fclose(fp);
            return false;
        }
    }
    if (NULL != fp)
        fclose(fp);
    printMSG("Sending data ... OK!\n");
    return true;
}


/**
 receive data from the connected host
 */
string Client::receiveData(int dataSize = 1024) {
    char buffer[dataSize];
    memset(buffer, 0, dataSize);
    int status = recv(sockfd, buffer, sizeof(buffer), 0);
    if (status < 0) {
        cerr << "Failed to receive data!" << endl;
    }
    printMSG("Receiving data ... OK!\n");
    return string(buffer);
}



Client::~Client() {
    if (-1 != sockfd)
        close(sockfd);
}

