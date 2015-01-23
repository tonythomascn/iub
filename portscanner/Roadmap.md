# Roadmap for portScanner Development

## Classes support
We need several C++ classes to support our developement of the portScanner.

### **CBaseThread** for base thread class
provides virtual thread interface and thread safe mechanism.

### **CFileOperation** for I/O supports
provides general I/O supports, it will support
safe read/write with elegant exeption handling.

## Library support
other common purpose functions can be packed in to `utils. [cpp | h]`.

Here is a tentative list:
### **CSocket** for socket programming
C++ wrapper for a set of raw socket functions
support connecting, sending data, receiving data etc.
provide elegant exception handling.

### **CThreadManager** for thread and task management
manages files queue and CPeers queue in every file, provides status of download and upload, and stores user informations.





