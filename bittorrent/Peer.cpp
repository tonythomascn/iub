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
#include <sstream>
#include <errno.h>
#include "CLog.h"
//   + add desctructor for all classed, very important
//use to format output message
char message[FILE_NAME_MAX] = "\0";

SeederManager::SeederManager(bt_args_t *btArg):
n_sockets(0),
m_ullUploaded(0){

	// set temp peer to represent this seeder
	peer_t thisPeer;
	// assign 0 as its port to automatically assign a port later
	init_peer(&thisPeer, btArg->id, btArg->ip, 0);
	sockaddr_in seederAddr = thisPeer.sockaddr;
	socklen_t socklen = sizeof(seederAddr);

	// create a reliable stream socket using TCP
	if ((sockid = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		//std::cerr << "Failed to open socket!\n";
		throw "Failed to open socket!";
	}
	printMSG("Opening the socket ... OK\n");

	// bind the sock
	int status = bind(sockid, (struct sockaddr *)&seederAddr, sizeof(struct sockaddr_in));
	if (status < 0) {
		//std::cerr << "Failed to bind the port, check you input!\n";
		if (-1 != sockid)
			close(sockid);
		throw "Failed to bind the port!";
	}
	else {
		// make as non-block socket
		if (-1 == make_socket_non_blocking(sockid)){
			if (-1 != sockid)
				close(sockid);
		}

		// obtain the port number
		if (getsockname(sockid, (struct sockaddr *)&seederAddr, &socklen) < 0) {
			//std::cerr << "Failed to obtain ip:port for this client!\n";
			if (-1 != sockid)
				close(sockid);
			throw "Failed to obtain ip:port for this client!";
		}
		btArg->port = seederAddr.sin_port;
		//store seeder itself information
        ip_sock_id addr;
        //printf("%s, %u\n", inet_ntoa(seederAddr.sin_addr), ntohs(seederAddr.sin_port));
        memcpy(addr.id, getIdfromPeer(inet_ntoa(seederAddr.sin_addr), ntohs(seederAddr.sin_port)).c_str(), ID_SIZE*2);
        memcpy(addr.ip, inet_ntoa(seederAddr.sin_addr), MAX_IP);
        addr.port = ntohs(seederAddr.sin_port);
        this->m_ipsockidMap.insert(std::pair<int, ip_sock_id>(sockid, addr));

		//  do not modify the following, it must be printed out to the user
		//  user then can use the ip:port to run a leecher
		snprintf(message, FILE_NAME_MAX, "\nBinding to IP:Port: %s:%u\n", btArg->ip, ntohs(seederAddr.sin_port));
        fprintf(stdout, "%s", message);

		// set listen to up to  MAX_CONNECTIONS queued connection
		if ( listen(sockid, MAX_CONNECTIONS) < 0 ) {
			//std::cerr << "Failed to listen on server socket.\n";
			if (-1 != sockid)
				Close(sockid);
			throw "Failed to listen on this socket!";
		}
		printMSG("Listening on seeder socket ... OK\n");
	}

	//copy the parsed bt_args
	args = btArg;
	//open the resource file as "rb"
	//note, for a seeder, resource file is read-only
	args->f_save = fopen(args->bt_info->name, "rb");
	if (args->f_save == NULL) {
		snprintf(message, FILE_NAME_MAX, "Open resource file %s failed", args->bt_info->name);
		throw message;
	}
}

// accept a new leecher
int SeederManager::acceptLeecher() {
	struct sockaddr_in leecherAddr;
	socklen_t addrLen = sizeof(leecherAddr);
	printMSG("Waiting for a new connection ...\n\n\n");
	int leecherSock = 0;
	while (true)
	{
		leecherSock = accept(sockid, (struct sockaddr *) &leecherAddr, &addrLen);
		if (leecherSock < 0) {
			if (EAGAIN != errno){
                //failed to accept leecher
				//std::cerr << "Failed to accept a new leecher!\n";
				throw "Failed to accept a new leecher!";
				break;
			}
		}
		else{
            //accept successfully
			if (make_socket_non_blocking(leecherSock) < 0) {
                if (-1 != leecherSock)
                    close(leecherSock);
			}
			break;
		}
		sleep(1);
    }
    //store leecher information and record handshake state
    if (0 != leecherSock){
        ip_sock_id addr;
        memcpy(addr.id, getIdfromPeer(inet_ntoa(leecherAddr.sin_addr), ntohs(leecherAddr.sin_port)).c_str(), ID_SIZE*2);
        memcpy(addr.ip, inet_ntoa(leecherAddr.sin_addr), MAX_IP);
        addr.port = ntohs(leecherAddr.sin_port);
        this->m_ipsockidMap.insert(std::pair<int, ip_sock_id>(leecherSock, addr));
        //mark leecherSock as not yet handshaked
        this->handshaked[leecherSock] = false;
        this->n_sockets++;
    }

	return leecherSock;
}
bool SeederManager::getAddrfromMap(int socket, ip_sock_id &addr){
    std::map<int, ip_sock_id>::iterator it = m_ipsockidMap.find(socket);
    if (it != m_ipsockidMap.end()){
        addr = it->second;
        return true;
    }
    else{
        return false;
    }
}
std::string SeederManager::getIdfromMap(int socket){
    std::map<int, ip_sock_id>::iterator it = m_ipsockidMap.find(socket);
    if (it != m_ipsockidMap.end()){
        return it->second.id;
    }
    else{
        return "";
    }
}
bool SeederManager::Close(int socket){
    //peer number minus one
    n_sockets--;
    std::map<int, ip_sock_id>::iterator it = m_ipsockidMap.find(socket);
    if (it != m_ipsockidMap.end()){
        //erase the leecher info
        m_ipsockidMap.erase(socket);
    }
    //shutdown connection
    if (-1 != socket)
        close(socket);
    return true;
}

bool SeederManager::sendHandshake(int leecherSock) {
	char buf[HANDESHAKE_SIZE];
    ip_sock_id addr;
    if (!getAddrfromMap(leecherSock, addr)){
        printMSG("Get leecher info failed!\n");
        return false;
    }
    //log the handshake
    LOG(LOG_NOTIFY, "HANDSHAKE INIT peer:%s port:%u id:%s\n", addr.ip, addr.port, addr.id);
	createHandshakeMsg(buf, args->bt_info, args->id);
	bool status = sendData(leecherSock, buf, HANDESHAKE_SIZE);
	if (status) {
		printMSG("Sending handshake msg to leecher " + getIdfromMap(leecherSock) + " ... OK\n");
        LOG(LOG_NOTIFY, "HANDSHAKE SUCCESS peer:%s port:%u id:%s\n", addr.ip, addr.port, addr.id);
	}
    LOG(LOG_NOTIFY, "HANDSHAKE FAILED peer:%s port:%u id:%s\n", addr.ip, addr.port, addr.id);
	return status;
}

bool SeederManager::recvHandshake(int leecherSock) {
	char buf[HANDESHAKE_SIZE];
	ssize_t t;
	bzero(buf, HANDESHAKE_SIZE);
	t = read(leecherSock, buf, HANDESHAKE_SIZE);
	if (t < 0) {
		printMSG("Failed to handshake with %s!\n", getIdfromMap(leecherSock).c_str());
		return false;
	}
	if (t > 0) {
		// now mark leecherSock as handshaked
		this->handshaked[leecherSock] = true;
		printMSG("Recv handshake msg from leecher " + getIdfromMap(leecherSock) + " ... OK\n");
		// verify the handshake message, if failed, return false
		bool integ = integrityVerify(args->bt_info, sizeof(bt_info_t) - 10, buf + 20 + 8);
		if (integ) {
			printMSG("Verifying handshake msg from leecher " + getIdfromMap(leecherSock) + " ... OK\n");
		} else {
			printMSG("Verifying handshake msg from leecher " + getIdfromMap(leecherSock) + " ... Failed!\n");
			return false;
		}
	}
	return true;
}


// 
// in our case, a seeder will always have the complete file
// thus bitfield is actually useless
// but we still implement it
bool SeederManager::sendBitfield(int sock) {
	char buf[MAX_BUF_SZIE];
	int len;
	if (! createBitfield(buf, len) ) {
		printMSG("Failed to create bitfield!");
		return false;
	}
	printMSG("Creating bitfield for " + getIdfromMap(sock) + " ... OK");
	if (! sendData(sock, buf, len)) {
		printMSG("Failed to send bitfield to " + getIdfromMap(sock) + "!");
        return false;
	}
	printMSG("Sending bitfield to " + getIdfromMap(sock) + " ... OK");
	return true;
}

bool SeederManager::createBitfield(char *buf, int &len) {
	bt_msg_t *msg = (bt_msg_t *) buf;
	bzero(buf, MAX_BUF_SZIE);
	len = sizeof(bt_msg_t);
	msg->length = len - sizeof(int);
	msg->bt_type = (unsigned char) 5;
	msg->payload.bitfield.size = this->args->bt_info->num_pieces;
	// fill the bitfield somehow
	// TODO
	return true;
}


bool SeederManager::processSock(int sock) {
	char buf[MAX_BUF_SZIE];
	ssize_t len;
	int status = readMSG(sock, buf, len);
	if (status < 0) { // error happened
        Close(sock);
        return false;
	}
	else if (status == 0) { // this sock is disconnected
		Close(sock);
		return true;
	}

	bt_msg_t *msg = (bt_msg_t *) buf;

	// process different types of msg
	printMSG("\nProcessing msg with msg_type [%d] from %s ...\n", (int) (msg->bt_type), getIdfromMap(sock).c_str());
	switch (msg->bt_type) {
		case 6: // msg:request
			//send piece
			bt_request_t request = msg->payload.request;
			char buf[MAX_BUF_SZIE];
			// calculate the offset
			long offset = request.index * args->bt_info->piece_length + request.begin;
            ip_sock_id addr;
            if (!getAddrfromMap(sock, addr)){
                printMSG("Get leecher info failed!\n");
                return false;
            }
            LOG(LOG_NOTIFY, "MESSAGE REQUEST FROM peer_id:%s piece:%ld offset:%ld length:%ld\n", addr.id, request.index, offset, args->bt_info->length);
			createPieceMsg(this->args->f_save, buf, len, offset, request); 
			if (sendData(sock, buf, len)) {
				printMSG("Sending piece [%d bytes] to leecher %s ... OK\n", len, getIdfromMap(sock).c_str());
                LOG(LOG_NOTIFY, "MESSAGE PIECE TO peer_id:%s piece:%ld offset:%ld length:%ld\n", addr.id, request.index, offset, args->bt_info->length);
			}
            //output minimum info: download status, number of connected peers, the amount of downloaded and uploaded
            //snprintf(message, FILE_NAME_MAX, "File: %s Progress: %.1f%% Peers: %d Uploaded: %ld KB\n", args->bt_info->name, (double)((offset + len) * 100 / args->bt_info->length), this->n_sockets, (offset + len) / 1024);
            m_ullUploaded += len;
            snprintf(message, FILE_NAME_MAX, "File: %s Peers: %d Uploaded: %lld KB\n", args->bt_info->name, this->n_sockets, m_ullUploaded / 1024);
            fprintf(stdout, "%s", message);
			break;
		//case 4: //msg:have
			//as a seeder always a seeder, do nothing here
			//break;
	}
	printMSG("Processing msg with msg_type [%d] from %s ... OK\n", (int) (msg->bt_type), getIdfromMap(sock).c_str());
	return true;
}


//
//  + release all memory
//  + close all handles
SeederManager::~SeederManager() {
	releaseInfo(this->args->bt_info);
	fclose(this->args->f_save);
}

LeecherManager::LeecherManager(bt_args_t *btArg){
	// set # of sockets as 0
	n_sockets = 0;
	args = btArg;
	// set downloaded all 0
	memset(downloaded, 0, MAX_PIECES_NUM);
	n_downloaded = 0;

	// open a save_file to save our data
	if ( (args->f_save = fopen(args->save_file, "wb")) == NULL) {
        snprintf(message, FILE_NAME_MAX, "Can not create save file %s", args->save_file);
        throw message;
	};
	FILE *tmp = args->f_save;
	// reserve the file size as large as the total download size
	// use tmp, do not modify f_save directly
	fseek(tmp, args->bt_info->length, SEEK_SET);
}

bool LeecherManager::connectSeeders() {
	for (int i = 0; i < MAX_CONNECTIONS; ++i) 
		if (args->peers[i] != NULL) {
			peer_t *peerP = args->peers[i];
            connectSeeder(peerP->sockaddr);
		}
	return true;
}

bool LeecherManager::connectSeeder(struct sockaddr_in seederAddr) {
	bool breturn = false;
	// create socket
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		//std::cerr << "Failed to create socket" << std::endl;
		throw "Failed to create socket!";
	}
    
    struct timeval timeout;
    timeout.tv_sec = SOCKETT_TIMEOUT;//socket timeout
    timeout.tv_usec = 0;
    if (setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0){
        if (-1 != sockfd)
            close(sockfd);
    }
    if (setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0){
        if (-1 != sockfd)
            close(sockfd);
    }
    if (make_socket_non_blocking(sockfd) < 0){
        if (-1 != sockfd)
            close(sockfd);
    }
	// connect to a seeder
	int status = 0;
	while (true){
		status = connect(sockfd, (struct sockaddr *)&seederAddr, sizeof(seederAddr));
		if (status < 0) {
			if (EINPROGRESS != errno){
				if (-1 != sockfd)
					Close(sockfd);
                snprintf(message, FILE_NAME_MAX, "Failed to connect to the seeder %s!\n", getIdfromMap(sockfd).c_str());
                printMSG(message);
				//std::cerr << "Failed to connect to the seeder XXX!" << std::endl;
				//throw message;
				break;
			}
		}
		else{
			//printMSG("Connecting to the seeder " + getIdfromMap(sockfd) + "... OK\n");
			breturn = true;
            //only successfully connected
            //store seeder information
            ip_sock_id addr;
            memcpy(addr.id, getIdfromPeer(inet_ntoa(seederAddr.sin_addr), ntohs(seederAddr.sin_port)).c_str(), ID_SIZE*2);
            memcpy(addr.ip, inet_ntoa(seederAddr.sin_addr), MAX_IP);
            addr.port = ntohs(seederAddr.sin_port);
            m_ipsockidMap.insert(std::pair<int, ip_sock_id>(sockfd, addr));
            //add socket into the array of sockets
            //n_sockets++;
            sockets[n_sockets++] = sockfd;
			break;
		}
		sleep(1);
	}
	return breturn;
}

