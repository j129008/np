#include <stdio.h>		
#include <sys/types.h>	
#include <netinet/in.h>	
#include <sys/socket.h>	
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include "readline.h"
#include "rmnewline.h"
#include "rmhead.h"
#include "mywrite.h"
#include "myTcpConnect.h"
#include "myClient.h"

#define	HOSTNAMELEN	40	/* maximal host name length */

void myClient(int connfd);


int main(int argc, char *argv[]){
  if (argc < 3) {
    fprintf(stderr, "Missing host name or port\n");
    exit (1);
  }

  int connfd = myTcpConnect(argv,atoi(argv[2]));
  myClient(connfd);
  return 0;
}

