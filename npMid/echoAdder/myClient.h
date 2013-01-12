#define	BUFLEN		1024	/* maximum response size    */

void myClient(int connfd ){
  char		buf[BUFLEN+1];  
  for(;;){
  // implement here
  while(fgets(buf ,BUFLEN ,stdin) ){
    printf("own:%s",buf);
    mywrite(connfd,buf);
    if(read(connfd,buf,BUFLEN) == 0){
      printf("read error"); exit(0);
    }

    printf("%sln:%d\n",buf,strlen(buf));
    memset(buf,'\0',BUFLEN);
  }
  
  }
}
