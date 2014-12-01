//
//  utils.h
//  portscanner
//
//  Created by Tony Liu on 11/2/14.
//  Copyright (c) 2014 Tony Liu. All rights reserved.
//

#ifndef portscanner_utils_h
#define portscanner_utils_h
#include <list>
#include <string>
#include <map>
#include <vector>
/*Maximum file name size*/
#define FILE_NAME_MAX 1024
//maximum file size
#define FILE_MAX_SIZE 10240
//default maximum scan port
#define MAX_SCAN_PORT 1024
//maximum netmask
#define MAX_NETMASK 32

// control the verbose output
// defined in bt_client
extern bool VERBOSE;
std::string ServiceName[1024] = {"tcpmux",
    "compressnet",
    "compressnet",
    "",
    "rje",
    "",
    "echo",
    "",
    "discard",
    "",
    "systat",
    "",
    "daytime",
    "",
    "",
    "",
    "qotd",
    "msp",
    "chargen",
    "ftp-data",
    "ftp",
    "ssh",
    "telnet",
    "",
    "smtp",
    "",
    "nsw-fe",
    "",
    "msg-icp",
    "",
    "msg-auth",
    "",
    "dsp",
    "",
    "",
    "",
    "time",
    "rap",
    "rlp",
    "",
    "graphics",
    "name",
    "nicname",
    "mpm-flags",
    "mpm",
    "mpm-snd",
    "ni-ftp",
    "auditd",
    "tacacs",
    "re-mail-ck",
    "",
    "xns-time",
    "domain",
    "xns-ch",
    "isi-gl",
    "xns-auth",
    "",
    "xns-mail",
    "",
    "",
    "ni-mail",
    "acas",
    "whoispp",
    "covia",
    "tacacs-ds",
    "sql-net",
    "bootps",
    "bootpc",
    "tftp",
    "gopher",
    "netrjs-1",
    "netrjs-2",
    "netrjs-3",
    "netrjs-4",
    "",
    "deos",
    "",
    "vettcp",
    "finger",
    "http",
    "",
    "xfer",
    "mit-ml-dev",
    "ctf",
    "mit-ml-dev",
    "mfcobol",
    "",
    "kerberos",
    "su-mit-tg",
    "dnsix",
    "mit-dov",
    "npp",
    "dcp",
    "objcall",
    "supdup",
    "dixie",
    "swift-rvf",
    "tacnews",
    "metagram",
    "",
    "hostname",
    "iso-tsap",
    "gppitnp",
    "acr-nema",
    "cso",
    "3com-tsmux",
    "rtelnet",
    "snagas",
    "pop2",
    "pop3",
    "sunrpc",
    "mcidas",
    "ident",
    "",
    "sftp",
    "ansanotify",
    "uucp-path",
    "sqlserv",
    "nntp",
    "cfdptkt",
    "erpc",
    "smakynet",
    "ntp",
    "ansatrader",
    "locus-map",
    "nxedit",
    "locus-con",
    "gss-xlicen",
    "pwdgen",
    "cisco-fna",
    "cisco-tna",
    "cisco-sys",
    "statsrv",
    "ingres-net",
    "epmap",
    "profile",
    "netbios-ns",
    "netbios-dgm",
    "netbios-ssn",
    "emfis-data",
    "emfis-cntl",
    "bl-idm",
    "imap",
    "uma",
    "uaac",
    "iso-tp0",
    "iso-ip",
    "jargon",
    "aed-512",
    "sql-net",
    "hems",
    "bftp",
    "sgmp",
    "netsc-prod",
    "netsc-dev",
    "sqlsrv",
    "knet-cmp",
    "pcmail-srv",
    "nss-routing",
    "sgmp-traps",
    "snmp",
    "snmptrap",
    "cmip-man",
    "cmip-agent",
    "xns-courier",
    "s-net",
    "namp",
    "rsvd",
    "send",
    "print-srv",
    "multiplex",
    "cl-1",
    "xyplex-mux",
    "mailq",
    "vmnet",
    "genrad-mux",
    "xdmcp",
    "nextstep",
    "bgp",
    "ris",
    "unify",
    "audit",
    "ocbinder",
    "ocserver",
    "remote-kis",
    "kis",
    "aci",
    "mumps",
    "qft",
    "gacp",
    "prospero",
    "osu-nms",
    "srmp",
    "irc",
    "dn6-nlm-aud",
    "dn6-smm-red",
    "dls",
    "dls-mon",
    "smux",
    "src",
    "at-rtmp",
    "at-nbp",
    "at-3",
    "at-echo",
    "at-5",
    "at-zis",
    "at-7",
    "at-8",
    "qmtp",
    "z39-50",
    "914c-g",
    "anet",
    "ipx",
    "vmpwscs",
    "softpc",
    "CAIlic",
    "dbase",
    "mpp",
    "uarps",
    "imap3",
    "fln-spx",
    "rsh-spx",
    "cdc",
    "masqdialer",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "direct",
    "sur-meas",
    "inbusiness",
    "link",
    "dsp3270",
    "subntbcst-tftp",
    "bhfhs",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "rap",
    "set",
    "",
    "esro-gen",
    "openport",
    "nsiiops",
    "arcisdms",
    "hdap",
    "bgmp",
    "x-bone-ctl",
    "sst",
    "td-service",
    "td-replica",
    "manet",
    "",
    "pt-tls",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "http-mgmt",
    "personal-link",
    "cableport-ax",
    "rescap",
    "corerjd",
    "",
    "fxp",
    "k-block",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "novastorbakcup",
    "entrusttime",
    "bhmds",
    "asip-webadmin",
    "vslmp",
    "magenta-logic",
    "opalis-robot",
    "dpsi",
    "decauth",
    "zannet",
    "pkix-timestamp",
    "ptp-event",
    "ptp-general",
    "pip",
    "rtsps",
    "rpki-rtr",
    "rpki-rtr-tls",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "texar",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "pdap",
    "pawserv",
    "zserv",
    "fatserv",
    "csi-sgwp",
    "mftp",
    "matip-type-a",
    "matip-type-b",
    "dtag-ste-sb",
    "ndsauth",
    "bh611",
    "datex-asn",
    "cloanto-net-1",
    "bhevent",
    "shrinkwrap",
    "nsrmp",
    "scoi2odialog",
    "semantix",
    "srssend",
    "rsvp-tunnel",
    "aurora-cmgr",
    "dtk",
    "odmr",
    "mortgageware",
    "qbikgdp",
    "rpc2portmap",
    "codaauth2",
    "clearcase",
    "ulistproc",
    "legent-1",
    "legent-2",
    "hassle",
    "nip",
    "tnETOS",
    "dsETOS",
    "is99c",
    "is99s",
    "hp-collector",
    "hp-managed-node",
    "hp-alarm-mgr",
    "arns",
    "ibm-app",
    "asa",
    "aurp",
    "unidata-ldm",
    "ldap",
    "uis",
    "synotics-relay",
    "synotics-broker",
    "meta5",
    "embl-ndt",
    "netcp",
    "netware-ip",
    "mptn",
    "kryptolan",
    "iso-tsap-c2",
    "osb-sd",
    "ups",
    "genie",
    "decap",
    "nced",
    "ncld",
    "imsp",
    "timbuktu",
    "prm-sm",
    "prm-nm",
    "decladebug",
    "rmt",
    "synoptics-trap",
    "smsp",
    "infoseek",
    "bnet",
    "silverplatter",
    "onmux",
    "hyper-g",
    "ariel1",
    "smpte",
    "ariel2",
    "ariel3",
    "opc-job-start",
    "opc-job-track",
    "icad-el",
    "smartsdp",
    "svrloc",
    "ocs-cmu",
    "ocs-amu",
    "utmpsd",
    "utmpcd",
    "iasd",
    "nnsp",
    "mobileip-agent",
    "mobilip-mn",
    "dna-cml",
    "comscm",
    "dsfgw",
    "dasp",
    "sgcp",
    "decvms-sysmgt",
    "cvc-hostd",
    "https",
    "snpp",
    "microsoft-ds",
    "ddm-rdb",
    "ddm-dfm",
    "ddm-ssl",
    "as-servermap",
    "tserver",
    "sfs-smp-net",
    "sfs-config",
    "creativeserver",
    "contentserver",
    "creativepartnr",
    "macon-tcp",
    "scohelp",
    "appleqtc",
    "ampr-rcmd",
    "skronk",
    "datasurfsrv",
    "datasurfsrvsec",
    "alpes",
    "kpasswd",
    "urd",
    "digital-vrc",
    "mylex-mapd",
    "photuris",
    "rcp",
    "scx-proxy",
    "mondex",
    "ljk-login",
    "hybrid-pop",
    "tn-tl-w1",
    "tcpnethaspsrv",
    "tn-tl-fd1",
    "ss7ns",
    "spsc",
    "iafserver",
    "iafdbase",
    "ph",
    "bgs-nsi",
    "ulpnet",
    "integra-sme",
    "powerburst",
    "avian",
    "saft",
    "gss-http",
    "nest-protocol",
    "micom-pfs",
    "go-login",
    "ticf-1",
    "ticf-2",
    "pov-ray",
    "intecourier",
    "pim-rp-disc",
    "retrospect",
    "siam",
    "iso-ill",
    "isakmp",
    "stmf",
    "mbap",
    "intrinsa",
    "citadel",
    "mailbox-lm",
    "ohimsrv",
    "crs",
    "xvttp",
    "snare",
    "fcp",
    "passgo",
    "exec",
    "login",
    "shell",
    "printer",
    "videotex",
    "talk",
    "ntalk",
    "utime",
    "efs",
    "ripng",
    "ulp",
    "ibm-db2",
    "ncp",
    "timed",
    "tempo",
    "stx",
    "custix",
    "irc-serv",
    "courier",
    "conference",
    "netnews",
    "netwall",
    "windream",
    "iiop",
    "opalis-rdv",
    "nmsp",
    "gdomap",
    "apertus-ldp",
    "uucp",
    "uucp-rlogin",
    "commerce",
    "klogin",
    "kshell",
    "appleqtcsrvr",
    "dhcpv6-client",
    "dhcpv6-server",
    "afpovertcp",
    "idfp",
    "new-rwho",
    "cybercash",
    "devshr-nts",
    "pirp",
    "rtsp",
    "dsf",
    "remotefs",
    "openvms-sysipc",
    "sdnskmp",
    "teedtap",
    "rmonitor",
    "monitor",
    "chshell",
    "nntps",
    "9pfs",
    "whoami",
    "streettalk",
    "banyan-rpc",
    "ms-shuttle",
    "ms-rome",
    "meter",
    "meter",
    "sonar",
    "banyan-vip",
    "ftp-agent",
    "vemmi",
    "ipcd",
    "vnas",
    "ipdd",
    "decbsrv",
    "sntp-heartbeat",
    "bdp",
    "scc-security",
    "philips-vc",
    "keyserver",
    "",
    "password-chg",
    "submission",
    "cal",
    "eyelink",
    "tns-cml",
    "http-alt",
    "eudora-set",
    "http-rpc-epmap",
    "tpip",
    "cab-protocol",
    "smsd",
    "ptcnameservice",
    "sco-websrvrmg3",
    "acp",
    "ipcserver",
    "syslog-conn",
    "xmlrpc-beep",
    "idxp",
    "tunnel",
    "soap-beep",
    "urm",
    "nqs",
    "sift-uft",
    "npmp-trap",
    "npmp-local",
    "npmp-gui",
    "hmmp-ind",
    "hmmp-op",
    "sshell",
    "sco-inetmgr",
    "sco-sysmgr",
    "sco-dtmgr",
    "dei-icda",
    "compaq-evm",
    "sco-websrvrmgr",
    "escp-ip",
    "collaborator",
    "oob-ws-http",
    "cryptoadmin",
    "dec-dlm",
    "asia",
    "passgo-tivoli",
    "qmqp",
    "3com-amp3",
    "rda",
    "ipp",
    "bmpp",
    "servstat",
    "ginad",
    "rlzdbase",
    "ldaps",
    "lanserver",
    "mcns-sec",
    "msdp",
    "entrust-sps",
    "repcmd",
    "esro-emsdp",
    "sanity",
    "dwr",
    "pssc",
    "ldp",
    "dhcp-failover",
    "rrp",
    "cadview-3d",
    "obex",
    "ieee-mms",
    "hello-port",
    "repscmd",
    "aodv",
    "tinc",
    "spmp",
    "rmc",
    "tenfold",
    "",
    "mac-srvr-admin",
    "hap",
    "pftp",
    "purenoise",
    "oob-ws-https",
    "sun-dr",
    "mdqs",
    "disclose",
    "mecomm",
    "meregister",
    "vacdsm-sws",
    "vacdsm-app",
    "vpps-qua",
    "cimplex",
    "acap",
    "dctp",
    "vpps-via",
    "vpp",
    "ggf-ncp",
    "mrm",
    "entrust-aaas",
    "entrust-aams",
    "xfr",
    "corba-iiop",
    "corba-iiop-ssl",
    "mdc-portmapper",
    "hcp-wismar",
    "asipregistry",
    "realm-rusd",
    "nmap",
    "vatp",
    "msexch-routing",
    "hyperwave-isp",
    "connendp",
    "ha-cluster",
    "ieee-mms-ssl",
    "rushd",
    "uuidgen",
    "olsr",
    "accessnetwork",
    "epp",
    "lmp",
    "iris-beep",
    "",
    "elcsd",
    "agentx",
    "silc",
    "borland-dsj",
    "",
    "entrust-kmsh",
    "entrust-ash",
    "cisco-tdp",
    "tbrpf",
    "iris-xpc",
    "iris-xpcs",
    "iris-lwz",
    "pana",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "netviewdm1",
    "netviewdm2",
    "netviewdm3",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "netgw",
    "netrcs",
    "",
    "flexlm",
    "",
    "",
    "fujitsu-dev",
    "ris-cm",
    "kerberos-adm",
    "rfile",
    "pump",
    "qrh",
    "rrh",
    "tell",
    "",
    "",
    "",
    "nlogin",
    "con",
    "ns",
    "rxe",
    "quotad",
    "cycleserv",
    "omserv",
    "webster",
    "",
    "phonebook",
    "",
    "vid",
    "cadlock",
    "rtip",
    "cycleserv2",
    "submit",
    "rpasswd",
    "entomb",
    "wpages",
    "multiling-http",
    "",
    "",
    "wpgs",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "mdbs-daemon",
    "device",
    "mbap-s",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "fcp-udp",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "itm-mcell-s",
    "pkix-3-ca-ra",
    "netconf-ssh",
    "netconf-beep",
    "netconfsoaphttp",
    "netconfsoapbeep",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "dhcp-failover2",
    "gdoi",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "iscsi",
    "owamp-control",
    "twamp-control",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "rsync",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "iclcnet-locate",
    "iclcnet-svinfo",
    "accessbuilder",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "omginitialrefs",
    "smpnameres",
    "ideafarm-door",
    "ideafarm-panic",
    "",
    "",
    "",
    "",
    "",
    "",
    "kink",
    "xact-backup",
    "apex-mesh",
    "apex-edge",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "ftps-data",
    "ftps",
    "nas",
    "telnets",
    "imaps",
    "",
    "pop3s",
    "vsinet",
    "maitrd",
    "busboy",
    "garcon",
    "cadlock2",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "surf",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "exp1",
    "exp2",
    "",
    ""};
