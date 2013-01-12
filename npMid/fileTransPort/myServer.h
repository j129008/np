#define MAXLINE     4096

void myServer( int listenfd ){
  // time_t  ticks;
  char    buf[MAXLINE];
  pid_t pid;
  FILE* fp;

  for(;;){
    int connfd = accept(listenfd,(SA*)NULL,NULL);
    if(-1==connfd) err_quit("accept error");
    printf("Got a connection on socket %d\n",connfd);
 
    // start implement here

    if(pid=fork()==0){
      close(listenfd);
      read(connfd,buf,sizeof(buf));
      printf("%s",buf);
      bzero(buf,sizeof(buf));
      fp=fopen("server.doc","w");
      while(1){
        if(read(connfd,buf,sizeof(buf))== 0){
          break;
        }
        fprintf(fp,"%s",buf);
      }
      fclose(fp);
      close(connfd);
      puts("connection closed");
      exit(1);
    }
    close(connfd);
  }
}