bool LeecherManager::getAddrfromMap(int socket, ip_sock_id &addr){
    std::map<int, ip_sock_id>::iterator it = m_ipsockidMap.find(socket);
    if (it != m_ipsockidMap.end()){
        addr = it->second;
        return true;
    }
    else{
        return false;
    }
}

std::string LeecherManager::getIdfromMap(int socket){
    std::map<int, ip_sock_id>::iterator it = m_ipsockidMap.find(socket);
    if (it != m_ipsockidMap.end()){
        return it->second.id;
    }
    else{
        return "";
    }
}

bool LeecherManager::Close(int socket){
    n_sockets--;
    std::map<int, ip_sock_id>::iterator it = m_ipsockidMap.find(socket);
    if (it != m_ipsockidMap.end()){
        m_ipsockidMap.erase(socket);
    }
    if (-1 != socket)
        close(socket);
    return true;
}

bool LeecherManager::recvHandshake(int sockfd) {
	char buf[HANDESHAKE_SIZE];
	ssize_t t;
	bzero(buf, HANDESHAKE_SIZE);
    ip_sock_id addr;
    if (!getAddrfromMap(sockfd, addr)){
        printMSG("Get leecher info failed!\n");
        return false;
    }
    //log the handshake
    LOG(LOG_NOTIFY, "HANDSHAKE RECV peer:%s port:%u id:%s\n", addr.ip, addr.port, addr.id);
	t = read(sockfd, buf, HANDESHAKE_SIZE);
	if (t < 0) {
		printMSG("Failed to handshake!");
		return false;
	}
	if (t > 0)
		printMSG("Recv handshake msg from " + getIdfromMap(sockfd) + " ... OK");
	// verify the handshake message, if failed, return false
	bool integ = integrityVerify(args->bt_info, sizeof(bt_info_t) - 10, buf + 20 + 8);
	if (integ) {
        LOG(LOG_NOTIFY, "HANDSHAKE INTEGRITY SUCCESS peer:%s port:%u id:%s\n", addr.ip, addr.port, addr.id);
		printMSG("Verifying handshake msg from seeder " + getIdfromMap(sockfd) + " ... OK\n");
	} else {
        LOG(LOG_NOTIFY, "HANDSHAKE INTEGRITY FAILED peer:%s port:%u id:%s\n", addr.ip, addr.port, addr.id);
		printMSG("Verifying handshake msg from seeder " + getIdfromMap(sockfd) + " ... Failed!\n");
		return false;
	}
	return true;

}

