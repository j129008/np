#define	BUFLEN		1024	/* maximum response size    */
#include <pthread.h>

pthread_mutex_t sendMux = PTHREAD_MUTEX_INITIALIZER;
char buf[BUFLEN+1];  
FILE *fp;
int connfd;
int sendNum=0;

static void* sendData(){
   int len=0;
   pthread_mutex_lock(&sendMux);
   while( (len = fread(buf,sizeof(char),BUFLEN,fp)) ){
      write(connfd,buf,len);
      printf("send num:%d\n",++sendNum);
   }
   pthread_mutex_unlock(&sendMux);
   return NULL;
}

void myClient(int getConnfd ){
   memset(buf,'\0',BUFLEN);
   pthread_t tidA, tidB, tidC;
   connfd=getConnfd;

   // scan file name
   scanf("%s",buf);
   fp = fopen((const char*)buf,"r");
   if(fp==NULL){
      printf("open file error\n");
   }
   mywrite(connfd,buf);
   
   pthread_create(&tidA,NULL,sendData,NULL);
   pthread_create(&tidB,NULL,sendData,NULL);
   pthread_create(&tidC,NULL,sendData,NULL);
   
   pthread_join(tidA,NULL);
   pthread_join(tidB,NULL);
   pthread_join(tidC,NULL);
   fclose(fp);
}

