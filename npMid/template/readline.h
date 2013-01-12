int readline(int fd, char* ptr, int maxlen){
  ssize_t n, rc;
  char c;
  for(n = 1; n < maxlen; n++){
    if( (rc = read(fd, &c, 1)) == 1 ){
      *ptr++ = c;
      if(c == '\n')
        break;
    }
    else if(rc == 0){
      *ptr = 0;
      return n-1;
    }
    else
      return -1;
  }
  *ptr = 0;

  return n;
}

