//
//  main.cpp
//  wiretap
//
//  Created by Tony Liu on 10/26/14.
//  Copyright (c) 2014 Tony Liu. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <map>
#include <netinet/if_ether.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
//#include <net/ethernet.h>
//#include <netinet/ether.h>
//#include <linux/if_ether.h>
#include <pcap.h>
#include <pcap/bpf.h>
#include <pcap/pcap.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>

#define MAX_FILE_NAME 1024
#define IP_HDR_SIZE 14
#define ARP_HDR_SIZE 14

typedef std::map<u_char*, int> STRU_STAT;
typedef STRU_STAT::iterator IT_STRU_STAT;

//=== summary ===
int iFirstPacketFlag = 0;
timeval firstPacketTime;//first packet capture time
int iLastPacketFlag = 0;
timeval lastPacketTime;//last packet capture time
int iLargestPacket = 0;//largest packet size
int iSmallestPacket = 0;//smallest packet size
long lPktNumber = 0;//total packet number
unsigned long long ullTotalPktSize = 0;//total packet size

//=== link layer ===
STRU_STAT sourceEthernet;
STRU_STAT destEthernet;

//=== network layer ===
STRU_STAT networkProtocols;
//=== network layer:ip ===
STRU_STAT sourceIp;
STRU_STAT destIp;
//=== network layer:arp ===
STRU_STAT arpParticipants;

//=== transport layer ===
STRU_STAT transportProtocols;
//=== transport layer:tcp ===
STRU_STAT sourceTCPPorts;
STRU_STAT destTCPPorts;
STRU_STAT TCPFlags;
STRU_STAT TCPOptions;
//=== transport layer:udp ===
STRU_STAT sourceUDPPorts;
STRU_STAT destUDPPorts;
//=== transport layer:icmp ===
STRU_STAT icmpTypes;
STRU_STAT icmpCodes;

//show usage
void usage(FILE * file);
//parse args
std::string parse_args(int argc,  char * argv[]);
//init tcpflags, set every flag's number be 0
void initTCPFlags();
//print a stat map
void printStat(STRU_STAT stat);
//print all the statistics
void printStat();
//compare two unsigned char string, return 0 if same
int compare(unsigned char *a, unsigned char *b, ssize_t size);
//destory all struct
void destory();
//destroy all structs
void destroyStruct(STRU_STAT stat);
//check ip exist or not
void checkIpExsit(STRU_STAT &statIp, u_char * Ip);
//analyze each packet
void callback(u_char *arg_array, const struct pcap_pkthdr *h, const u_char *bytes);

