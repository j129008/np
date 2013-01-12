#define MAXLINE     4096

fd_set rset , allset ;
fd_set rset , allset ;
socklen_t clilen ;
struct sockaddr_in cliaddr , servaddr ;

FILE* doit(int sockfd,FILE* f,char* buf){
  int n;
  char fname[30], line[MAXLINE];
  int num;
  int i;
  FILE* ff;
  memset(fname,'\0',30);
  ff=f;
  if(ff==NULL){
    sscanf(fname,"%s",buf);
    snprintf(fname,sizeof(fname),"srv_%s",buf);
    for(i=0;i<MAXLINE-30;i++) line[i]=buf[i+30];
    if((ff=fopen(fname,"w"))<=0) printf("fail to open file:''\n",buf);
    printf("open %s\n=============\n",fname);
    fprintf(ff,"%s",line);
    printf("%s",line);
  }else{
    fprintf(ff,"%s",buf);
    printf("%s",buf);
  }
  return ff;
}

void myServer( int listenfd ){
  char    buf[MAXLINE];
  int maxfd = listenfd ;
  int cliNum = -1;
  int client[FD_SETSIZE];
  int i,nready,connfd,sockfd;
  FILE* fpointer[FD_SETSIZE];

  for(i = 0; i < FD_SETSIZE ;i++){
    client[i] = -1 ;
  }
  FD_ZERO(&allset);
  FD_SET(listenfd , &allset );


  for(;;){
    // start implement here
    rset = allset ;
    nready = select( maxfd + 1 , &rset , NULL , NULL , NULL );

    if(FD_ISSET(listenfd , &rset) ){
      clilen = sizeof( cliaddr ) ;
      connfd = accept( listenfd , (SA* )&cliaddr , &clilen );

      printf("start to read: \n");

      for(i = 0 ;i<FD_SETSIZE;i++){
        if(client[i]<0){
          client[i] = connfd;
          break;
        }
      }
      if(i == FD_SETSIZE){
        printf("too many client\n");
        exit(0);
      }
      
      FD_SET(connfd, &allset);
      if(connfd > maxfd )maxfd = connfd;
      if(i > cliNum ) cliNum = i;

      if(--nready <= 0)
        continue;
    }

    for(i = 0; i <= cliNum;i++){
      if( (sockfd = client[i]) < 0 )
        continue;
      if(FD_ISSET(sockfd,&rset)){
        if( !readline(sockfd , buf , MAXLINE ) ){
          fclose(fpointer[i]);
          fpointer[i]=0;
          close(sockfd);
          FD_CLR(sockfd , &allset );
          client[i] = -1;
          printf("client %d is leaving\n",i);
        }else {
          fpointer[i]=doit(sockfd,fpointer[i],buf);
        }
        if(--nready <= 0)
          break;
      }
    }
    // end implement
  }
}
