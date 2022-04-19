#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int proc_blob (char *path, long fr, long to);
int proc_one_blob (char *path, unsigned long fr, int to);
int proc_file (char *path);
int find_hash (char *path, char * hash);
int main(int argc, char *argv[]){
  if (argc == 2){
    int res = proc_file (argv[1]);
    if (res == -1){ 
      fprintf(stderr, "cant process file %s\n", argv[1]);
      return -1;
    }
    return 0;
  }

  if (argc == 3){
    int res = find_hash (argv[1], argv[2]);
    if (res == -1){
      fprintf(stderr, "cant process file %s\n", argv[1]);
      return -1;
    }
    return 0;
  }
  // printf (":%s:%lu\n", argv[0]+ strlen(argv[0])-4, strlen(argv[0]));
  if (argc == 4 && !strcmp(argv[0] + strlen(argv[0])-3, "off")){
    char *ptr;
    unsigned long off = strtoul(argv[2],&ptr,10);
    // printf("\n%s:%lu", argv[2], off);
    int res = proc_one_blob (argv[1], off, atoi(argv[3]));
    if (res == -1){
      fprintf(stderr, "cant process one blob: %s %s %s\n", argv[1], argv[2], argv[3]);
      return -1;
    }
    return 0;
  }
  int res = proc_blob (argv[1],atol(argv[2]),atol(argv[3]));
  if (res == -1){
    fprintf(stderr, "cant process blob: %s %s %s\n", argv[1], argv[2], argv[3]);
	  return -1;
  }
  return 0;
}

