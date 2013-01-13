
void rmnewline(char* x){
  int i;	
  for(i=0;i<strlen(x);i++)if(x[i]=='\n'||x[i]=='\r')x[i]='\0';
}
