# Makefile for Proxy Lab
#
# You may modify this file any way you like (except for the handin
# rule). You instructor will type "make" on your specific Makefile to
# build your proxy from sources.

CC = gcc
CFLAGS = -g -Wall
LDFLAGS = -lpthread

all: proxy # echoclient echoserver

csapp.o: csapp.c csapp.h
	$(CC) $(CFLAGS) -c csapp.c

proxy.o: proxy.c csapp.h
	$(CC) $(CFLAGS) -c proxy.c

sbuf.o: sbuf.c sbuf.h
	$(CC) $(CFLAGS) -c sbuf.c

# hash.o: hash.c hash.h
# 	$(CC) $(CFLAGS) -c hash.c

proxy: proxy.o csapp.o sbuf.o #hash.o
	$(CC) $(CFLAGS) proxy.o csapp.o sbuf.o -o proxy $(LDFLAGS)

# echoclient.o: ../echoclient.c
# 	$(CC) $(CFLAGS) -c ../echoclient.c

# echoserver.o: ../echoserver.c
# 	$(CC) $(CFLAGS) -c ../echoserver.c

# echoclient: echoclient.o csapp.o
# 	$(CC) $(CFLAGS) echoclient.o csapp.o -o echoclient $(LDFLAGS)

# echoserver: echoserver.o csapp.o
# 	$(CC) $(CFLAGS) echoserver.o csapp.o -o echoserver $(LDFLAGS)

# Creates a tarball in ../proxylab-handin.tar that you can then
# hand in. DO NOT MODIFY THIS!
handin:
	(make clean; cd ..; tar cvf $(USER)-proxylab-handin.tar proxylab-handout --exclude tiny --exclude nop-server.py --exclude proxy --exclude driver.sh --exclude port-for-user.pl --exclude free-port.sh --exclude ".*")

clean:
	rm -f *~ *.o proxy core *.tar *.zip *.gzip *.bzip *.gz
