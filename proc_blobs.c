#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <math.h>
#include "winnowing.h"

#define MAX_FILE_SIZE (1024 * 1024 * 4)
#define MIN_FILE_SIZE 256


unsigned int lzf_decompress (const void *const in_data,  unsigned int in_len,
			     void             *out_data, unsigned int out_len);

int get_fingerprints (char *src, long length){
  if (strlen(src) == length && length < MAX_FILE_SIZE){
    uint32_t *hashes = malloc(MAX_FILE_SIZE);
    uint32_t *lines = malloc(MAX_FILE_SIZE);
    uint32_t last_line = 0;

    uint32_t size = winnowing (src, hashes, lines, MAX_FILE_SIZE);
    // printf ("%ld, %u, %u %u %u;", length, size, lines[0], lines[1], last_line);
    
    for (int i = 0; i < size; i++){
      if (last_line != lines[i]){
	if (last_line != 0) printf(";");
	//fprintf (stderr, "%ld, %u, %u %u %u\n", length, size, lines[0], lines[1], last_line);
	printf("%d=%08x", lines[i], hashes[i]);
	last_line = lines[i];
      }else
	printf(",%08x", hashes[i]);
    }
    printf(";");
    free(hashes);
    free(lines); 
    return 0;
  }else{
    // printf ("length=%ld, actual=%ld, %d;", length, ll, MAX_FILE_SIZE);
    return 1;
  } 
}

int proc_blob (char *path, int fr, int to){
  char pi [255];
  char * line = NULL;
  size_t len = 0;

  //sprintf(pi, "%s.idx", path);
  FILE * fi = stdin;//fopen(pi, "rb");
  sprintf(pi, "%s.bin", path);
  FILE * fb = fopen(pi, "rb");
  for (int i = 0; i < fr; i++){
    line = NULL;
    getline (&line, &len, fi);
    free(line);
  }
  for (int i = fr; i < to; i++){
    line = NULL;
    getline (&line, &len, fi);
    char * cp = strdup (line);
    char * nn = strtok (cp, ";");
    if (atoi (nn) != i) return 0;
    char * off = strtok (NULL, ";");
    char * sz = strtok (NULL, ";");
    char * sha1 = strtok (NULL, ";");
    char * nm = strtok (NULL, ";");
    nm [strlen(nm)-1] = 0;
    fseek (fb, atol(off), SEEK_SET);
    unsigned len = atoi(sz);
    if (len < MAX_FILE_SIZE){
      char * buff = malloc (len);
      char * buffO = malloc (len*10);
      int err = fread (buff, atoi(sz), 1, fb);
      //unsigned short * ll = (unsigned short *) (buff +1);
      //printf ("%d %u %x\n", err, len, *ll);
      err = lzf_decompress (buff+3, len-3, buffO, len*10);
      if (err > 0){
        printf ("%s;%d;%s;%s;%s;%s;%d;", nn, atoi (nn), off, sz, sha1, nm, err);
        buffO[err] = 0;
	if (! get_fingerprints (buffO, err)){	  
          printf ("\n");
	}else printf ("\n"); // this should not happen unless file too short
	//printf ("err=%d, outLen=%lu, firstword=%x\n", err, strnlen(buffO, len*10-1), *ll);
      }
      free (buffO);
      free (buff);
    }
    free (cp);
    free (line);
  }
  return 0;
}



