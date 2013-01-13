#!/usr/bin/bash
gcc -g -o serv -Wall server.c;
gcc -g -lpthread -o cli  -Wall client.c;
