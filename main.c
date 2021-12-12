#include <stdio.h>
#include <stdlib.h>

int proc_blob (char *path, int fr, int to);

int main(int argc, char *argv[]){
  //scanner_file_proc (argv[1]);
  int res = proc_blob (argv[1],atol(argv[2]),atol(argv[3]));
  if (res == -1){
    fprintf(stderr, "cant process blob: %s %s %s\n", argv[1], argv[2], argv[3]);
	return -1;
  }
  return 0;
}
