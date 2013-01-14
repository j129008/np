#define	BUFLEN		1024	/* maximum response size    */

void myClient(int connfd ){
   char		buf[BUFLEN+1];  
   FILE *fp;
   memset(buf,'\0',BUFLEN);
   
   // scan file name
   scanf("%s",buf);
   fp = fopen((const char*)buf,"r");
   if(fp==NULL){
      printf("open file error\n");
   }
   
   // send file name
   mywrite(connfd,buf);

   // send file
   int len=0;
   while( (len = fread(buf,sizeof(char),BUFLEN,fp)) ){
      write(connfd,buf,len);
   }

   fclose(fp);
}
