all: server agent

server: server.o help.o
	gcc -Wall -g server.o help.o -o server

agent: agent.o help.o
	gcc -Wall -g agent.o help.o -o agent

server.o: server.c help.h
	gcc -Wall -g -c server.c

agent.o: agent.c help.h
	gcc -Wall -g -c agent.c

help.o: help.c help.h
	gcc -Wall -g -c help.c
