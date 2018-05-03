# John Harvey


#CONSTANTS
CC = gcc
RUN = ./all
#CFLAGS = -g
# CFLAGS = -ansi -pedantic -Wall -g -DDEBUG


all: msgQueue.o
	${CC} -o all msgQueue.o

msgQueue.o : msgQueue.c
	${CC} ${CFLAGS} -c msgQueue.c

run: all
	 ${RUN}

#clean = clean will remove object files and executables
clean:
	rm -rf *.o *.out server.txt all
