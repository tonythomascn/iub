
# BitTorrent (BT) client. 

## How to Compile
In the root of the source fold, run

	$ make
which gives a excutable `bt-client` file.

## How to Run
type `./bt-client -h` for help, we get

	bt-client [OPTIONS] file.torrent
	-h            	 Print this help screen
	-b ip         	 Bind to this ip for incoming connections, ports
                	 are selected automatically
	-s save_file  	 Save the torrent in directory save_dir (dflt: ./download)
	-l log_file   	 Save logs to log_filw (dflt: bt-client.log)
	-p ip:port    	 Instead of contacing the tracker for a peer list,
                	 use this peer instead, ip:port (ip or hostname)
                	 (include multiple -p for more than 1 peer)
    -I id            Set the node identifier to id (dflt: random)
	-m l|s           set running mode, l - leecher, s - seeder, (dflt: seeder)
	-v            	 verbose, print additional verbose info

Here is an exmaple to run this function

	$ ./bt-client moby_dick.txt.torrent -v
	Parsing .torrent file ...  DONE!

	Torrent INFO:
	name: moby_dick.txt
	piece_length: 262144 bytes
	length: 31690 bytes
	num_pieces: 1
Our parser also implemements the multi-file .torrent support,
see [Info_in_Multiple_File_Mode](https://wiki.theory.org/BitTorrentSpecification#Info_in_Multiple_File_Mode) for detail.

## Source code List
CLog.cpp|h: Provide Logging function. Use class CFileOperation to maintain a log to record all the important logs. Provide an log API to let other function log what they want.

CFileOperation.cpp|h: Provide file I/O. Support read&write, file lock, directory create, file describer management, error handling.

bt_lib.cpp|h: All the basic data structures, constants and useful functions, like torrent parse, peer id calculation, output.

bt_setup.cpp|h: Command line arguments usage and parse the command line arguments.

Peer.cpp|h: Seeder and Leecher management, file save and check. Support peer handshake, piece, message communication, data integrity check, communication error handling, sockets management, seeder&leecher information management, process output and logging.

bt_client.cpp: Main function. Support multiple seeder&leecher with epoll. Necessary error handling and output.

CBaseThread.cpp|h: Provide several ways of multithread (We had intended to implimented N seeder&leecher in this way, but chose a easier way, so not used in this project). Support thread fork by inheriting and implimenting own virtual function, timer function (wake and sleep), condition function (set and wake).

## Reference
[1]: https://wiki.theory.org/BitTorrentSpecification
[2]: man pages
[3]: http://linux.die.net/man/4/epoll
[4]: http://stackoverflow.com/questions/9847441/setting-socket-timeout
[5]: https://computing.llnl.gov/tutorials/pthreads/
[6]: http://ramprasadk.wordpress.com/2010/06/09/c-programming-linux-color-text-output/
[7]: http://www-01.ibm.com/support/knowledgecenter/ssw_ibm_i_71/rtref/localtr.htm%23localtr