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

static void sig_alrm(int signo){
   fclose(fp);
   // get end time
   if(gettimeofday(&timeintv_b,NULL)!=0){
      printf("gettimeofday failed\n");
      exit(1);
   }
   printf("close by longest time out\n");
   float timecost=0;
   timecost=(timeintv_b.tv_sec-timeintv_a.tv_sec)*1000;
   timecost+=(timeintv_b.tv_usec-timeintv_a.tv_usec)/1000;
   printf("*************\nTotal time cost: %f ms\n*************\n",timecost);
   exit(0);
}

int main(int argc, char *argv[]){
   // get start time
   if(gettimeofday(&timeintv_a,NULL)!=0){
      printf("gettimeofday failed\n");
      exit(1);
   }

   int i=0;
   fp=fopen("/tmp/received.tar","wb");
   if(fp==NULL){
      printf("openfile error\n");
      return 1;
   }

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

   signal(SIGALRM, sig_alrm);
   addrlen = sizeof(senderinfo);
   int dataNum=0;
   while(1){
      // get data
      recvfrom(sock,&packet,sizeof(packet),0,(struct sockaddr*)&senderinfo,&addrlen);
      printf("server: recv packet num: %d, string: %s\n",packet.num,packet.buf);
      if(packet.num==dataNum+1){
         fwrite(packet.buf,1,packet.len,fp);
         fflush(fp);
         printf("server: write packet num: %d\n",packet.num);
         dataNum=packet.num;
      }
      // send ack
      ack.num=packet.num;
      if(packet.end=='0'){
         printf("server: send ack num: %d\n",ack.num);
         sendto(sock,&ack,sizeof(ack),0,(struct sockaddr*)&senderinfo,addrlen);               
         memset(packet.buf,'\0',sizeof(packet.buf));
      }else if(packet.end='1'){
         printf("server: send end ack and close\n");
         ack.end='1';
         for(i=0;i<5;i++){
            sendto(sock,&ack,sizeof(ack),0,(struct sockaddr*)&senderinfo,addrlen);               
         }
         memset(packet.buf,'\0',sizeof(packet.buf));
         break;
      }
   }

   // get end time
   if(gettimeofday(&timeintv_b,NULL)!=0){
      printf("gettimeofday failed\n");
      exit(1);
   }
   float timecost=0;
   timecost=(timeintv_b.tv_sec-timeintv_a.tv_sec)*1000;
   timecost+=(timeintv_b.tv_usec-timeintv_a.tv_usec)/1000;
   printf("*************\nTotal time cost: %f ms\n*************\n",timecost);
   
   close(sock);
   fclose(fp);
   return 0;
}

