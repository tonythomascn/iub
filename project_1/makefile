OBJS = netcat_part.o Client.o utils.o Server.o
cc = g++
DEBUG = -g
CFLAGS =  -c -Wall $(DEBUG) 
LFLAGS =  -Wall $(DEBUG) 

netcat_part : $(OBJS)
	$(cc) $(LFLAGS) $(OBJS) -o netcat_part -lcrypto
netcat_part.o : netcat_part.cpp Client.h Server.h common.h
	$(cc) $(CFLAGS) netcat_part.cpp Client.h common.h
Client.o : Client.h Client.cpp utils.h common.h
	$(cc) $(CFLAGS) Client.h Client.cpp utils.h common.h
Server.o : Server.h Server.cpp utils.h common.h
	$(cc) $(CFLAGS) Server.h Server.cpp utils.h common.h
utils.o : utils.cpp utils.h common.h
	$(cc) $(CFLAGS) utils.h utils.cpp common.h 

clean :
	rm -f *.o 
	rm -f *~
	rm -f *.gch
	rm -f netcat_part

tar :
	tar -cvf netcat_part.tar *.h *.cpp README.md makefile

