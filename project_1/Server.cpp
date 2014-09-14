#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Server.h"
#include "utils.h"
#include "common.h"

const int MAX_QUE = 1;

Server::Server(std::string addr, int port, std::string ofile) {
    // setup the address structure
    serverAddr = setupAddr(addr, port);
    Server(serverAddr, ofile);
    
    m_pCFileOperation = new CFileOperation(ofile);
    if (NULL == m_pCFileOperation)
    {
        fprintf(stderr, "ERROR: Server construct %s,%s,%d\n", __FILE__,__PRETTY_FUNCTION__,__LINE__);
        exit(EXIT_FAILURE);
    }
    // // bind the sock
    // bind(serverSock, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_in));
    
    // // set listen to up to 1 queued connection
    // if ( listen(serverSock, MAX_QUE) < 0 ) {
    //   printMSG( "Failed to listen on server socket.\n" );
    // }
    
}


Server::Server(struct sockaddr_in serverAddr, std::string ofile) {
    // open the socket
    serverSock = socket(AF_INET, SOCK_STREAM, 0);
    // if failed to open
    if (serverSock < 0) {
        std::cerr << "Failed to open the socket!\n";
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
        std::cerr << "Failed to bind the port, check you input!\n";
        exit(1);
    }
    printMSG("Binding to the socket ... OK!\n");
    // set listen to up to 1 queued connection
    if ( listen(serverSock, MAX_QUE) < 0 ) {
        std::cerr << "Failed to listen on server socket.\n";
        exit(1);
    }
    printMSG("Listening on the server socket ... OK!\n");
}

Server::~Server()
{
    if (NULL != m_pCFileOperation)
        delete m_pCFileOperation;
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
    char buffer[BUF_SIZE];
    FILE *fp = fopen(outfile.c_str(), "wb");
    int t;
    while((t =  read(sock, buffer, BUF_SIZE)) ) {
        fwrite(buffer, t, 1, fp);
    }  
    close(sock);
    fclose(fp);
    printMSG("Receiving data ... OK!\n");
    return true;
}

bool Server::ProcessClient(int Socket)
{
    char buffer[BUF_SIZE] = "\0";
    ssize_t ireturn;
    while ((ireturn = read(Socket, buffer, BUF_SIZE)))
    {
        m_pCFileOperation->WriteFile(buffer, BUF_SIZE);
    }
    if (-1 != Socket)
        close(Socket);
    printMSG("Receiving data ... OK!\n");
    return true;
}
