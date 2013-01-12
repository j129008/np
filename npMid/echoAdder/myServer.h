#define MAXLINE     4096

void myServer( int listenfd ){
  char    buf[MAXLINE];
  pid_t childpid;
  long  arg1 , arg2;
  int num;
  int i;
  char ggyy;
  for(;;){
    int connfd = accept(listenfd,(SA*)NULL,NULL);
    if(-1==connfd) err_quit("accept error");
    printf("Got a connection on socket %d\n",connfd);

    // start implement here
    if( (childpid = fork() ) == 0 ){
      close(listenfd);

      printf("strat to read\n");
      for(;;){
        if( (num=read(connfd,buf,MAXLINE))  == 0){
          printf("read error"); return;
        }
        if(sscanf(buf,"%ld%c%ld",&arg1,&ggyy,&arg2)==3){
          for(i=0;i<MAXLINE;i++)buf[i]=0;
          printf("a=%ld b=%ld ans=%ld \n",arg1,arg2,arg1+arg2);
          snprintf(buf,sizeof(buf),"%ld\n",arg1+arg2);
        }else{ 
          snprintf(buf,sizeof(buf),"input error\n");
        }
        printf("%s",buf);

        mywrite(connfd,buf);
      }

      exit(0);
    }
    close(connfd);
    // end implement
  }
}
