#common rules
CFLAGS	=   -g -Wall
CLINK	= 	-c -o
CC      = 	cc
CPP     = 	g++
RM      = 	rm -f 

#include and library path
INCLUDE	=
LDFLAGS =
LIB	= $(SYSLIBS) -lcrypto -lpthread

FTPCLIENTOBJ	= bt_client.o bt_lib.o bt_setup.o CLog.o CFileOperation.o Peer.o

EXECUTABLE	= bt-client

all:		$(EXECUTABLE)

clean:  
	$(RM) *.o *~ 
tar:
	tar -cvf bt_client.tar *.h *.cpp *.md *.torrent Makefile

clobber: clean
	$(RM) $(EXECUTABLE)

#compile rules
.SUFFIXES:	.cpp

$(EXECUTABLE):  $(FTPCLIENTOBJ) 
		$(CPP) $(CFLAGS) -o $@ $(FTPCLIENTOBJ) $(LIB)

%.o:		%.cpp $(HEADERS)
		$(CPP) $(CFLAGS) $(CLINK) $@ $< $(INCLUDE)    
