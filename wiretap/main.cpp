//
//  main.cpp
//  wiretap
//
//  Created by Tony Liu on 10/26/14.
//  Copyright (c) 2014 Tony Liu. All rights reserved.
//
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <iostream>
#include <map>
#include <netinet/if_ether.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netinet/ether.h>
#include <linux/if_ether.h>
#include <pcap.h>
#include <pcap/bpf.h>
#include <pcap/pcap.h>
#define MAX_FILE_NAME 1024
#define IP_HDR_SIZE 20
#define ARP_HDR_SIZE 20
typedef std::map<u_char*, int> STRU_STAT;
typedef STRU_STAT::iterator IT_STRU_STAT;

//=== summary ===
int iLargestPacket = 0;
int iSmallestPacket = 0;
long lPktNumber = 0;
unsigned long long ullTotalPktSize = 0;

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


//show usage
void usage(FILE * file);
//parse args
std::string parse_args(int argc,  char * argv[]);
//print all the statistics
void printStat();
//check ip exist or not
void checkIpExsit(STRU_STAT statIp, u_char * Ip);
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
    //u_char * user;
    pcap_loop(pcap, 0, callback, NULL);
    pcap_close(pcap);
    std::cout<< "pcap file parse finished!\n";
    printStat();
    exit(EXIT_SUCCESS);
}
//parse every packet
void callback(u_char *arg_array, const struct pcap_pkthdr *h, const u_char *bytes){
    timeval now  = h->ts;
    bpf_u_int32 length = h->len;
    //check it's the largest or smallest
    if (length > iLargestPacket)
        iLargestPacket = length;
    else if (length < iSmallestPacket)
        iSmallestPacket = length;
    lPktNumber++;
    ullTotalPktSize += length;
    //std::cout << length << std::endl;
    //pcap_file_header fd;
    struct ethhdr * e = (struct ethhdr*)bytes;
    u_char msg[MAX_FILE_NAME] =  "\0";
    char msg2[MAX_FILE_NAME] = "\0";
    //std::stringstream ss;
    //datalink layer record its source and destination
    for (int i = 0; i < ETH_ALEN - 1; i++) {
        sprintf(msg2 + i * 3, "%02x:", e->h_source[i]);
    }
    sprintf(msg2 + ETH_ALEN * 3 - 3, "%02x", e->h_source[ETH_ALEN - 1]);
    memcpy(msg, msg2, strlen(msg2));
    checkIpExsit(sourceEthernet, msg);
    memset(msg, 0x00, MAX_FILE_NAME);
    for (int i = 0; i < ETH_ALEN - 1; i++) {
        sprintf(msg2 + i * 3, "%02x:", e->h_dest[i]);
    }
    sprintf(msg2 + ETH_ALEN * 3 - 3, "%02x", e->h_dest[ETH_ALEN - 1]);
    memcpy(msg, msg2, strlen(msg2));
    checkIpExsit(destEthernet, msg);
    
    //std::stringstream ss;
    if (ETH_P_IP == ntohs(e->h_proto)){
        //ip
        memcpy(msg, "IP", sizeof("IP"));
        checkIpExsit(networkProtocols, msg);
        //network layer
        struct iphdr *ip = (struct iphdr*)(bytes + IP_HDR_SIZE);
        struct in_addr addr;
        addr.s_addr = ip->saddr;
        memcpy(msg, inet_ntoa(addr), strlen(inet_ntoa(addr)));
        checkIpExsit(sourceIp, msg);
        addr.s_addr = ip->daddr;
        memcpy(msg, inet_ntoa(addr), strlen(inet_ntoa(addr)));
        checkIpExsit(destIp, msg);
    }
    else if (ETH_P_ARP == ntohs(e->h_proto)){
        //arp
        memcpy(msg, "ARP", sizeof("ARP"));
        checkIpExsit(networkProtocols, msg);
        struct arphdr *arp = (struct arphdr*)(bytes + ARP_HDR_SIZE);
        //checkIpExsit(arp, );
    }
    else{
        sprintf(msg2, "%02x", e->h_proto);
        memcpy(msg, msg2, strlen(msg2));
        //memcpy(msg, ntohs(e->h_proto), strlen(ntohs(e->h_proto)));
        checkIpExsit(networkProtocols, msg);
        //std::cout << "This packet is not IP packet!\n";
    }
    return;
}
void checkIpExsit(STRU_STAT statIp, u_char * Ip){
    IT_STRU_STAT it = statIp.find(Ip);
    if (it != statIp.end()){
        it->second++;
    }
    else{
        u_char *msg =  (u_char *)malloc(sizeof(u_char *) * MAX_FILE_NAME);
        memset(msg, 0x00, MAX_FILE_NAME);
        memcpy(msg, Ip, sizeof(Ip));
        //std::cout << ip << std::endl;
        statIp.insert(std::make_pair(msg, 1));
    }
    return;
}
void printStat(){
    IT_STRU_STAT it;
    std::cout << "=== Packet capture summary ===" << std::endl << std::endl <<
    "Start date:" << std::endl <<
    "Duration:" << std::endl <<
    "# Packets:" << lPktNumber << std::endl <<
    "Smallest:" << iSmallestPacket << std::endl <<
    "Largest:" << iLargestPacket << std::endl <<
    "Average:" << ullTotalPktSize / lPktNumber << std::endl << std::endl;
    
    std::cout << "=== Link layer ===" << std::endl << std::endl;
    std::cout << "--- Source ethernet addresses ---" << std::endl << std::endl;
    std::cout << sourceEthernet.size() << std::endl << std::endl;
    for (it = sourceEthernet.begin(); it != sourceEthernet.end(); it++){
        std::cout << it->first << "\t" << it->second << std::endl;
    }
    std::cout << "--- Destination ethernet addresses ---" << std::endl <<std::endl;
    std::cout << destEthernet.size() << std::endl << std::endl;
    for (it = destEthernet.begin(); it != destEthernet.end(); it++){
        std::cout << it->first << "\t" << it->second << std::endl;
    }
    std::cout << "=== Network layer ===" << std::endl << std::endl;
    std::cout <<"--- Network layer protocols ---" << std::endl;
    for (it = destEthernet.begin(); it != destEthernet.end(); it++){
    }
    std::cout << "--- Source IP addresses ---" << std::endl << std::endl;
    for (;0;){
    }
    std::cout << "--- Destination IP addresses ---" << std::endl <<std::endl;
    for (;0;){
        
    }
    std::cout << "--- Unique ARP participants ---" << std::endl <<std::endl;
    for (;0;){
        
    }
    std::cout << "=== Transport layer ===" << std::endl << std::endl;
    std::cout <<"--- Network layer protocols ---" << std::endl << std::endl;
    for (;0;){
    }
    std::cout << "=== Transport layer: TCP ===" << std::endl << std::endl;
    std::cout << "--- Source TCP ports ---" << std::endl << std::endl;
    for (;0;){
    }
    std::cout << "--- Destination TCP ports ---" << std::endl <<std::endl;
    for (;0;){
        
    }
    std::cout << "--- TCP flags ---" << std::endl <<std::endl;
    for (;0;){
        
    }
    std::cout << "--- TCP options ---" << std::endl <<std::endl;
    for (;0;){
        
    }
    std::cout << "=== Transport layer: UDP ===" << std::endl << std::endl;
    std::cout << "--- Source UDP ports ---" << std::endl << std::endl;
    for (;0;){
    }
    std::cout << "--- Destination UDP ports ---" << std::endl <<std::endl;
    for (;0;){
        
    }
    
    std::cout << "=== Transport layer: ICMP ===" << std::endl << std::endl;
    std::cout << "--- ICMP types ---" << std::endl <<std::endl;
    for (;0;){
        
    }
    std::cout << "--- ICMP codes ---" << std::endl <<std::endl;
    for (;0;){
        
    }
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
                   printf ("option o with value '%s'\n", file_name);
                   break;
                   
               default:
                   usage(stdout);
                   break;
           }
       }
    return file_name;
}
