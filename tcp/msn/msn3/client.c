#include <stdio.h>		/* Basic I/O routines          */
#include <sys/types.h>		/* standard system types       */
#include <netinet/in.h>		/* Internet address structures */
#include <sys/socket.h>		/* socket interface functions  */
#include <netdb.h>		/* host to IP resolution       */
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include "readline.h"
#include "rmnewline.h"
#include "rmhead.h"
#include "dump_line.h"
#include "mywrite.h"

#define SA struct sockaddr
#define MAXLINE     4096
#define	HOSTNAMELEN	40	  /* maximal host name length */
#define	BUFLEN	  	1024	/* maximum response size    */


char ip[100];
int port=0;


// after connect success, do this func
void doit(int sockfd){
  char recvline[MAXLINE] ;
  int n,maxfd,len,sel;
  static char name[100],send[MAXLINE+100];
  fd_set rset, allset;


  FD_ZERO(&rset);
  if(sockfd>fileno(stdin))maxfd=sockfd;
  else maxfd=fileno(stdin);
  // add stdin & sockfd in set
  FD_SET(sockfd,&allset);
  FD_SET(fileno(stdin),&allset);


  
  int readNameFlag=0;
  // set name
  while(1){
    rset=allset;
    sel=select(maxfd+1,&rset,NULL,NULL,NULL);
    if(sel<0)continue;
    
    // wait server send msg
    if(FD_ISSET(fileno(stdin),&rset) && readNameFlag ){
      fgets(name,100,stdin);
      memset(send,'\0',MAXLINE+100);
      sprintf(send,"/initName:%s",name);
      mywrite(sockfd,send);
      rmnewline(name);
    }
    
    // get from server
    if(FD_ISSET(sockfd,&rset)){
      if( (n=readline(sockfd,recvline,MAXLINE))> 0 ){
        if( !strncmp( recvline,"/serv " ,strlen("/serv") )){
          rmhead( recvline,strlen("/serv ") );
        }
        if( !strncmp( recvline, "/msg " , strlen("/msg") ) ){
          rmhead( recvline,strlen("/msg ") );
        }

        if(!strncmp(recvline,"What's your name?",17)){
          printf("[Server] What's your name?\n");
          readNameFlag=1;
        }

        char helloMsg[100];
        sprintf(helloMsg,"Hello %s, welcome! ServerIP:%s:%d\n",name,ip,port);
        if(!strcmp(recvline,helloMsg)){
          printf("[Server] Hello %s, welcome! ServerIP:%s:%d\n",name,ip,port);
          break;
        }else if(!strcmp(recvline,"[Server] This name has been used by others\n")){
          printf("%s",recvline);
        }else if(!strcmp(recvline, "[Server] Username can only consists of 2~12 digits or English letters.\n") ){
          printf("%s",recvline);
        }
      }
      else if(n==0)printf("close connection\n"),exit(0);
      else if(n<0)printf("error!\n");
    }
  }
  
  while(1){
    rset=allset;
    sel=select(maxfd+1,&rset,NULL,NULL,NULL);
    if(sel<0)continue;
   
    // get message from server
    if(FD_ISSET(sockfd,&rset)){
      if((n=readline(sockfd,recvline,MAXLINE))>0){
        if( !strncmp( recvline,"/serv " ,strlen("/serv") )){
          rmhead( recvline,strlen("/serv ") );
        }
        
        if( !strncmp( recvline, "/msg " , strlen("/msg") ) ){
          rmhead( recvline,strlen("/msg ") );
        }
        printf("%s",recvline);
      }
      else if(n==0)printf("close connection\n"),exit(0);
      else if(n<0)printf("error!\n");
    }
    
    // get message from stdin
    if(FD_ISSET(fileno(stdin),&rset)){
      // chat
      memset(send,'\0',MAXLINE+100);
      fgets(send,MAXLINE+100 ,stdin);
      if(!strncmp(send,"/quit",5)){
        exit(0);
      }
      len=strlen(send);
      if((write(sockfd, send , len))!=len)exit(0);
    }
  }
}


int main(int argc,char* argv[]){

  int sockfd;
  struct sockaddr_in servaddr;
  struct hostent*     hen; 
  char buf[100];

   if(argc < 3 ){
     while(1){
       printf("It is not connected to any server\n");
       printf("Please use /connect <IP address> <Port number>\n");
       scanf("%s %s %d",buf,ip,&port);
       dump_line(stdin);
       if( !strcmp(buf,"/connect") ){
         if( ( hen= gethostbyname(ip) ) ){
           if(port>0){
             break;
           }
         }
       }
     }
   }else{
     /* Address resolution stage */
     hen = gethostbyname(argv[1]);
     strcpy(ip,argv[1]);
     if (!hen) {
       perror("couldn't resolve host name");
     }
  }

  sockfd = socket(AF_INET , SOCK_STREAM , 0);

  /* first clear out the struct, to avoid garbage  */
  memset(&servaddr, 0, sizeof(servaddr));			 

  /* Using Internet address family */
  servaddr.sin_family = AF_INET;		        	  

  /* copy port number in network byte order */
  if(argc>=3){
    servaddr.sin_port = htons(port=atoi(argv[2]));		      	  
  }else{
    servaddr.sin_port = htons(port);
  }
  /* copy IP address into address struct */
  memcpy(&servaddr.sin_addr.s_addr, hen->h_addr_list[0], hen->h_length); 	

  connect( sockfd , (SA * ) &servaddr , sizeof(servaddr) );

  doit(sockfd);

  exit(0);
}

