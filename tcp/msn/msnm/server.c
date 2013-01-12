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
#include "readline.h"
#include "rmnewline.h"
#include "rmhead.h"
#include "mywrite.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define SA struct sockaddr
#define MAXLINE      4096   
#define CLI_LIMIT    10


int main(int argc, char* argv[]){
  //  sockfd : socket file decriptor
  int i , cliNum , maxfd , listenfd , connfd , sockfd ;
  int nready , client[FD_SETSIZE] ,clientPort[FD_SETSIZE];
  char clientName [FD_SETSIZE][100] ,clientAddr[FD_SETSIZE][100],clientNameSav[FD_SETSIZE][100];  
  ssize_t n ,len;
  fd_set rset , allset ;
  char buf [ MAXLINE ],send[MAXLINE];
  socklen_t clilen ;
  struct sockaddr_in cliaddr , servaddr ;

  // creat listen socket
  listenfd = socket( AF_INET , SOCK_STREAM , 0 );

  // set socket
  bzero(&servaddr , sizeof( servaddr ));
  servaddr.sin_family = AF_INET ;
  servaddr.sin_addr.s_addr = htonl( INADDR_ANY );
  if(argc!=2)printf("please input port\n"),exit(0);
  servaddr.sin_port = htons( atoi(argv[1]) ) ;

  // bind port
  bind(listenfd , (SA*) &servaddr ,sizeof(servaddr) );

  // listen and set pending number
  printf("server listen\n");
  listen(listenfd , CLI_LIMIT );  //  max client is 10

  // max client fd number
  maxfd = listenfd ;

  // num of client online
  cliNum = -1;

  // all client port are closed now
  for(i = 0; i < FD_SETSIZE ; i++) client[i] = -1 ;

  // initial select set
  FD_ZERO(&allset);
  // add listenfd into allset
  FD_SET(listenfd , &allset );



  /******************          wait for connect      ********************/
  for(;;){
    rset = allset ;
    // modify rset to reflect which fd is ready for reading and return a ready fd
    nready = select( maxfd + 1 , &rset , NULL , NULL , NULL );
      // if thereis no sockfd can use, next
      if(nready < 0)continue;

    /*********  new client connect   **************/
    if(FD_ISSET(listenfd,&rset)){
      // connect to client
      clilen = sizeof( cliaddr ) ;
      connfd = accept( listenfd , (SA* )&cliaddr , &clilen );
      // save fd to client[i]
      for(i = 0 ; i<FD_SETSIZE; i++){
        if(client[i]<0) {
          strcpy(clientAddr[i],inet_ntoa(cliaddr.sin_addr) );
          clientPort[i]=cliaddr.sin_port;
          client[i] = connfd;
          break;
        }
      }

      // if full, exit
      if(i == FD_SETSIZE){
        printf("too many client\n");
        exit(0);
      }

      // add connfd into allset
      FD_SET(connfd, &allset);

      // check max
      if(connfd > maxfd )maxfd = connfd;
      if(i > cliNum )cliNum = i;
      
      // say hello to client
      mywrite(client[i],"/serv What's your name?\n");
    }

    /********   receive and send message with client  **********/
    for(i=0;i<=cliNum;i++){
      // find connected client sockfd
      if( (sockfd = client[i]) < 0 )continue;

      int k;
      // if sockfd is in read set
      if( FD_ISSET(sockfd,&rset) ){
        // if get ctrl+D (NULL) from client, close
        if((n = readline(sockfd , buf , MAXLINE )) == 0 ){
          close(sockfd);
          FD_CLR(sockfd , &allset );
          FD_CLR(sockfd , &rset );
          printf("receive NULL string from client %d\n",i);
          // send to other user
          if(strlen(clientName[i])>0){
            for(k=0;k<=cliNum;k++){
              memset(send,'\0',MAXLINE);
              sprintf(send,"/serv [Server] %s is offline.\n",clientNameSav[i]);
              if(client[k]!=-1 && k!=i){
                mywrite(client[k],send);
              }
            }
          }
          client[i] = -1;
          clientName[i][0]='\0';
          clientNameSav[i][0]='\0';
        }else if(strlen(buf)>0){
          // if get message from client
          printf("client[%d]:%s",i,buf,n);

          // change nick name
          if(!strncmp(buf,"/nick ",6)){
            int sameFlag=0;
            rmhead(buf,6);
            rmnewline(buf);
            // check nick name
            for(k=0;k<=cliNum;k++){
              if( !strcmp(buf,clientName[k]) ){
                if(k!=i){
                  memset(send,'\0',MAXLINE);
                  strcat(send,"/serv [Server] This name has been used by others\n");
                  mywrite(client[i],send);
                }else{
                  memset(send,'\0',MAXLINE);
                  strcat(send,"/serv [Server] Error\n");
                  mywrite(client[i],send);
                }
                sameFlag=1;
                break;
              }
              
              
              if(strlen(buf)<2 || strlen(buf)>12){
                strcpy(send,"/serv [Server] Username can only consists of 2~12 digits or English letters.\n");
                mywrite(client[i],send);
                sameFlag=1;
                break;
              }
            }
            // if the nick name can use
            if(!sameFlag){
              printf("client[%d] change nick name\n",i);
              memset(send,'\0',MAXLINE);
              sprintf(send,"/serv [Server] You're now known as %s.\n",buf);
              mywrite(client[i],send);
              // send to other user
              for(k=0;k<=cliNum;k++){
                memset(send,'\0',MAXLINE);
                sprintf(send,"/serv [Server] %s is now known as %s\n",clientName[i],buf);
                if(client[k]!=-1 && k!=i){
                  mywrite(client[k],send);
                }
              }
              // save client name
              strcpy(clientName[i],buf);
            }
          }else if(!strncmp(buf,"/who",4)){
            // get /who message
            for(k=0;k<=cliNum;k++){
              printf("return client[%d](sockfd %d) : %s\n",i,client[i],clientName[k]);
              memset(send,'\0',MAXLINE);
              sprintf(send,"[Server] %s %s:%d\n",clientName[k],clientAddr[k],clientPort[k]);
              mywrite(client[i],send);
            }
          }else if(!strncmp(buf,"/initName:",10)){
            // client initialize
            rmhead(buf,10);
            rmnewline(buf);
            int sameFlag=0;
            // check same name
            for(k=0;k<=cliNum;k++){
              if(strlen(buf)<2 || strlen(buf)>12){
                strcpy(send,"/serv [Server] Username can only consists of 2~12 digits or English letters.\n");
                mywrite(client[i],send);
                sameFlag=1;
                break;
              }

              if( !strcmp(buf,clientName[k]) ){
                strcpy(send,"/serv [Server] This name has been used by others\n");
                mywrite(client[i],send);
                sameFlag=1;
                break;
              }
            }

            if(!sameFlag){
              strcpy(clientName[i],buf);
              strcpy(clientNameSav[i],buf);
              printf("client %s is coming\n",clientName[i]);
              memset(send,'\0',MAXLINE);
              sprintf(send,"/serv Hello %s, welcome! ServerIP:%s:%d\n",clientName[i],clientAddr[i],atoi(argv[1]) );
              mywrite(client[i],send);

              // send to other users: xxx is online
              for(k=0;k<=cliNum;k++){
                memset(send,'\0',MAXLINE);
                sprintf(send,"/serv [Server] %s is online.\n",clientName[i]);
                if(client[k]!=-1 && k!=i){
                  mywrite(client[k],send);
                }
              }
            }
          }else{
            // send message to all user
            for(k=0;k<=cliNum;k++){
              memset(send,'\0',MAXLINE);
              sprintf(send,"/msg %s SAID: %s",clientName[i],buf);
              if(client[k]!=-1){
                mywrite(client[k],send);
              }
            }
          }
        }else if (n<0)printf("error!\n");	
        if(--nready <= 0) break;
      }
    }

  } // for(;;) end
} // main end

