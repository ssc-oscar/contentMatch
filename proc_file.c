#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <math.h>
#include "winnowing.h"
#include "fingerprint.h"
#define MAX_FILE_SIZE (1024 * 1024 * 4)
#define MIN_FILE_SIZE 256


int proc_file (char *path){
  FILE * f = fopen (path, "rb");
  fseek (f, 0, SEEK_END);
  long len = ftell (f);
  char *cnt = malloc (len + 1);
  fseek(f, 0, SEEK_SET);
  int err = fread (cnt, 1, len, f);
  if (err != len){
    fprintf (stderr, "could not read %ld bytes, got %d\n", len, err);
	 if (cnt != NULL) free (cnt);
	 exit (-1);
}
  fclose (f);
  if (! get_fingerprints (cnt, len)){	  
    printf ("\n");
  }else{
    printf ("%s;%ld\n",path, len); // this should not happen unless file too short
    //printf ("err=%d, outLen=%lu, firstword=%x\n", err, strnlen(buffO, len*10-1), *ll);
  }
  free (cnt);
  return 0;
}

int find_hash (char *path, char *hash){
  FILE * f = fopen (path, "rb");
  fseek (f, 0, SEEK_END);
  long len = ftell (f);
  char *cnt = malloc (len + 1);
  fseek(f, 0, SEEK_SET);
  int err = fread (cnt, 1, len, f);
  if (err != len){
    fprintf (stderr, "could not read %ld bytes, got %d\n", len, err);
	 if (cnt != NULL) free (cnt);
	 exit (-1);
  }
  fclose (f);
  uint32_t off = 0;
  while (off < len){
    uint32_t fr = 0;
    uint32_t slen = find_fingerprints (cnt+off, len-off, hash, &fr);
    if (slen > 0){
      uint8_t *gram = malloc (31+1);
      uint8_t *gram2 = malloc (slen+1);
      int k = 0;
      for (int j = fr; j < fr+slen; j++){ 
        uint8_t c = cnt[j+off];
        uint8_t byte = normalize(cnt[j+off]);
        if (byte){
          if (k >= 31){
            gram [k]=0;
            fprintf (stderr, "too long %s\n",gram);
            exit (-1);
          }
          gram [k++]=byte;
        }
        c = (c =='\n' || c == '\r') ? ' ' : c; 
        gram2[j-fr] = c;
      } 
      gram2[slen] = 0;
      gram[k] = 0;
      printf ("%d;%s;%d-%d;%s\n", k, gram, fr+off, slen, gram2);
      free (gram2);
      off += fr + slen;
      free (gram);
    }else
      break;
  }
  free (cnt);
  return 0;
}



