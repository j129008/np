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
#include <errno.h>
#include <sys/ioctl.h>


#define SA struct sockaddr
#define MAXLINE      4096   


int readline(int fd, char* ptr, int maxlen){
   ssize_t n, rc;
   char c;
   for(n = 1; n < maxlen; n++){
      if( (rc = read(fd, &c, 1)) == 1 ){
         *ptr++ = c;
         if(c == '\n')
            break;
      }
      else if(rc == 0){
         *ptr = 0;
         return n-1;
      }
      else
         return -1;
   }
   *ptr = 0;

   return n;
}


int main(int argc, char* argv[]){

   int i , maxi , maxfd , listenfd , connfd , sockfd , val=1;
   int nready , client[ FD_SETSIZE ] ;
   ssize_t n ;
   fd_set rset , allset ;
   char buf [ MAXLINE ];
   socklen_t clilen ;
   struct sockaddr_in cliaddr , servaddr ;
   struct timeval timeval;
   int errClient[499999];
   for(i=0;i<499999;i++)errClient[i]=-1;
   char **savBuf, *pData;
   savBuf = (char **)malloc(499999*sizeof(char *)+499999*MAXLINE*sizeof(char));
   for (i = 0, pData = (char *)(savBuf+499999); i < 499999; i++, pData += MAXLINE)
      savBuf[i]=pData;
   int savBufPtr=0;
   //creat socket
   listenfd = socket( AF_INET , SOCK_STREAM , 0 );
   //set socket
   bzero(&servaddr , sizeof( servaddr ));
   servaddr.sin_family = AF_INET ;
   servaddr.sin_addr.s_addr = htonl( INADDR_ANY );
   if(argc!=2){
      printf("usage: server <port number>\n"); exit(0);
      free(savBuf);
   }
   servaddr.sin_port = htons( atoi(argv[1]) ) ;
   //bind port
   bind(listenfd , (SA*) &servaddr ,sizeof(servaddr) );
   //listen port listenfd
   listen(listenfd , 10 );  // max client is 10
   //consider maxfd 
   maxfd = listenfd ;
   //initial maxi which is num of fd
   maxi = -1;
   //all client port is closed now
   for(i = 0; i < FD_SETSIZE ; i++) client[i] = -1 ;

   //initial select set
   FD_ZERO(&allset);
   //wait listenfd which is in allset when select
   FD_SET(listenfd , &allset );


   for(;;){
      rset = allset ;
      //nready is the fd ready to ready
      timeval.tv_sec=0;
      timeval.tv_usec=0;
      // nonblock (timeval=0)
      nready = select( maxfd + 1 , &rset , NULL , NULL , &timeval );

      if(FD_ISSET(listenfd , &rset) ){
         //connect to ready client
         clilen = sizeof( cliaddr ) ;
         connfd = accept( listenfd , (SA* )&cliaddr , &clilen );

         //sav fd to client[i]
         for(i = 0 ; i<FD_SETSIZE; i++){
            if(client[i]<0) {
               client[i] = connfd;
               break;
            }
         }
         //if full, exit
         if(i == FD_SETSIZE){
            printf("too many client\n");
            exit(0);
            free(savBuf);
         }
         //wait connfd when select
         FD_SET(connfd, &allset);
         if(connfd > maxfd )maxfd = connfd;
         if(i > maxi )maxi = i;
         if(--nready <= 0)continue;
      }

      //seek connected socket
      static int maxBufPtr=0;
      for(i=0;i<=maxi;i++){
         if( (sockfd = client[i]) < 0 )continue;
         val=1;
         ioctl(sockfd,FIONBIO,&val);
         if(FD_ISSET(sockfd,&rset)){
            if((n = readline(sockfd , buf , MAXLINE )) == 0 ){
               close(sockfd);
               FD_CLR(sockfd , &allset );
               client[i] = -1;
            }else if(n>0){
               int k;
               //write messege to all client
               for(k=0;k<=maxi;k++){
                  if(client[k]!=-1){
                     val=1;
                     ioctl(client[k],FIONBIO,&val);
                     if((write(client[k] , buf , n))!=n){
                        int bufPtr=0;
                        for(bufPtr=0;bufPtr<=499999;bufPtr++){
                           if(errClient[bufPtr]==-1){
                              strcpy(savBuf[bufPtr],buf);
                              errClient[bufPtr]=client[k];
                              if(bufPtr>maxBufPtr)maxBufPtr=bufPtr;
                              break;
                           }
                        }
                     }
                  }
               }
            }else if (n<0)printf("error!\n");	
            if(--nready <= 0) break;
         }
      }
      int resendNum=0;
      // rewrite
      for(i=0;i<=maxBufPtr;i++){
         if(errClient[i]!=-1){
            n=strlen(savBuf[i]);
            if((write(errClient[i] , savBuf[i] , n))==n){
               errClient[i]=-1;
            }
            resendNum++;
         }
      }
      static int count=0;
      if(resendNum%100==0 && resendNum!=0)printf("remaind  %f%% to resend (%d/%d)\n", (float)resendNum*100.0/((float)maxBufPtr+1.0), resendNum, maxBufPtr+1);
   }
   free(savBuf);
   return 0;
}

