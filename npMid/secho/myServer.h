#define MAXLINE     4096

fd_set rset , allset ;
fd_set rset , allset ;
socklen_t clilen ;
struct sockaddr_in cliaddr , servaddr ;

void myServer( int listenfd ){
  char    buf[MAXLINE];
  int maxfd = listenfd ;
  int cliNum = -1;
  int client[FD_SETSIZE];
  int i,nready,connfd,sockfd;

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
        if( !read(sockfd , buf , MAXLINE ) ){
          close(sockfd);
          FD_CLR(sockfd , &allset );
          client[i] = -1;
        }else {
          mywrite(sockfd , buf);
        }
        if(--nready <= 0)
          break;
      }
    }
    // end implement
  }
}