int main (int argc, char **argv) {
	//parse cmd
	std::string pcap_file_name = parse_args(argc, argv);
	if (pcap_file_name.empty()){
		usage(stdout);
		exit(EXIT_FAILURE);
	}
	if (std::string::npos == pcap_file_name.find(".pcap")){
		std::cout << "Not a valid tcpdump capture file!\n";
		exit(EXIT_FAILURE);
	}
	//open pcap file
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t* pcap = pcap_open_offline(pcap_file_name.c_str(), errbuf);
	if (NULL == pcap){
		std::cout << "pcap file open failed:\n" << errbuf << std::endl;;
		exit(EXIT_FAILURE);
	}
	//pcap_file_header fd;
	//check it's from ethernet
	if (DLT_EN10MB != pcap_datalink(pcap)){
		std::cout << "pcap file was not saved from Ethernet!" << std::endl;;
		exit(EXIT_FAILURE);
	}
    //pcap_pkthdr * pcap_pktheader;
	//get the stat info from the pcap file
	pcap_stat * pcapstat;
	if (-1 == pcap_stats(pcap, pcapstat)){
	    std::cout << "pcap file doesn’t support packet statistics!" << std::endl;;
	}
    else
        lPktNumber = pcapstat->ps_recv;

    initTCPFlags();
	pcap_loop(pcap, 0, callback, NULL);
	pcap_close(pcap);
	//std::cout<< "pcap file parse finished!\n";
	printStat();
	exit(EXIT_SUCCESS);
}
void initTCPFlags(){
    u_char msg[5] = "\0";
    memset(msg, 0x00, 5);
    memcpy(msg, "FIN", sizeof("FIN"));
    checkIpExsit(TCPFlags, msg);
    memset(msg, 0x00, 5);
    memcpy(msg, "ACK", sizeof("ACK"));
    checkIpExsit(TCPFlags, msg);
    memset(msg, 0x00, 5);
    memcpy(msg, "SYN", sizeof("SYN"));
    checkIpExsit(TCPFlags, msg);
    memset(msg, 0x00, 5);
    memcpy(msg, "RST", sizeof("RST"));
    checkIpExsit(TCPFlags, msg);
    memset(msg, 0x00, 5);
    memcpy(msg, "PUSH", sizeof("PUSH"));
    checkIpExsit(TCPFlags, msg);
    memset(msg, 0x00, 5);
    memcpy(msg, "URG", sizeof("URG"));
    checkIpExsit(TCPFlags, msg);
    return;
}
//parse every packet
void callback(u_char *arg_array, const struct pcap_pkthdr *h, const u_char *bytes){
	lastPacketTime = h->ts;
	bpf_u_int32 length = h->len;
	if (0 == iFirstPacketFlag){
		//get the first packet's time as the start time to capture packets
		firstPacketTime = h->ts;
		iFirstPacketFlag = 1;
	}
	//check it's the largest or smallest
    if (0 == lPktNumber)
        iLargestPacket = iSmallestPacket = length;
    if (length > iLargestPacket){
        iLargestPacket = length;
    }
    else if (length < iSmallestPacket){
		iSmallestPacket = length;
    }
	lPktNumber++;
	ullTotalPktSize += length;

	ether_header * ether = (ether_header *)bytes;
	u_char msg[MAX_FILE_NAME] =  "\0";
	char msg2[MAX_FILE_NAME] = "\0";
	/*
	   ether_addr etheraddr;
	   memcpy(etheraddr.ether_addr_octet, ether->ether_shost, ETHER_ADDR_LEN*sizeof(u_char));
	   memset(msg, 0x00, MAX_FILE_NAME);
	   memcpy(msg, ether_ntoa(&etheraddr), strlen(ether_ntoa(&etheraddr)));
	   checkIpExsit(sourceEthernet, msg);
	//std::cout << msg << std::endl;

	memcpy(etheraddr.ether_addr_octet, ether->ether_dhost, ETHER_ADDR_LEN*sizeof(u_char));
	memset(msg, 0x00, MAX_FILE_NAME);
	memcpy(msg, ether_ntoa(&etheraddr), strlen(ether_ntoa(&etheraddr)));
	checkIpExsit(destEthernet, msg);
	 */
	//datalink layer record its source and destination
	memset(msg2, 0x00, MAX_FILE_NAME);
	for (int i = 0; i < ETHER_ADDR_LEN - 1; i++) {
		sprintf(msg2 + i * 3, "%02x:", ether->ether_shost[i]);
	}
	sprintf(msg2 + ETHER_ADDR_LEN * 3 - 3, "%02x", ether->ether_shost[ETHER_ADDR_LEN - 1]);
	memset(msg, 0x00, MAX_FILE_NAME);
	memcpy(msg, msg2, strlen(msg2));
	checkIpExsit(sourceEthernet, msg);

	memset(msg2, 0x00, MAX_FILE_NAME);
	for (int i = 0; i < ETHER_ADDR_LEN - 1; i++) {
		sprintf(msg2 + i * 3, "%02x:", ether->ether_dhost[i]);
	}
	sprintf(msg2 + ETHER_ADDR_LEN * 3 - 3, "%02x", ether->ether_dhost[ETHER_ADDR_LEN - 1]);
	memset(msg, 0x00, MAX_FILE_NAME);
	memcpy(msg, msg2, strlen(msg2));
	checkIpExsit(destEthernet, msg);


	//if (ETH_P_IP == ntohs(e->h_proto)){
	if (ETHERTYPE_IP == ntohs(ether->ether_type)){
		//ip
		memset(msg, 0x00, MAX_FILE_NAME);
		memcpy(msg, "IP", sizeof("IP"));
		checkIpExsit(networkProtocols, msg);
		//network layer
		struct ip *iphdr = (struct ip*)(bytes + ETHER_HDR_LEN);
		struct in_addr addr;
		addr = iphdr->ip_src;
		memset(msg, 0x00, MAX_FILE_NAME);
		memcpy(msg, inet_ntoa(addr), strlen(inet_ntoa(addr)));
		checkIpExsit(sourceIp, msg);
		addr = iphdr->ip_dst;
		memset(msg, 0x00, MAX_FILE_NAME);
		memcpy(msg, inet_ntoa(addr), strlen(inet_ntoa(addr)));
		checkIpExsit(destIp, msg);
		//transport layer
		if (IPPROTO_TCP == iphdr->ip_p){
			memset(msg, 0x00, MAX_FILE_NAME);
			memcpy(msg, "TCP", sizeof("TCP"));
			checkIpExsit(transportProtocols, msg);
			//tcp source ports
			tcphdr * tcp = (tcphdr *)(bytes + ETHER_HDR_LEN + sizeof(ip));
			memset(msg, 0x00, MAX_FILE_NAME);
            sprintf(msg2, "%u", ntohs(tcp->source));
			//sprintf(msg2, "%u", tcp->th_sport);
			memset(msg, 0x00, MAX_FILE_NAME);
			memcpy(msg, msg2, strlen(msg2));
			checkIpExsit(sourceTCPPorts, msg);
            //tcp dest ports
			memset(msg, 0x00, MAX_FILE_NAME);
            sprintf(msg2, "%u", ntohs(tcp->dest));
			//sprintf(msg2, "%u", tcp->th_dport);
			memset(msg, 0x00, MAX_FILE_NAME);
			memcpy(msg, msg2, strlen(msg2));
			checkIpExsit(destTCPPorts, msg);
			memset(msg, 0x00, MAX_FILE_NAME);
            
            //TODO flag is not correct tcp flags
            if (1 == tcp->fin)
                checkIpExsit(TCPFlags, (u_char*)"FIN");
            else if (1 == tcp->syn)
                checkIpExsit(TCPFlags, (u_char*)"SYN");
            else if (1 == tcp->rst)
                checkIpExsit(TCPFlags, (u_char*)"RST");
            else if (1 == tcp->psh)
                checkIpExsit(TCPFlags, (u_char*)"PUSH");
            else if (1 == tcp->ack)
                checkIpExsit(TCPFlags, (u_char*)"ACK");
            else if (1 == tcp->urg)
                checkIpExsit(TCPFlags, (u_char*)"URG");
            /*
			if ((tcp->th_flags & TH_FIN) == TH_FIN)
				checkIpExsit(TCPFlags, (u_char*)"FIN");
			else if ((tcp->th_flags & TH_SYN) == TH_SYN)
				checkIpExsit(TCPFlags, (u_char*)"SYN");
			else if ((tcp->th_flags & TH_RST) == TH_RST)
				checkIpExsit(TCPFlags, (u_char*)"RST");
			else if ((tcp->th_flags & TH_PUSH) == TH_PUSH)
				checkIpExsit(TCPFlags, (u_char*)"PUSH");
			else if ((tcp->th_flags & TH_ACK) == TH_ACK)
				checkIpExsit(TCPFlags, (u_char*)"ACK");
			else if ((tcp->th_flags & TH_URG) == TH_URG)
				checkIpExsit(TCPFlags, (u_char*)"URG");
			else if ((tcp->th_flags & TH_ECE) == TH_ECE)
				checkIpExsit(TCPFlags, (u_char*)"ECE");
			else if ((tcp->th_flags & TH_CWR) == TH_CWR)
				checkIpExsit(TCPFlags, (u_char*)"CWR");
			else
				std::cout << "error tcp flags:" << tcp->th_flags << std::endl;
            */
            
            // TODO tcp options
            
		}
		else if (IPPROTO_UDP == iphdr->ip_p){
			memset(msg, 0x00, MAX_FILE_NAME);
			memcpy(msg, "UDP", sizeof("UDP"));
			checkIpExsit(transportProtocols, msg);
            //tcp source ports
            udphdr * udp = (udphdr *)(bytes + ETHER_HDR_LEN + sizeof(ip));
            memset(msg, 0x00, MAX_FILE_NAME);
            sprintf(msg2, "%u", ntohs(udp->source));
            //sprintf(msg2, "%u", udp->uh_sport);
            memset(msg, 0x00, MAX_FILE_NAME);
            memcpy(msg, msg2, strlen(msg2));
            checkIpExsit(sourceUDPPorts, msg);
            //tcp dest ports
            memset(msg, 0x00, MAX_FILE_NAME);
            sprintf(msg2, "%u", ntohs(udp->dest));
            //sprintf(msg2, "%u", udp->uh_dport);
            memset(msg, 0x00, MAX_FILE_NAME);
            memcpy(msg, msg2, strlen(msg2));
            checkIpExsit(destUDPPorts, msg);
            memset(msg, 0x00, MAX_FILE_NAME);
		}
		else if (IPPROTO_ICMP == iphdr->ip_p){
			memset(msg, 0x00, MAX_FILE_NAME);
			memcpy(msg, "ICMP", sizeof("ICMP"));
			checkIpExsit(transportProtocols, msg);
            icmp * icmphdr = (icmp *)(bytes + ETHER_HDR_LEN + sizeof(ip));
            memset(msg2, 0x00, MAX_FILE_NAME);
            sprintf(msg2, "%u", icmphdr->icmp_type);
            memset(msg, 0x00, MAX_FILE_NAME);
            memcpy(msg, msg2, strlen(msg2));
            checkIpExsit(icmpTypes, msg);
            memset(msg2, 0x00, MAX_FILE_NAME);
            sprintf(msg2, "%u", icmphdr->icmp_code);
            memset(msg, 0x00, MAX_FILE_NAME);
            memcpy(msg, msg2, strlen(msg2));
            checkIpExsit(icmpCodes, msg);
		}
		else{
			memset(msg2, 0x00, MAX_FILE_NAME);
			sprintf(msg2, "%02x", ntohs(iphdr->ip_p));
			memset(msg, 0x00, MAX_FILE_NAME);
			memcpy(msg, msg2, strlen(msg2));
			//memcpy(msg, ntohs(e->h_proto), strlen(ntohs(e->h_proto)));
			checkIpExsit(transportProtocols, msg);
		}
	}
	//else if (ETH_P_ARP == ntohs(e->h_proto)){
	else if (ETHERTYPE_ARP == ntohs(ether->ether_type)){
		//arp
		memset(msg, 0x00, MAX_FILE_NAME);
		memcpy(msg, "ARP", sizeof("ARP"));
		checkIpExsit(networkProtocols, msg);
		//network layer
		struct ether_arp * earp = (struct ether_arp*)(bytes + ETHER_HDR_LEN);
		memset(msg2, 0x00, MAX_FILE_NAME);
		for (int i = 0; i < ETHER_ADDR_LEN - 1; i++) {
			sprintf(msg2 + i * 3, "%02x:", earp->arp_sha[i]);
		}
		sprintf(msg2 + ETHER_ADDR_LEN * 3 - 3, "%02x/", earp->arp_sha[ETHER_ADDR_LEN - 1]);
		//get ip
		sprintf(msg2 + ETHER_ADDR_LEN * 3, "%u.%u.%u.%u", earp->arp_spa[0], earp->arp_spa[1], earp->arp_spa[2], earp->arp_spa[3]);
		//put mac/ip together
		memset(msg, 0x00, MAX_FILE_NAME);
		memcpy(msg, msg2, strlen(msg2));
		checkIpExsit(arpParticipants, msg);
	}
	else{
		memset(msg2, 0x00, MAX_FILE_NAME);
		sprintf(msg2, "%d (0x%02x)", ntohs(ether->ether_type), ntohs(ether->ether_type));
		memset(msg, 0x00, MAX_FILE_NAME);
		memcpy(msg, msg2, strlen(msg2));
		checkIpExsit(networkProtocols, msg);
	}
	return;
}
void destory(){
	destroyStruct(sourceEthernet);
	destroyStruct(destEthernet);
	destroyStruct(networkProtocols);
	destroyStruct(sourceIp);
	destroyStruct(destIp);
	destroyStruct(arpParticipants);
	destroyStruct(transportProtocols);
	destroyStruct(sourceTCPPorts);
	destroyStruct(destTCPPorts);
	destroyStruct(TCPFlags);
	destroyStruct(TCPOptions);
	destroyStruct(sourceUDPPorts);
	destroyStruct(destUDPPorts);
	destroyStruct(icmpTypes);
	destroyStruct(icmpCodes);
}
void destroyStruct(STRU_STAT stat){
	IT_STRU_STAT it = stat.begin();
	for (; it != stat.end(); it++){
		free(it->first);
	}
	stat.clear();
	return;
}
void checkIpExsit(STRU_STAT &statIp, u_char * Ip){
	//int iflag = 0;
	IT_STRU_STAT it = statIp.begin();
	for (; it != statIp.end(); it++){
		if (0 == compare(Ip, it->first, strlen((char*)Ip))){
			//find the same one
            //iflag = 1;
			break;
		}
	}
	if (it != statIp.end()){
		it->second++;
	}
	else{
		u_char *msg =  (u_char *)malloc(sizeof(u_char *) * MAX_FILE_NAME);
		memset(msg, 0x00, MAX_FILE_NAME);
		memcpy(msg, Ip, strlen((char*)Ip));
        if (0 == strcmp(msg,"URG")) std::cout << msg << std::endl;
		statIp.insert(std::pair<u_char*, int>(msg, 1));
	}
	return;
}
int compare(unsigned char *a, unsigned char *b, ssize_t size) {
	while(size-- > 0) {
		if ( *a != *b ) { return (*a < *b ) ? -1 : 1; }
		a++; b++;
	}
	return 0;
}

