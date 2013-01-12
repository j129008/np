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
struct timeval timeval;

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

void rmnewline(char* x){
   int len=strlen(x);
   if(x[len-1]=='\n')x[len-1]='\0';
}

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
   char retBuf[bufsize];

   while(1){
      // get data
      memset(packet.buf,'\0',bufsize);
      memset(retBuf,'\0',bufsize);

      // set sockopt to SO_RCVTIMEO
      timeval.tv_sec=500;
      timeval.tv_usec=0;
      setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeval,sizeof(timeval));
      recvfrom(sock,&packet,sizeof(packet),0,(struct sockaddr*)&senderinfo,&addrlen);
      printf("\nstring1:%s\n",packet.buf);
      rmnewline(packet.buf);

      strcat(retBuf,packet.buf);
      memset(packet.buf,'\0',bufsize);
      
      timeval.tv_sec=5;
      timeval.tv_usec=0;
      setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeval,sizeof(timeval));
      recvfrom(sock,&packet,sizeof(packet),0,(struct sockaddr*)&senderinfo,&addrlen);
      printf("string2:%s\n",packet.buf);
      strcat(retBuf,packet.buf);

      printf("\nret string:%s",retBuf);
      
      strcpy(packet.buf,retBuf);
      int n = sendto(sock,&packet,sizeof(packet), 0, (struct sockaddr *)&senderinfo,sizeof(senderinfo));
      if (n < 1) perror("sendto");
   }

   close(sock);
   fclose(fp);
   return 0;
}

