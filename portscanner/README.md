
# portScanner. 

## How to Compile
In the root of the source fold, run

	$ make
which gives a excutable `portScanner` file.

## How to Run
type `./portScanner -h` for help, we get

portScanner [option1, ..., optionN]
--help        	 Display this help screen
--ports <port1, ..., portN-portM>                	 Scan specified ports if there are.
Ports separated by a comma or a range. (dflt: 1 - 1024)
--ip <IP address to scan>                	 Scan an individual IP address.
--prefix <IP prefix to scan>                	 Scan an IP prefix.
--file <file name containing IP addresses to scan>                	 Scan a list of IP addresses from a file respectively.
The IP addresses in the file must be one on each line.
At least specify one of --ip, --prefix, --file.
--speedup <parallel threads to use>                	 Specify the number of threads to be used (dflt: one thread)
--scan <one or more scans>                	 Scan subset of these flags: SYN, NULL, FIN, XMAS, ACK, UDP (dflt: all scan)
--verbose     	 verbose, print additional verbose info