bool LeecherManager::sendHandshake(int sockfd) {
	char buf[MAX_BUF_SZIE];
	bzero(buf, MAX_BUF_SZIE);
	createHandshakeMsg(buf, args->bt_info, args->id);
	bool status = sendData(sockfd, buf, HANDESHAKE_SIZE);
	if (status) {
		printMSG("Response handshake msg from seeder " + getIdfromMap(sockfd) + " ... OK\n");
	}
	return status;
}


bool LeecherManager::createRequest(char *buf, ssize_t &len, long index, long begin, long length) {
	bt_msg_t *msg = (bt_msg_t *) buf;
	bt_request_t request;
	long file_size = args->bt_info->length;
	long offset = index * args->bt_info->piece_length + begin;
	if (length > file_size - offset) {
		length = file_size - offset;
	}
	request.index = index;
	request.begin = begin;
	request.length = length;

	msg->payload.request = request;
	// add other field
	msg->bt_type = (unsigned char) 6;

	msg->length = sizeof(bt_msg_t) - sizeof(int);
	printMSG("Creating request (index %d, begin %d, length %d)\n", index, begin, length);
	len = sizeof(bt_msg_t);
	return true;
}


bool LeecherManager::sendRequest(int sock, long index, long begin, long length) {
	char buf[MAX_BUF_SZIE];
	ssize_t len;
	createRequest(buf, len, index, begin, length);
	if (!sendData(sock, buf, len)) {
		printMSG("Failed to send request data to seeder " + getIdfromMap(sock) + "!");
        return false;
	}
	return true;
}


