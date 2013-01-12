#define MAXLINE     4096

struct send_num{
  long int a;
  long int b;
  long int c;
};

void myServer( int listenfd ){
  char    buf[MAXLINE];
  struct send_num nums;
  long int arg1 , arg2 ,arg3;
  int n,i;
  int num;

  for(;;){
    int connfd = accept(listenfd,(SA*)NULL,NULL);
    if(-1==connfd) err_quit("accept error");
    printf("Got a connection on socket %d\n",connfd);
    // start implement here
    for(;;){
      if( (num=read(connfd,buf,MAXLINE))  <= 0){
        printf("client close connection\n"); return;
      }

      printf("num=%d",n);
      if(sscanf(buf,"%ld%ld%ld",&arg1,&arg2,&arg3)==3){
        for(i=0;i<MAXLINE;i++){buf[i]=0;}
        nums.a = (arg1>arg2) ? (arg1>arg3) ? arg1 : arg3 : arg2>arg3 ? arg2 : arg3;
        nums.b = (arg1<arg2) ? (arg1<arg3) ? arg1 : arg3 : arg2<arg3 ? arg2 : arg3;
      }else snprintf(buf,sizeof(buf),"input error\n");
      write(connfd, &nums , sizeof(nums) );
    }
    // end implement
  }
}
