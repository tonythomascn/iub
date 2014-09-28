# Roadmap for BitTorrent Client Development
## Classes support
We need several C++ classes to support our developement of the BitTorrent Client.
Here is a tentative list:
### **CThread** for base thread class
provides virtual thread interface and thread safe mechanism.

### **CFileOperation** for I/O supports
provides general I/O supports, it will support
safe read/write with elegant exeption handling.

### **CSocket** for socket programming
C++ wrapper for a set of socket functions
support connecting, sending data, receiving data etc.
provide elegant exception handling.

### **CPeer** for peer access
provides interfaces to access peers, like handshake, download and upload.

### **CPeerManager** for peers and files management
manages files queue and CPeers queue in every file, provides status of download and upload, and stores user informations.

### **CLog** for Log processing
Generate/Parse logs, with friendly interface.

### **CTest** for testing
provide specific functions for testing our project


## Library support
other common purpose functions can be packed in to `utils. [cpp | h]`.


## Develop `BtClient` class
The main task of our project can be considered as developing
a C++ class `BtClient` in which we encode the **client protocl**.
A way to describe the `BtClient` can be found as below:

``` 
	class BtClient {
		// methods
		// constructor, args TBD
		BtClient(...);
		// .torrent file parser
		bool parseTorrent(string path_to_file);
		// perform handshake step
		bool handshake(...);
		// send message to dest_id
		bool sendMSG(bt_msg_t msg, string dest_id);
		// recieve message from some dest
		bool recieveMSG(...);
		// parse received msg and take action
		bool takeAction(...)
	}

```
The detail args in each method have not yet been decided, we will implementthem in a relatively optimal way in our later development.
As we can see, the implementation of each of those method heavily relies on our previously described classes, for example, `handshake(...)` need the support from both `CSocket` and `CLog`, `takeAction(...)` needs I/O support from `CFileOperation` and `Socket` support from `CSocket`. Hence we reduce the complexity of this project by creating those supporting C++ classes.

## Handling with Muliple Socket Connections
Our client must be able to handle multiple connections seamlessly. That is, a client will have open sockets and it needs to be able to read and send on all of them. However some operations are blocking.

We will use `select()` and `poll()` to handle with those issues.
