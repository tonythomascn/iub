
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
	-s save_file  	 Save the torrent in directory save_dir (dflt: .)
	-l log_file   	 Save logs to log_filw (dflt: bt-client.log)
	-p ip:port    	 Instead of contacing the tracker for a peer list,
                	 use this peer instead, ip:port (ip or hostname)
                	 (include multiple -p for more than 1 peer)
	-I id         	 Set the node identifier to id (dflt: random)
	-v            	 verbose, print additional verbose info
Currently, we only implemented the `.torrent` file parser (with I/O LOG supports)
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


## Reference
[1]: https://wiki.theory.org/BitTorrentSpecification