bool LeecherManager::sendRequest(int sock) {
	long index = pickNeedPiece(downloaded, this->args->bt_info->num_pieces);
	if (index < 0) {
		// download complete
		// take some action
		return false; // no request sent, all pieces are either completed or in progress
	}
	else { // some valid indx
		this->downloaded[index] = 1; // set as in progress
		if (this->sendRequest(sock, index, 0, this->args->bt_info->piece_length)) {
			printMSG("Send request with (index %d, begin %d, length %d) to %s ... OK\n",
					index, 0, this->args->bt_info->piece_length, getIdfromMap(sock).c_str());
            ip_sock_id addr;
            if (!getAddrfromMap(sock, addr)){
                printMSG("Get leecher info failed!\n");
                return false;
            }
            LOG(LOG_NOTIFY, "MESSAGE REQUEST TO peer:%s port:%u id:%s\n", addr.ip, addr.port, addr.id);
			return true;
		} else {return false;}
	}

}

bool LeecherManager::isDownloadComplete() {
	return (n_downloaded == args->bt_info->num_pieces);
}


bool LeecherManager::processSock(int sock) {
	char buf[MAX_BUF_SZIE];
	ssize_t len;
	if (readMSG(sock, buf, len) < 0) {
		printMSG("Failed to read msg from seeder " + getIdfromMap(sock) + "!");
        return false;
	}
	bt_msg_t *msg = (bt_msg_t *) buf;
	printMSG("\nProcessing msg with msg_type [%d] from %s ... \n", (int) (msg->bt_type), getIdfromMap(sock).c_str());
	switch (msg->bt_type) { // process diff types

		case 5: // recv  a bitfield
			// need to send a request
			//puts("bitfield recv!\n");
			if (!this->isDownloadComplete())
				this->sendRequest(sock);
			break; 

		case 7: // recieve a piece
			bt_piece_t piece = msg->payload.piece; // assign the piece
			// write to file based on the info in piece
			this->downloaded[piece.index] = 2; // set this piece as download
			n_downloaded++; // ++ # of downloaded pieces
            long offset = piece.index * args->bt_info->piece_length + piece.begin;
			long piece_length = len - sizeof(bt_msg_t); // calc the size of the piece
			bool integ = integrityVerify(buf + sizeof(bt_msg_t), piece_length, 
					args->bt_info->piece_hashes[piece.index]);
			if (!integ) {
                printMSG("Recieved broken piece from %s at index [%ld]", getIdfromMap(sock).c_str(), piece.index);
				// resend
				this->downloaded[piece.index] = 0; // set from 1 [in progress] to 2 [in need]
				this->sendRequest(sock); // randomly resend a request
				break;
			} else {
                printMSG("Verifying recieved piece from %s at index [%ld]\n", getIdfromMap(sock).c_str(), piece.index);
			}
            ip_sock_id addr;
            if (!getAddrfromMap(sock, addr)){
                printMSG("Get leecher info failed!\n");
                return false;
            }
            LOG(LOG_NOTIFY, "MESSAGE PIECE FROM peer:%s port:%u id:%s\n", addr.ip, addr.port, addr.id);
			// save this piece to file
			if (saveToFile(args->f_save, buf + sizeof(bt_msg_t), offset, piece_length) > 0) { // save to file
				printMSG("Piece %ld from %s, offset [%ld], length [%ld] downloaded and saved to file!\n",
                        piece.index, getIdfromMap(sock).c_str(), offset, piece_length);
			} else {
				printMSG("Failed to save piece %ld:  offset [%ld], length [%ld]!\n", piece.index, offset, piece_length);
			}
            
            //output minimum info: download status, number of connected peers, the amount of downloaded and uploaded
            snprintf(message, FILE_NAME_MAX, "File: %s Progress: %.1f%% Peers: %d Downloaded: %ld KB\n", args->bt_info->name, (double)((offset + piece_length) * 100 / args->bt_info->length), this->n_sockets, (offset + piece_length) / 1024);
            fprintf(stdout, "%s", message);
			// if not complete send another request
			if (!this->isDownloadComplete())
				this->sendRequest(sock);
			break;
	} // switch
    printMSG("Msg with msg_type [%d] from %s ... processed!\n", (int) (msg->bt_type), getIdfromMap(sock).c_str());
	return true;
}

