#define	BUFLEN		1024	/* maximum response size    */

void myClient(int connfd ){
   char		buf[BUFLEN+1];  
   FILE *fp;
   memset(buf,'\0',BUFLEN);
   
   // scan file name
   scanf("%s",buf);
   fp = fopen((const char*)buf,"r");
   if(fp!=NULL){
      printf("success");
   }
   
   while(fread(buf,sizeof(char),BUFLEN,fp)){
      printf("test\n");
   }

   fclose(fp);
}
