# 컴파일러 설정
CC = gcc
CFLAGS = -Wall -g
LDLIBS = -lpthread -lrt

# 타겟 파일 설정
TARGETS = csapp hello

# 의존성 규칙
all: $(TARGETS)

csapp: csapp.o
	$(CC) $(CFLAGS) -o csapp csapp.o $(LDLIBS)

hello: hello.o csapp.o
	$(CC) $(CFLAGS) -o hello hello.o csapp.o $(LDLIBS)

csapp.o: csapp.c csapp.h
	$(CC) $(CFLAGS) -c csapp.c

hello.o: hello.c csapp.h
	$(CC) $(CFLAGS) -c hello.c

clean:
	rm -f $(TARGETS) *.o

.PHONY: all clean


# gcc -Wall -g -c csapp.c
# gcc -Wall -g -o hello hello.c csapp.o -lpthread -lrt