LeecherManager::~LeecherManager() {
    if (NULL != args->f_save){
        fclose(args->f_save); // close f_save handle
        args->f_save = NULL;
    }
	for (int i = 0; i < this->n_sockets; ++i) { // close all connect sockets
		Close(this->sockets[i]); 
	}
    n_sockets = 0;
}


bool sendData(int seederSock, char *buf, ssize_t n_bytes) {
	ssize_t sent_size = write(seederSock, buf, n_bytes);
	char *tmp = buf;
	while (sent_size > 0 && n_bytes > sent_size) { // not yet finish the sending
		tmp += sent_size;
		n_bytes -= sent_size; // remaining size
		sent_size = write(seederSock, tmp, n_bytes);
	}
	if (sent_size < 0) {
		return false;
	}

	return true;
}


bool createHandshakeMsg(char *buf, bt_info_t *info,  char *id) {
	// clean buf
	memset(buf, 0, HANDESHAKE_SIZE);
	buf[0] = (char) 19;
	strcpy(buf + 1, "BitTorrent Protocol");
	unsigned char hash[ID_SIZE + 5];
	bzero(hash, ID_SIZE);
	// the reason to -10 is that info->piece_hashes will not keep same
	SHA1((unsigned char *) info, sizeof(bt_info_t) - 10, hash);
	strncpy(buf + 20 + 8, (char *) hash, ID_SIZE);
	strncpy(buf + 20 +8 + 20, id, ID_SIZE);
	return true;
}

