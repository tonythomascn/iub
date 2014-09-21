 ## Copyright (C) 2014 - Tony Liu
 ## Any question, please contact me.
 ## Email : xl41@indiana.edu
  
 #common rules
CFLAGS	=   -g -Wall
CLINK	= 	-c -o
CC      = 	cc
CPP     = 	g++
RM      = 	rm -f 

#include and library path
INCLUDE	=
LDFLAGS =
LIB	= $(SYSLIBS) -lcrypto

FTPCLIENTOBJ	= bt_client.o bt_lib.o bt_setup.o CLog.o CFileOperation.o

EXECUTABLE	= bt-client

all:		$(EXECUTABLE)

clean:  
	$(RM) *.o *~ 

clobber: clean
	$(RM) $(EXECUTABLE)

#compile rules
.SUFFIXES:	.cpp

$(EXECUTABLE):  $(FTPCLIENTOBJ) 
		$(CPP) $(CFLAGS) -o $@ $(FTPCLIENTOBJ) $(LIB)

%.o:		%.cpp $(HEADERS)
		$(CPP) $(CFLAGS) $(CLINK) $@ $< $(INCLUDE)    
