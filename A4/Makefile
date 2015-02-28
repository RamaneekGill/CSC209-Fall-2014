PORT=55754
CFLAGS = -DPORT=$(PORT) -g -Wall

calserver: calendar_server.o lists.o 
	gcc $(CFLAGS) -o calserver calendar_server.o lists.o

calendar_server.o: calendar_server.c calendar_server.h lists.h
	gcc $(CFLAGS) -c calendar_server.c

lists.o: lists.c lists.h
	gcc $(CFLAGS) -c lists.c

clean: 
	rm calserver *.o