int readMSG(int sock, char *buf, ssize_t &len) {
	char *tmp = buf;
	len = 0;
	ssize_t t;
	bzero(buf, MAX_BUF_SZIE);
	while (true) {
		t = read(sock, tmp, 100);
		if (t <= 0) { // no further data available
			break;
		}
		else {
			len += t;
			tmp += t;
		}
	}

	bt_msg_t *msg = (bt_msg_t *) buf;
	long need_to_read = msg->length + sizeof(int);
	if (msg->length == 0) need_to_read = 0;
	if (len != need_to_read && msg->length != 0) {
        printMSG("%ld bytes read, %ld bytes needed!\n", len, need_to_read);
		//exit(1);
	}
	//  std::cerr << len << " bytes read,  " << need_to_read << " bytes needed!" << std::endl;
	return (int) msg->bt_type;
}


long pickNeedPiece(int *indexes, long len) {
	for (long i = 0; i < len; ++i)
		if (indexes[i] == 0) return i;
	return -1;
}


bool createPieceMsg(FILE *fp, char *buf, ssize_t &len, long offset,  bt_request_t request) {
	if (fp == NULL) {
        printMSG("File can not open when create the piece msg!");
        return false;
	}
	// do not directly operate on fp, 
	// fp should not be modified
	FILE *tmp = fp;

	// construct msg
	bt_msg_t *msg =  (bt_msg_t *) buf;
	msg->length = sizeof(bt_msg_t) + request.length - sizeof(int);
	msg->bt_type = (unsigned char) 7;
	bt_piece_t *piece = &(msg->payload.piece); // point to payload
	piece->index = request.index;
	piece->begin = request.begin;

	printMSG("Reading - offset [%d] - length [%d] \n", offset, request.length);
	fseek(tmp, offset, SEEK_SET);
	len = fread(buf + sizeof(bt_msg_t), 1, request.length, tmp);
	if (len != request.length) {
		printMSG("Reading error!");
        return false;
	}
	len = request.length + sizeof(bt_msg_t);
	return true;
}


ssize_t saveToFile(FILE *fp, char *src, long offset, long length) {
	if (fp == NULL) {
		printMSG("Invalid file!");
		return -1;
	}
	FILE *tmp = fp;
	fseek(tmp, offset, SEEK_SET);
	ssize_t t = fwrite(src, 1, length, tmp);
	if (t != length) {
		return -1;
	}
	return t;
}


bool integrityVerify(void *buf, long length, char *hash) {
	unsigned char hash_buf[ID_SIZE + 2];
	bzero(hash_buf, ID_SIZE + 2);
	SHA1((unsigned char *) buf, length, hash_buf);
	printMSG("Verifying data integrity...  OK\n");
	return (memcmp(hash_buf, hash, ID_SIZE) == 0);
}