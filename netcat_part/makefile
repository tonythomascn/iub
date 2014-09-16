OBJS = netcat_part.o Client.o utils.o Server.o
cc = g++
DEBUG = -g
CFLAGS =  -c -Wall $(DEBUG) 
LFLAGS =  -Wall $(DEBUG) 

netcat_part : $(OBJS)
	$(cc) $(LFLAGS) $(OBJS) -o netcat_part -lcrypto
netcat_part.o : netcat_part.cpp
	$(cc) $(CFLAGS) netcat_part.cpp
Client.o : Client.cpp
	$(cc) $(CFLAGS) Client.cpp
Server.o : Server.cpp
	$(cc) $(CFLAGS) Server.cpp
utils.o : utils.cpp
	$(cc) $(CFLAGS) utils.cpp 

clean :
	rm -f *.o 
	rm -f *~
	rm -f *.gch
	rm -f netcat_part

tar :
	tar -cvf netcat_part.tar *.h *.cpp README.md makefile

