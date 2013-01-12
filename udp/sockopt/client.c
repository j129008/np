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
struct sockaddr_in senderinfo;
socklen_t addrlen;
struct timeval timeval, myTimeval;

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

int resendEndAck=0;


int main(int argc, char *argv[]){
   // set timeval
   timeval.tv_sec=1;
   timeval.tv_usec=0;

   addrlen=sizeof(senderinfo);
   fp=fopen("/tmp/input.tar","rb");
   if(fp==NULL){
      printf("open file error!!\n");
      return 1;
   }
   
   //argument error
   if (argc != 3) {
      fprintf(stderr, "Usage : %s [ip] [port]\n", argv[0]);
      return 1;
   }
   
   //creat UDP socket
   int ret=0;
   sock = socket(AF_INET, SOCK_DGRAM, 0);
   addr.sin_family = AF_INET;
   addr.sin_port = htons(atoi(argv[2]));
   ret=inet_pton(AF_INET, argv[1], &addr.sin_addr.s_addr);
   if(ret!=1){
      printf("port error\n");
      return 1;
   }

   // set socket option
   setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeval,sizeof(timeval));
   

   // read file
   int dataLen=0;
   int rcvLen=0;
   memset(packet.buf,'\0',bufsize);
   packet.end='0';
   packet.num=0;
   while(packet.len=fread(packet.buf,1,bufsize,fp)){
      // send packet
      packet.num++;
      int n = sendto(sock,&packet,sizeof(packet), 0, (struct sockaddr *)&addr,sizeof(addr));
      if (n < 1) perror("sendto");
      printf("client: send packet num: %d, string: %s\n",packet.num, packet.buf);
      // while get ok_num send next packet in wait time else resend
      while(1){
         // wait 1 sec to get ack , otherwise resend data
         if(rcvLen=recvfrom(sock,&ack,sizeof(ack),0,(struct sockaddr*)&senderinfo,&addrlen)>0 ){
            // if get ack, continue
            printf("client: get ack num: %d\n",ack.num);

            // if get success ack, send next data
            if(ack.num==packet.num)break;
         }else{
            sendto(sock,&packet,sizeof(packet), 0, (struct sockaddr *)&addr,sizeof(addr));
            printf("client: resend packet num: %d, string: %s\n",packet.num, packet.buf);
         }   
      }
      memset(packet.buf,'\0',bufsize);
   }
   // send end packet
   int getEndAck=0;
   while(!getEndAck){
      packet.end='1';
      int n = sendto(sock,&packet,sizeof(packet), 0, (struct sockaddr *)&addr,sizeof(addr));
      if (n < 1) perror("sendto");
      else printf("client: send end message\n");
      while(1){
         // wait 1 sec to get ack , otherwise resend data
         if( rcvLen=recvfrom(sock,&ack,sizeof(ack),0,(struct sockaddr*)&senderinfo,&addrlen)>0 ){
            // if get ack, continue
            printf("client: get end ack, close\n");
            if(ack.end=='1'){
               getEndAck=1;
               break;
            }
         }else{
            printf("client: resend end message\n");
            sendto(sock,&packet,sizeof(packet), 0, (struct sockaddr *)&addr,sizeof(addr));
            if(packet.end=='1'){
               resendEndAck++;
               printf("resend end ack time: %d\n",resendEndAck);
            }else if(resendEndAck==5){
               printf("trans end\n");
               exit(0);
            }
         }
      }
   }

   //close socket
   close(sock);
   fclose(fp);
   return 0;
}


