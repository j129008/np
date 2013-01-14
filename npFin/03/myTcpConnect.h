#include "connect_nonb.h"
int myTcpConnect(char** argv, int port){
   int sockfd;
   struct sockaddr_in	sa;            /* Internet address struct */
   struct hostent*     hen; 	       /* host-to-IP translation */

   hen = gethostbyname(argv[1]);
   if (!hen) {
      perror("couldn't resolve host name");
   }

   memset(&sa, 0, sizeof(sa));
   sa.sin_family = AF_INET;
   sa.sin_port = htons(port);
   memcpy(&sa.sin_addr.s_addr, hen->h_addr_list[0], hen->h_length);
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd < 0) {
      perror("socket: allocation failed");
   }
   // non blocking
   int  rc = connect_nonb(sockfd, (struct sockaddr *)&sa, sizeof(sa),1);
   
   // blocking ver.
   // int  rc = connect(sockfd, (struct sockaddr *)&sa, sizeof(sa));
   
   if (rc) {
      perror("connect");
   }

   return sockfd;
}
