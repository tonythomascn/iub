# Lab 1: Socket Programming: netcat_part

--------
## Team members

+ **Name**: Jiecao Chen (jiecchen)

+ **Name**: Tony Liu (xl41)

## List of files
* `Client.h` `Client.cpp`  : class for client, include connecting to server, reading data from file, digesting the data, sending data to server.
* `Common.h` : head for constants
* `makefile` : makefile
* `Server.h` `Server.cpp` : class for server, include creating server socket, binding a IP address and listening, accepting the client socket, receiving data from client, writing data to a file.
* `utils.h` `utils.cpp` : include several commonly used functions, including digesting data, verifying messgage, parsing command line arguments. 
* `netcat_part.cpp` : main file, control the workflow
* `README.md` : readme file, explain the mechanism, give credit and list references.
* `alphabet.txt` : a text file for test.


## How to compile
Run the following in your terminal

    $make
    
Following commands are also available

	$make clean
	$make tar

## How to execute
After you run `$make` to compile the source files, there will
be a executable file `netcat_part` in the current path. To run the file,
here is an example to run the command.

Open one terminal, '-l' must be add to activate it as a server, run

	$./netcat_part -l localhost results.txt

Open another terminal, as a client, no '-l' is allowed, run

	$./netcat_part -o 3 -n 3 localhost alphabet.txt 

The file results.txt should be just 3 characters (-n 3), offset by 3 bytes (-o 3):

> DEF

Basically, the bytes 3 through 5 of file `alphabet.txt` was transmitted into `results.txt`.

The default of the program is to send all contents from the file you provided without providing offset(-o) and how many bytes(-n) to send. IP address, port(-p), message(-m "MSG"), verbose the log(-v) are also supported. 

## Get help

To get help of the compiled command, run

	$./netcat_part -h

------------------------
## Reference

+ man page
+ http://www.cplusplus.com/reference/
+ http://www.binarytides.com/code-a-simple-socket-client-class-in-c/
+ https://www.gitorious.org/inspircd/inspircd/source/e842add667c59d49b8c635aafd48e35c427b811d:src/inspircd_io.cpp
+ some code directly copied from the **base-code** provided for this lab.
