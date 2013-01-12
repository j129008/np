#define	BUFLEN		1024	/* maximum response size    */
#include <errno.h>
#include <signal.h>

void sig_chld(int signo){
  pid_t pid;
  int stat;
  while((pid = waitpid(-1,&stat,WNOHANG))>0) printf("leaving !!\n");
  exit(0);
}

void myClient(int connfd ){
  char		buf[BUFLEN+1];  
  int i;
  pid_t childpid;
  signal( SIGCHLD , sig_chld );
  memset(buf,'\0',BUFLEN);
  
  if( (childpid = fork() ) == 0 ){
    while(fgets(buf ,BUFLEN ,stdin)){
      mywrite(connfd,buf);
      memset(buf,'\0',BUFLEN);
    }
    close(connfd);
    exit(0);
  }else{
    char	buf[BUFLEN] ;
    for(;;){
      if(read(connfd,buf,BUFLEN) <= 0){
        printf("read error"); exit(0);
      }
      printf("%s",buf);
      memset(buf,'\0',BUFLEN);
    }
  }

}
