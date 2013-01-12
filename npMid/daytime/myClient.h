#define	BUFLEN		1024	/* maximum response size    */

void myClient(int connfd ){
  char		buf[BUFLEN+1];  
  memset(buf,'\0',BUFLEN);
  while (read(connfd, buf , BUFLEN )) {}
  close(connfd);
  printf("Time: %s\n", buf);

}
