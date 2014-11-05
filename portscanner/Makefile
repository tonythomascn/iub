#common rules
CFLAGS	=   -g -Wall
CLINK	= 	-c -o
CC      = 	cc
CPP     = 	g++
RM      = 	rm -f 

#include and library path
INCLUDE	=
LDFLAGS =
LIB	= $(SYSLIBS) -lpthread

FTPCLIENTOBJ	= portScanner.o CFileOperation.o CBaseThread.o utils.o

EXECUTABLE	= portScanner

all:		$(EXECUTABLE)

clean:  
	$(RM) *.o *~ 
tar:
	tar -cvf portScanner.tar *.h *.cpp *.md *.torrent Makefile

clobber: clean
	$(RM) $(EXECUTABLE)

#compile rules
.SUFFIXES:	.cpp

$(EXECUTABLE):  $(FTPCLIENTOBJ) 
		$(CPP) $(CFLAGS) -o $@ $(FTPCLIENTOBJ) $(LIB)

%.o:		%.cpp $(HEADERS)
		$(CPP) $(CFLAGS) $(CLINK) $@ $< $(INCLUDE)    
