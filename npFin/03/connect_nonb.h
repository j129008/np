#include <errno.h>
#include <fcntl.h>

int connect_nonb(int sockfd, const struct sockaddr* saptr, socklen_t salen, int nsec ){
   int flags, n , error;
   socklen_t len;
   fd_set rset, wset;
   struct timeval tval;
   
   // get flag & set nonblock
   flags=fcntl( sockfd, F_GETFL, 0 );
   fcntl( sockfd, F_SETFL, flags | O_NONBLOCK );  // use "or" operation
   
   // EINPROGRESS: still building connection
   error=0;
   if( (n=connect(sockfd, saptr, salen))<0 ){
      if(errno != EINPROGRESS)return (-1);
   }

   if(n==0)goto done;

   FD_ZERO(&rset);
   FD_SET(sockfd, &rset);
   wset = rset;
   tval.tv_sec = nsec;
   tval.tv_usec = 0;

   if( (n=select(sockfd+1, &rset, &wset, NULL, nsec?&tval:NULL)) == 0 ){
      close(sockfd);
      errno=ETIMEDOUT;
      return (-1);
   }

   if(FD_ISSET(sockfd,&rset) || FD_ISSET(sockfd, &wset)){
      len = sizeof(error);
      if(getsockopt(sockfd, SOL_SOCKET,SO_ERROR, &error,&len) < 0){
         return (-1);
      }
   }else{
      perror("select error: sockfd not set");
   }
done:
   fcntl(sockfd, F_SETFL,flags);  // restore file status
   if(error){
      close(sockfd);
      errno=error;
      return (-1);
   }
   return 0;
}
