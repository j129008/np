#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#define bufsize 3000



int sock;
struct sockaddr_in addr;
FILE *fp;
socklen_t addrlen;
fd_set rset, allset;
int maxfd;

struct ack{
   int num;
   char end;
}ack;

struct packet{
   int len;
   int num;
   char end;
   char buf[bufsize];
}packet;


int main(int argc, char *argv[]){
   addrlen=sizeof(addr);
   
   //argument error
   if (argc != 3) {
      fprintf(stderr, "Usage : %s [ip] [port]\n", argv[0]);
      return 1;
   }
   
   //creat UDP socket
   int ret=0;
   sock = socket(AF_INET, SOCK_DGRAM, 0);
   maxfd = sock;
   addr.sin_family = AF_INET;
   addr.sin_port = htons(atoi(argv[2]));
   ret=inet_pton(AF_INET, argv[1], &addr.sin_addr.s_addr);
   if(ret!=1){
      printf("port error\n");
      return 1;
   }

   // read file
   FD_ZERO(&allset);
   FD_SET(fileno(stdin) , &allset );
   FD_SET( sock , &allset );
   memset(packet.buf,'\0',bufsize);
   while(1){
      rset=allset;
      select( maxfd + 1 , &rset , NULL , NULL , NULL );
    
      if(FD_ISSET(fileno(stdin),&rset)){
         fgets(packet.buf,bufsize,stdin);
         printf("send %s",packet.buf);
         int n = sendto(sock,&packet,sizeof(packet), 0, (struct sockaddr *)&addr,sizeof(addr));
         if (n < 1) perror("sendto");
      }

      if(FD_ISSET(sock,&rset)){
         recvfrom(sock,&packet,sizeof(packet),0,(struct sockaddr*)&addr,&addrlen);
         printf("%s\n",packet.buf);
      }
   }

   //close socket
   close(sock);
   return 0;
}


