#define BUFLEN     1024

void myServer( int listenfd ){
   char    buf[BUFLEN+1];
   FILE* fp;
   
   int connfd = accept(listenfd,(SA*)NULL,NULL);
   if(-1==connfd) err_quit("accept error");

   // get file name
   read(connfd, buf, BUFLEN);
   printf("file name:%s\n",buf);
   fp = fopen(buf,"w");
   
   // write file
   int len=0;
   while( (len=read(connfd,buf,BUFLEN)) ){
      fwrite(buf,sizeof(char), len,fp);
   }

   // end
   printf("send file end\n");
   
   fclose(fp);
}
 
