#define BUFLEN     1024

void myServer( int listenfd ){
   char    buf[BUFLEN+1];
   FILE* fp;
   while(1){  
      int connfd = accept(listenfd,(SA*)NULL,NULL);
      if(-1==connfd) err_quit("accept error");
      // get file name
      memset(buf,'\0',BUFLEN);
      read(connfd, buf, BUFLEN);
      fp = fopen(buf,"w");
      // write file
      int len=0;
      while( (len=read(connfd,buf,BUFLEN)) ){
         fwrite(buf,sizeof(char), len,fp);
         printf("get data\n");
      }
      // end
      printf("send file end\n");

      fclose(fp);
   }
}

