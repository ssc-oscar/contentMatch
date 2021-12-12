#include <stdio.h>
#include <stdlib.h>

int proc_blob (char *path, int fr, int to);

int main(int argc, char *argv[]){
  //scanner_file_proc (argv[1]);
  proc_blob (argv[1],atol(argv[2]),atol(argv[3]));
  return 0;
}
