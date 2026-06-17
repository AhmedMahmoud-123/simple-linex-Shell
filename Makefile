CC = gcc
CFLAGS = -Wall -Wextra -g

all: myShell

myShell: myShell.o builtins.o parser.o
	gcc -Wall -Wextra -g -o myShell myShell.o builtins.o parser.o

myShell.o: myShell.c
	gcc -Wall -Wextra -g -c myShell.c

builtins.o: builtins.c
	gcc -Wall -Wextra -g -c builtins.c

parser.o: parser.c
	gcc -Wall -Wextra -g -c parser.c

clean:
	rm -f myShell.o builtins.o parser.o myShell
