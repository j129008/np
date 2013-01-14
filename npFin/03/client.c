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

void myClient(char **);


int main(int argc, char *argv[]){
  if (argc < 5) {
    fprintf(stderr, "<host> <port> <port><port>\n");
    exit (1);
  }

  myClient(argv);
  return 0;
}

