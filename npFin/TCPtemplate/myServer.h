#define MAXLINE     4096

void myServer( int listenfd ){
  char    buf[MAXLINE];

  for(;;){
    int connfd = accept(listenfd,(SA*)NULL,NULL);
    if(-1==connfd) err_quit("accept error");
    printf("Got a connection on socket %d\n",connfd);
    // start implement here

    // end implement
  }
}
