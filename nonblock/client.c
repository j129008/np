#include <stdio.h>		/* Basic I/O routines          */
#include <sys/types.h>		/* standard system types       */
#include <netinet/in.h>		/* Internet address structures */
#include <sys/socket.h>		/* socket interface functions  */
#include <netdb.h>		/* host to IP resolution       */
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>

#define SA struct sockaddr
#define MAXLINE     4096
#define	HOSTNAMELEN	40	  /* maximal host name length */
#define	BUFLEN	  	1024	/* maximum response size    */

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
   FD_ZERO(&fds);
   
   if(sockfd>fileno(stdin))maxfd=sockfd;
   else maxfd=fileno(stdin);
   
   while(1){
      FD_SET(sockfd,&fds);
      FD_SET(fileno(stdin),&fds);
      sel=select(maxfd+1,&fds,NULL,NULL,NULL);
      if(sel<0)printf("select error!\n");
      if(FD_ISSET(sockfd,&fds)){
         if((n=readline(sockfd,recvline,MAXLINE))>0)printf("%s",recvline);
         else if(n==0)printf("close connection\n"),exit(0);
         else if(n<0)printf("error!\n");
      }
      
      if(FD_ISSET(fileno(stdin),&fds)){
         fgets(sendline ,MAXLINE ,stdin);
         len=strlen(sendline);
         while(write(sockfd, sendline , len)==len){
            break;
         };
         memset(sendline,'\0',MAXLINE);
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

   if(argc != 3 ){
      printf("usage: client <ip> <port>\n");
      exit(0);
   }

   sockfd = socket(AF_INET , SOCK_STREAM , 0);

   memset(&servaddr, 0, sizeof(servaddr));			/* first clear out the struct, to avoid garbage  */
   servaddr.sin_family = AF_INET;		        	/* Using Internet address family */  
   servaddr.sin_port = htons(atoi(argv[2]));		      	/* copy port number in network byte order */
   memcpy(&servaddr.sin_addr.s_addr, hen->h_addr_list[0], hen->h_length); 	/* copy IP address into address struct */

   connect( sockfd , (SA * ) &servaddr , sizeof(servaddr) );

   doit(sockfd);

   exit(0);
}

