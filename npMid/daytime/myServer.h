#define MAXLINE     4096

void myServer( int listenfd ){
  time_t  ticks;
  char    buf[MAXLINE];
  for(;;){
    int connfd = accept(listenfd,(SA*)NULL,NULL);
    if(-1==connfd) err_quit("accept error");
    printf("Got a connection on socket %d\n",connfd);
    
    ticks = time(NULL);
    snprintf(buf,sizeof(buf),"%.24s\r\n",ctime(&ticks));
    write(connfd,buf,strlen(buf));
    close(connfd);
    puts("connection closed");
  }
}
