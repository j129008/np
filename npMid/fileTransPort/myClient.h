#define	BUFLEN		1024	/* maximum response size    */

void myClient(int connfd ){
  char		buf[BUFLEN+1];  
  FILE* fp;
  for(;;){
    bzero(buf,sizeof(buf));
    fgets(buf, BUFLEN, stdin);
    printf("*****%s\n",buf);
    write(connfd, buf, strlen(buf));
    rmnewline(buf);
    printf("%s",buf);
    fp=fopen(buf,"r");
    printf("-----%d\n",fp);
    
    while (fgets(buf, BUFLEN, fp)) {
      if(fp<=0){
        printf("NULL file\n");
        continue;
      }
      printf("=====%d\n",fp);
      mywrite(connfd,buf);
      printf("now=====%s\n",buf);
      bzero(buf,sizeof(buf));
    }
    close(connfd);
    exit(0);
  }
  fclose(fp);

  /*
     memset(buf,'\0',BUFLEN);
     while (read(connfd, buf , BUFLEN )) {}
     close(connfd);
     printf("Time: %s\n", buf);
   */
}
