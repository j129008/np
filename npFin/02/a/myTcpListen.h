#define SA struct sockaddr
int myTcpListen(int wPort){
  int     listenfd;
  struct  sockaddr_in servaddr;
  if(-1==(listenfd = socket(AF_INET,SOCK_STREAM,0))) err_quit("socket error");
  memset(&servaddr,0,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(wPort);
  if(-1==bind(listenfd,(SA*)&servaddr,sizeof(servaddr))) err_quit("bind error");
  if(-1==listen(listenfd,5)) err_quit("listen error");
  printf("server listening on port %u\n",wPort);
  return listenfd;
}
