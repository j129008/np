// receive data
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>    
#include <sys/time.h>
#define bufsize 3000

int sock;
struct sockaddr_in addr;
struct sockaddr_in senderinfo;
socklen_t addrlen;
int n;
FILE *fp;
struct timeval timeintv_a, timeintv_b;

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
   if(argc!=2){
      printf("usage: %s port\n",argv[0]);
      return 1;
   }
   //creat UDP socket
   sock = socket(AF_INET, SOCK_DGRAM, 0);
   addr.sin_family = AF_INET;
   addr.sin_port = htons(atoi(argv[1]));
   addr.sin_addr.s_addr = INADDR_ANY;
   bind(sock, (struct sockaddr *)&addr, sizeof(addr));

   addrlen = sizeof(senderinfo);
   while(1){
      // get data
      recvfrom(sock,&packet,sizeof(packet),0,(struct sockaddr*)&senderinfo,&addrlen);
      printf("server: recv packet num: %d, string: %s\n",packet.num,packet.buf);
   }

   
   close(sock);
   fclose(fp);
   return 0;
}

