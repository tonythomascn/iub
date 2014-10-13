#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Peer.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "bt_lib.h"
#include <openssl/sha.h>
#include <fcntl.h>
SeederManager::SeederManager(bt_args_t *btArg) {
	// set temp peer to represent this seeder
	peer_t thisPeer;
	// assign 0 as its port to automatically assign a port later
	init_peer(&thisPeer, btArg->id, btArg->ip, 0);
	sockaddr_in seederAddr = thisPeer.sockaddr;
	socklen_t socklen = sizeof(seederAddr);

	// create a reliable stream socket using TCP
	if ((sockid = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		std::cerr << "Failed to open socket!\n";
		throw "Failed to open socket!";
	}
	printMSG("Opening the socket ... OK!\n");

	int flags; 
	/* Set socket to non-blocking */
	if ((flags = fcntl(sockid, F_GETFL, 0)) < 0) 
	{ 
		/* Handle error */ 
		std::cerr << "Failed to set socket into nonblocking mode!\n";
		if (-1 != sockid)
			close(sockid);
		throw "Failed to get socket flag!";
	} 

	if (fcntl(sockid, F_SETFL, flags | O_NONBLOCK) < 0) 
	{ 
		/* Handle error */ 
		std::cerr << "Failed to set socket into nonblocking mode!\n";
		if (-1 != sockid)
			close(sockid);
		throw "Failed to set socket into nonblocking mode!";

	}

	// bind the sock
	int status = bind(sockid, (struct sockaddr *)&seederAddr, sizeof(struct sockaddr_in));
	if (status < 0) {
		std::cerr << "Failed to bind the port, check you input!\n";
		if (-1 != sockid)
			close(sockid);
		throw "Failed to bind the port!";
	}
	else {
		// obtain the port number
		if (getsockname(sockid, (struct sockaddr *)&seederAddr, &socklen) < 0) {
			std::cerr << "Failed to obtain ip:port for this client!\n";
			if (-1 != sockid)
				close(sockid);
			throw "Failed to obtain port for this client!";
		};
		btArg->port = seederAddr.sin_port;
		// recalc the id using new port number
		calc_id(btArg->ip, btArg->port, btArg->id);
		printMSG("Binding to the socket ... OK!\n");

		//  do not modify the following, it must be printed out to the user
		//  user then can use the ip:port to run a leecher
		std::cout << "\n>>>> Binding to IP:Port: " << btArg->ip 
			<< ":" << ntohs(seederAddr.sin_port) << "\n" << std::endl;

		// set listen to up to  MAX_CONNECTIONS queued connection
		if ( listen(sockid, MAX_CONNECTIONS) < 0 ) {
			std::cerr << "Failed to listen on server socket.\n";
			if (-1 != sockid)
				close(sockid);
			throw "Failed to listen on this socket!";
		}
		printMSG("Listening on the server socket ... OK!\n");
	}

	//copy the parsed bt_args
	args = btArg;
}



// accept a new leecher
int SeederManager::acceptLeecher() {
	struct sockaddr_in leecherAddr;
	socklen_t addrLen = sizeof(leecherAddr);
	printMSG("Waiting for a new connection ...\n");
    int leecherSock = 0;
    while (true)
    {
	leecherSock = accept(sockid, (struct sockaddr *) &leecherAddr, &addrLen);
	if (leecherSock < 0) {
        if (EAGAIN != errno){
            std::cerr << "Failed to accept a new leecher!\n";
            break;
        }
	}
        else
            break;
        sleep(1);
    }
	return leecherSock;
}


bool SeederManager::handshake(int leecherSock) {
	char buf[HANDESHAKE_SIZE];
	createHandshakeMsg(buf, args->bt_info, args->id);
	return sendData(leecherSock, buf, HANDESHAKE_SIZE);
}




LeecherManager::LeecherManager(bt_args_t *btArg) {
	sockfd = -1;
	args = btArg;
}

bool LeecherManager::connectSeeders() {
	for (int i = 0; i < MAX_CONNECTIONS; ++i) 
		if (args->peers[i] != NULL) {
			peer_t *peerP = args->peers[i];
			try {
				connectSeeder(peerP->sockaddr);
			} catch (const char* msg) {
				std::cerr << msg << std::endl;
			}
		}
	return true;
}

bool LeecherManager::connectSeeder(struct sockaddr_in seederAddr) {
	bool breturn = false;
	// create socket
	if (sockfd == -1) {
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd == -1) {
			//std::cerr << "Failed to create socket" << std::endl;
			throw "Failed to create socket!";
		}
	}

	int flags;
	/* Set socket to non-blocking */
	if ((flags = fcntl(sockfd, F_GETFL, 0)) < 0)
	{
		/* Handle error */
		//std::cerr << "Failed to set socket into nonblocking mode!\n";
		if (-1 != sockfd)
			close(sockfd);
		throw "Failed to get socket flag!";
	}

	if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0)
	{
		/* Handle error */
		//std::cerr << "Failed to set socket into nonblocking mode!\n";
		if (-1 != sockfd)
			close(sockfd);
		throw "Failed to set socket into nonblocking mode!";

	}

	// connect to a seeder
    int status = 0;
    
    while (true){
    status = connect(sockfd, (struct sockaddr *)&seederAddr, sizeof(seederAddr));
	if (status < 0) {
        printf("%d\n", errno);
        if (EINPROGRESS != errno){
            if (-1 != sockfd)
                close(sockfd);
		//std::cerr << "Failed to connect to the seeder!" << std::endl;
		    throw "Failed to connect to seeder!";
            break;
        }
	}
	else{

		printMSG("Connecting to the seeder ... OK!\n");
		breturn = true;
        break;
	}
        sleep(1);
    }
	return breturn;
}


bool LeecherManager::handshake() {
	char buf[HANDESHAKE_SIZE];
	int status = recv(sockfd, buf, HANDESHAKE_SIZE, 0);
	if (status < 0) {
		std::cerr << "Failed to handshake!" << std::endl;
		return false;
	}
	buf[20] = '\0';
	printMSG("Handshaking ... OK! received: %s...\n", buf);
	return true;
}


bool sendData(int seederSock, char *buf, int n_bytes) {
	int status = send(seederSock, buf, n_bytes, 0);
	if (status < 0) {
		std::cerr << "Failed to send data." << std::endl;
		return false;
	}
	return true;
}


bool createHandshakeMsg(char *buf, bt_info_t *info,  char *id) {
	// clean buf
	memset(buf, 0, HANDESHAKE_SIZE);
	buf[0] = (char) 19;
	strcpy(buf + 1, "BitTorrent Protocol");
	unsigned char *data = (unsigned char *) info;
	unsigned char hash[20];
	SHA1(data, sizeof(data), hash);
	char *md = (char *) hash;
	strncpy(buf + 20 + 8, md, 20);
	strncpy(buf + 20 +8 + 20, id, ID_SIZE);
	return true;
}

// bool recvData(int leecherSock, char *buf, int &n_bytes) {
//   int status = recv(leecherSock, buf, n_bytes, 0);
//   if (status < 0) {
//     std::cerr << "Failed to recieve data." << std::endl;
//     return false;
//   }
//   return true;
// }

















