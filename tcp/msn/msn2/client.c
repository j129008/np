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

#define SA struct sockaddr
#define MAXLINE     4096
#define	HOSTNAMELEN	40	  /* maximal host name length */
#define	BUFLEN	  	1024	/* maximum response size    */
#define	PORT		    4361  /* port of daytime server   */


void rmnewline(char* x){
  int i;	
  for(i=0;i<strlen(x);i++)if(x[i]=='\n'||x[i]=='\r')x[i]='\0';
}

// after connect success, do this func
void doit(int sockfd){
  char	sendline[MAXLINE] , recvline[MAXLINE] ;
  int i,n,maxfd,len,sel;
  static char name[100],send[MAXLINE+100];
  fd_set fds, readfds;
  //initial
  printf("set your account:\n");
  fgets(name,100,stdin);
  printf("your name is %s\n",name);

  FD_ZERO(&fds);
  if(sockfd>fileno(stdin))maxfd=sockfd;
  else maxfd=fileno(stdin);
  while(1){
    // server input
    FD_SET(sockfd,&fds);
    FD_SET(fileno(stdin),&fds);
    sel=select(maxfd+1,&fds,NULL,NULL,NULL);
    if(sel<0)printf("select error!\n");
    if(FD_ISSET(sockfd,&fds)){
      if((n=readline(sockfd,recvline,MAXLINE))>0)printf("%s",recvline);
      else if(n==0)printf("close connection\n");
      else if(n<0)printf("error!\n");
    }

    // user input
    if(FD_ISSET(fileno(stdin),&fds)){
      fgets(sendline ,MAXLINE ,stdin);
      strcat(send,name);
      rmnewline(send);
      strcat(send,":");
      strcat(send,sendline);
      len=strlen(send);
      if((write(sockfd, send , len))!=len)printf("write error!\n");
      memset(send,'\0',MAXLINE+100);
    }
  }

}


int main(int argc,char* argv[]){

  int sockfd;
  struct sockaddr_in servaddr;
  struct hostent*     hen; 

  /* Address resolution stage */
  hen = gethostbyname(argv[1]);
  if (!hen) {
    perror("couldn't resolve host name");
  }

  if(argc < 2 ){
    printf("no addr");
    exit(0);
  }

  sockfd = socket(AF_INET , SOCK_STREAM , 0);
  
  /* first clear out the struct, to avoid garbage  */
  memset(&servaddr, 0, sizeof(servaddr));			 
  
  /* Using Internet address family */ 
  servaddr.sin_family = AF_INET;		 

  /* copy port number in network byte order */  
  servaddr.sin_port = htons(PORT);		     

  /* copy IP address into address struct */
  memcpy(&servaddr.sin_addr.s_addr, hen->h_addr_list[0], hen->h_length); 	

  connect( sockfd , (SA * ) &servaddr , sizeof(servaddr) );

  doit(sockfd);

  exit(0);
}

