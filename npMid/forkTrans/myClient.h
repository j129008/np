#define	BUFLEN		1024	/* maximum response size    */

void myClient(int connfd ){
  char	buf[BUFLEN+1];  
  char	filename[30] ;
  int i;
  FILE*  ff;
  
  for(;;){
    // implement here
    memset(filename,'\0',30);
    if( !scanf("%s",filename) ){
      printf("file name error, exit\n");
      exit(0);
    }

    if( (ff=fopen(filename,"r")) <= 0 ) printf("open file:'%s'error\n",filename);
    mywrite(connfd, filename);

    while(fgets(buf ,BUFLEN ,ff)){
      printf("own:%s",buf);
      mywrite(connfd, buf);
    }

    printf("file transmit finish\n");
    fclose(ff);
  }
}
