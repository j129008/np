#define	BUFLEN		1024	/* maximum response size    */
#include <sys/ioctl.h>

void myClient(char ** argv ){
   char		buf[BUFLEN+1]; 
   int connfd1, connfd2, connfd3;
   FILE *fp;
   int ioctlVal=0; 

   memset(buf,'\0',BUFLEN);
   // scan file name
   scanf("%s",buf);
   fp = fopen((const char*)buf,"r");
   if(fp==NULL){
      printf("open file error\n");
   }
   
   // send file name
   connfd1 = myTcpConnect(argv,atoi(argv[2]));
   rmnewline(buf);
   strcat(buf,"I");
   mywrite(connfd1,buf);
   
   connfd2 = myTcpConnect(argv,atoi(argv[3]));
   rmnewline(buf);
   strcat(buf,"I");
   mywrite(connfd2,buf);
   
   connfd3 = myTcpConnect(argv,atoi(argv[4]));
   rmnewline(buf);
   strcat(buf,"I");
   mywrite(connfd3,buf);
   
   
   // send file
   int len=0;
   while( (len = fread(buf,sizeof(char),BUFLEN,fp)) ){
      ioctlVal=1;
      ioctl(connfd1,FIONBIO,&ioctlVal);
      write(connfd1,buf,len);
      
      ioctlVal=1;
      ioctl(connfd2,FIONBIO,&ioctlVal);
      write(connfd2,buf,len);
      
      ioctlVal=1;
      ioctl(connfd3,FIONBIO,&ioctlVal);
      write(connfd3,buf,len);
   }
   fclose(fp);
}
