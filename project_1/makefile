OBJS = netcat_part.o Client.o
cc = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

all : netcat_part
	$(cc) $(LFAGS) $(OBJS) -o netcat_part
netcat_part.o : netcat_part.cpp Client.h
	$(CC) $(CFLAGS) netcat_part.cpp Client.h
Client.o : Client.h Client.cpp utils.h
	$(CC) $(CFLAGS) Client.h Client.cpp utils.h

clean :
	rm -f *.o 
	rm -f *.*~
	rm -f *.gch
	rm -f netcat_part

tar :
	tar -cvf netcat_part.tar *.h *.cpp README

