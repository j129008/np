#define	BUFLEN		1024	/* maximum response size    */

void myClient(int connfd ){
  char		buf[BUFLEN+1],filename[30] ;
  FILE*  ff;

  for(;;){
    // implement here
    memset(filename,'\0',30);
    if( !scanf("%s",filename) ){
      printf("file name error, exit\n");
      exit(0);
    }

    if( (ff=fopen(filename,"r")) <= 0 ) printf("open file:'%s'error\n",filename);
    write(connfd, filename,30);

    memset(buf,'\0',BUFLEN);
    while(fgets(buf ,BUFLEN ,ff)){
      printf("%s",buf);
      mywrite(connfd, buf);
      memset(buf,'\0',BUFLEN);
    }

    printf("file transmit finish\n");
    fclose(ff);
    // end implement
  }
}
