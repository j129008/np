void rmhead(char* str,int num){
  int i=0;
  int len=0;
  for(;i<(len=(strlen(str)-num));i++){
    str[i]=str[i+num];
  }
  str[len]='\0';
}