//port scanner arguments structure
typedef struct _ps_args_t{
    //ip list
    std::list<std::string> ipList;
    //std::map<std::string, int> ipMap;
    //ip prefix
    std::string subnetIp;
    int prefixMask;
    //ip list file
    std::string ipListFile;
    //port list
    std::list<int> portList;
    //thread number, default is one thread
    int nthread;
    //scan flags list
    std::list<std::string> flagList;
    //output verbose flag
    int verbose;
    _ps_args_t():
    nthread(1),
    verbose(0){
        ipListFile = "\0";
    }
    ~_ps_args_t(){
        nthread = 0;
        verbose = 0;
        ipListFile = "\0";
    }
}ps_args_t;

//port scanner task structure
typedef struct _ps_task_t{
    //ip
    std::string ip;
    //port
    int port;
    //flag
    std::string flag;
    //service name (if applicable)
    std::string serviceName;
    //open/filtered/close
    std::string result;
    
    _ps_task_t():
    ip("\0"),
    port(0),
    flag("\0"),
    serviceName("\0"),
    result("\0"){
        
    }
    ~_ps_task_t(){
        ip = "\0";
        port = 0;
        flag = "\0";
        serviceName = "\0";
        result = "\0";
    }
}ps_task_t;

//command line usgage
void usage(FILE * file);
//parse arguments
void parse_args(ps_args_t * ps_args, int argc,  char * argv[]);
//parse the content of the ip list file
void parse_ip_file(std::list<std::string> &ipList, void * buffer, int buffersize);
// print msg to std::cout when VERBOSE is true
// for error, should use perror etc.
bool printMSG(std::string msg);
// take the same args as printf
// print to standard output when VERBOSE is true
bool printMSG(const char *fmt, ...);
//parse ports argument
void parse_ports(std::list<int> &portList, std::string str);
//parse range ports
void parse_range_ports(std::list<int> &portList, std::string str);
//parse prefix argument
void parse_prefix(std::list<std::string> &ipList, const std::string subnetIp, const int prefixMask);
void parse_prefix(std::string &subnetIp, int &prefixMask, std::string str);

//build task queue from ps_args_t
std::vector<ps_task_t> build_task_queue(ps_args_t ps_args);
#endif