void printStat(){
	IT_STRU_STAT it;
    time_t t = (time_t)firstPacketTime.tv_sec;
    tm capturetime;
    char buf[80] = "\0";
    strftime(buf,80,"%F %H:%M:%S %Z",localtime_r(&t, &capturetime));
	std::cout << "=== Packet capture summary ===" << std::endl << std::endl <<
		"Start date:" << buf <<std::endl <<
		"Duration:" << lastPacketTime.tv_sec - firstPacketTime.tv_sec << " seconds" <<std::endl <<
		"# Packets:" << lPktNumber << std::endl <<
		"Smallest:" << iSmallestPacket << std::endl <<
		"Largest:" << iLargestPacket << std::endl <<
    "Average:" << std::setprecision(4) << ullTotalPktSize / (double)lPktNumber << std::endl << std::endl;

	std::cout << "=== Link layer ===" << std::endl << std::endl;
	std::cout << "--- Source ethernet addresses ---" << std::endl << std::endl;
    printStat(sourceEthernet);

	std::cout << std::endl << std::endl << "--- Destination ethernet addresses ---" << std::endl << std::endl;
    printStat(destEthernet);
    
	std::cout << std::endl << "=== Network layer ===" << std::endl << std::endl;
	std::cout <<"--- Network layer protocols ---" << std::endl;
    printStat(networkProtocols);

	std::cout << std::endl << "--- Source IP addresses ---" << std::endl << std::endl;
    printStat(sourceIp);

	std::cout << std::endl << "--- Destination IP addresses ---" << std::endl <<std::endl;
    printStat(destIp);

	std::cout << std::endl << "--- Unique ARP participants ---" << std::endl <<std::endl;
    printStat(arpParticipants);

	std::cout << std::endl << "=== Transport layer ===" << std::endl << std::endl;
	std::cout <<"--- Transport layer protocols ---" << std::endl << std::endl;
    printStat(transportProtocols);

	std::cout << std::endl << "=== Transport layer: TCP ===" << std::endl << std::endl;
	std::cout << "--- Source TCP ports ---" << std::endl << std::endl;
    printStat(sourceTCPPorts);
	std::cout << std::endl << "--- Destination TCP ports ---" << std::endl <<std::endl;
    printStat(destTCPPorts);
	std::cout << std::endl << "--- TCP flags ---" << std::endl <<std::endl;
    printStat(TCPFlags);
	std::cout << std::endl << "--- TCP options ---" << std::endl <<std::endl;
    printStat(TCPOptions);

	std::cout << std::endl << "=== Transport layer: UDP ===" << std::endl << std::endl;
	std::cout << "--- Source UDP ports ---" << std::endl << std::endl;
    printStat(sourceUDPPorts);
	std::cout << std::endl << "--- Destination UDP ports ---" << std::endl <<std::endl;
    printStat(destUDPPorts);

	std::cout << std::endl << "=== Transport layer: ICMP ===" << std::endl << std::endl;
	std::cout << "--- ICMP types ---" << std::endl <<std::endl;
    printStat(icmpTypes);
	std::cout << std::endl << "--- ICMP codes ---" << std::endl <<std::endl;
    printStat(icmpCodes);
	return;
}
void printStat(STRU_STAT stat){
    IT_STRU_STAT it;
    if (0 != stat.size()){
        for (it = stat.begin(); it != stat.end(); it++){
            std::cout << it->first;
            std::cout.width(15);
            std::cout << "\t" << it->second << std::endl;
        }
    }
    else
        std::cout << "(no results)" << std::endl;
    return;
}
/**
 * usage(FILE * file) -> void
 *
 * print the usage of this program to the file stream file
 *
 **/

void usage(FILE * file){
	if(file == NULL){
		file = stdout;
	}

	fprintf(file,
			"wiretap [OPTIONS]\n"
			"  --help         \t Print this help screen\n"
			"  --open\t<capture file to open>\tOpen a valid tcpdump file\n");
}

/**

 * parse the command line arguments using getopt_long and
 * return the capture file name.
 *
 * ERRORS: Will exit on various errors
 *
 **/
std::string parse_args(int argc,  char * argv[]){
	int c;
	char file_name[MAX_FILE_NAME] = "\0";
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"help", no_argument, NULL, 'h'},
			{"open", required_argument, NULL, 'o'},
			{0, 0, 0, 0}
		};

		c = getopt_long (argc, argv, "ho:",
				long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
			case 'h':
				usage(stdout);
				break;

			case 'o':
				memcpy(file_name, optarg, strlen(optarg));
				//if (strcmp)
				//printf ("option o with value '%s'\n", file_name);
				break;

			default:
				usage(stdout);
				break;
		}
	}
	return file_name;
}
