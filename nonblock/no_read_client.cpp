#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define MAX_LINE 1024
#define WAIT_TIME 500000
#define ERR_RETURN(x...) do{ fprintf(stderr, x); exit(1); }while(0)
int setup_socket(char *ip, char *port);
void client_echo(int sockfd);
void sig_alarm(int id);
int sockfd;

int main(int argc, char **argv){
   if(argc != 3)
      ERR_RETURN("Usage: no_read_client <IP address> <Port number>\n");
   sockfd = setup_socket(argv[1], argv[2]);
   client_echo(sockfd);
}
int setup_socket(char *ip, char *port){
   int sockfd;
   sockaddr_in sin;
   hostent *h = gethostbyname(ip);
   if(!h)
      ERR_RETURN("Error: Get host by name error.\n");

   memset(&sin, 0, sizeof(sin));
   sin.sin_family = AF_INET;
   sin.sin_port = htons(atoi(port));
   memcpy(&sin.sin_addr, h->h_addr, h->h_length);

   if((sockfd = socket(PF_INET, SOCK_STREAM, 0)) <0)
      ERR_RETURN("Error: Create socket error.\n");
   if(connect(sockfd, (sockaddr*)&sin, sizeof(sin)) <0)
      ERR_RETURN("Error: Connect to the server error.\n");
   return sockfd;
}
void client_echo(int sockfd){
   signal(SIGALRM, sig_alarm);
   siginterrupt(SIGALRM, 1);
   ualarm(WAIT_TIME,WAIT_TIME);

   while(true){
      char buf[MAX_LINE+1];
      memset(buf, 0, MAX_LINE+1);
      int len = read(0, buf, MAX_LINE);
      if(len >0){
         buf[len] = 0;
         write(sockfd, buf, len);
      }

   }
}
void sig_alarm(int id){
   char buf[MAX_LINE];
   memset(buf, 0, MAX_LINE);
   int len = recv(sockfd, buf, MAX_LINE, MSG_DONTWAIT);
   if(len >0){
      write(1, "\n### receive from socket begin ###\n", strlen("\n### receive from socket begin ###\n"));
      buf[len] = 0;
      write(1, buf, len);
      write(1, "\n### receive from socket end ###\n", strlen("\n### receive from socket end ###\n"));
   }
   ualarm(WAIT_TIME,WAIT_TIME);
}
