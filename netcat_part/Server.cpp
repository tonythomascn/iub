#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Server.h"
#include "utils.h"
#include "common.h"
using namespace std;

Server::Server(string addr, int port, string ofile) {
    // setup the address structure
    serverAddr = setupAddr(addr, port);
    Server(serverAddr, ofile);
    
    // // bind the sock
    // bind(serverSock, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_in));
    
    // // set listen to up to 1 queued connection
    // if ( listen(serverSock, MAX_QUE) < 0 ) {
    //   printMSG( "Failed to listen on server socket.\n" );
    // }
    
}


Server::Server(struct sockaddr_in serverAddr, string ofile) {
    // open the socket
    if (-1 != serverSock)
        serverSock = socket(AF_INET, SOCK_STREAM, 0);
    // if failed to open
    if (serverSock < 0) {
        cerr << "Failed to open the socket!\n";
        exit(1);
    }
    printMSG("Opening the socket ... OK!\n");
    // setup the address structure
    // serverAddr = setupAddr(addr, port);
    // set output file
    outfile = ofile;
    
    // bind the sock
    int status = bind(serverSock, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_in));
    if (status < 0) {
        cerr << "Failed to bind the port, check you input!\n";
        if (-1 != serverSock)
            close(serverSock);
        exit(1);
    }
    else
    {
        printMSG("Binding to the socket ... OK!\n");
        // set listen to up to 1 queued connection
        if ( listen(serverSock, MAX_QUE) < 0 ) {
            cerr << "Failed to listen on server socket.\n";
            if (-1 != serverSock)
                close(serverSock);
            exit(1);
        }
        printMSG("Listening on the server socket ... OK!\n");
    }
}

Server::~Server()
{
    if (-1 != serverSock)
        close(serverSock);
    if (-1 != clientSock)
        close(clientSock);
}

int Server::acceptClient() {
    unsigned int clientSize = sizeof(clientAddr);
    clientSock = accept(serverSock, (struct sockaddr *) &clientAddr, &clientSize);
    if (clientSock < 0) {
        printMSG( "Failed to accept client.\n" );
    }
    return clientSock;
}


bool Server::processClient(int sock) {
    char buffer[BUF_SIZE] = "\0";
    FILE *fp = fopen(outfile.c_str(), "wb");
    if (NULL == fp)
    {
        perror("fopen failed");
        return false;
    }
    int t = 0;
    while((t = read(sock, buffer, BUF_SIZE)) ) {
        if ( !verifyMessage(buffer, t - PRE_SIZE) ) {
            cerr << "Data pocket is not as the same as the original!" << endl;
            // what action do I need to take ?
        }
        fwrite(buffer + PRE_SIZE, t - PRE_SIZE, 1, fp);
        if (ferror(fp))
        {
            perror("fwrite");
            clearerr(fp);
            break;
        }
        
    }
    if (-1 != sock)
        close(sock);
    if (NULL != fp)
        fclose(fp);
    printMSG("Receiving data ... OK!\n");
    return true;
}

