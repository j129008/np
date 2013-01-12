#define MAXLINE     4096

void myServer( int listenfd ){
  char    buf[MAXLINE];
  pid_t childpid;
  int n;
  char fname[30];
  int i;
  FILE* ff;

  for(;;){
    int connfd = accept(listenfd,(SA*)NULL,NULL);
    if(-1==connfd) err_quit("accept error");
    printf("Got a connection on socket %d\n",connfd);

    // start implement here
    if( ( childpid = fork() ) == 0 ){
      close(listenfd);

      printf("strat to read\n");

      if( read( connfd,buf,sizeof(fname) ) <= 0 ){
        printf("client close connection\n"); return;
      }

      snprintf(fname,sizeof(fname),"srv_%s",buf);
      if((ff=fopen(fname,"w"))<=0) printf("fail to open file:''\n",buf);

      for(;;){
        for(i=0;i<MAXLINE;i++) buf[i]=0;
        if( read(connfd,buf,MAXLINE) <= 0){
          printf("close..\n"); break;
        }
        fprintf(ff,"%s",buf);
      }
      fclose(ff);
      exit(0);
    }
    close(connfd);
  }
}
