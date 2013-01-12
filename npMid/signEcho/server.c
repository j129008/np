#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "readline.h"
#include "rmnewline.h"
#include "rmhead.h"
#include "mywrite.h"
#include "myerr.h"
#include "myTcpListen.h"
#include "myServer.h"



void myServer();

int main(int argc, char* argv[]){
   int wPort=0;

   if(argc>1){
      wPort=atoi(argv[1]);
   }else{
      printf("please input port\n");
      exit(0);
   }
   int listenfd=myTcpListen(wPort);
   myServer(listenfd);
   return 0;
}
