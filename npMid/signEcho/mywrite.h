int mywrite(int sockfd,void * send){
  int len=strlen(send);
  if((write(sockfd, send , len))!=len)printf("write error\n");
  return len;
}
