#define	BUFLEN		1024	/* maximum response size    */

struct send_num{
  long int a;
  long int b;
  long int c;
};

void myClient(int connfd ){
  char		buf[BUFLEN+1];  
  for(;;){
    // implement here
    struct send_num nums;
    while(fgets(buf ,BUFLEN,stdin)){
      printf("own:%s",buf);
      mywrite(connfd, buf );
      if(read(connfd,&nums,sizeof(nums)) == 0){
        printf("read error"); exit(0);
      }
      printf("Max:%ld  Min:%ld",nums.a,nums.b);
    }
    // end implement
  }
}
