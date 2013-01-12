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

#define SA struct sockaddr
#define MAXLINE      4096   
#define SERV_PORT    4361    //connect port is 4361


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

   int i , maxi , maxfd , listenfd , connfd , sockfd ;
   int nready , client[ FD_SETSIZE ] ;
   ssize_t n ;
   fd_set rset , allset ;
   char buf [ MAXLINE ];
   socklen_t clilen ;
   struct sockaddr_in cliaddr , servaddr ;

   //creat socket
   listenfd = socket( AF_INET , SOCK_STREAM , 0 );
   //set socket
   bzero(&servaddr , sizeof( servaddr ));
   servaddr.sin_family = AF_INET ;
   servaddr.sin_addr.s_addr = htonl( INADDR_ANY );
   servaddr.sin_port = htons( SERV_PORT ) ;
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

   printf("ready to wait \n listenfd=%d",listenfd);

   for(;;){
      rset = allset ;
      //nready is the fd ready to ready
      nready = select( maxfd + 1 , &rset , NULL , NULL , NULL );

      if(FD_ISSET(listenfd , &rset) ){
         //connect to ready client
         clilen = sizeof( cliaddr ) ;
         connfd = accept( listenfd , (SA* )&cliaddr , &clilen );

         printf("start to read: \n");
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
         }
         //wait connfd when select
         FD_SET(connfd, &allset);
         if(connfd > maxfd )maxfd = connfd;
         if(i > maxi )maxi = i;
         if(--nready <= 0)continue;
      }
      //seek connected socket
      for(i=0;i<=maxi;i++){
         if( (sockfd = client[i]) < 0 )continue;
         if(FD_ISSET(sockfd,&rset)){
            if((n = readline(sockfd , buf , MAXLINE )) == 0 ){
               close(sockfd);
               FD_CLR(sockfd , &allset );
               client[i] = -1;
            }
            else if(n>0){
               int k;
               //write messege to all client
               printf("%s",buf);
               for(k=0;k<=maxi;k++){
                  if(client[k]!=-1){
                     printf("write in k=%d\n",k);
                     if((write(client[k] , buf , n))!=n)printf("write error!\n");
                  }
               }
            }
            else if (n<0)printf("error!\n");	
            if(--nready <= 0) break;
         }

      }

   }


}

