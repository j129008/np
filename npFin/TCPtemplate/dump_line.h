void dump_line(FILE * fp) {
  int ch;
  while ((ch = fgetc(fp)) != EOF && ch != '\n') {
    /* null body */;
  }
}
