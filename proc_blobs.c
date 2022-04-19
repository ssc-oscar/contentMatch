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
    uint32_t last_line = -1;

    uint32_t size = winnowing (src, hashes, lines, MAX_FILE_SIZE);
    
    for (int i = 0; i < size; i++){
      if (last_line != lines[i]){
        printf(";%d=%08x", lines[i], hashes[i]);
        last_line = lines[i];
      }else
	      printf(",%08x", hashes[i]);
    }
    printf(";");
    free(hashes);
    free(lines); 
    return 0;
  }else{
    fprintf (stderr, "length=%ld, actual=%ld, %d;", strlen(src), length, MAX_FILE_SIZE);
    return 1;
  } 
}

uint32_t find_fingerprints (char *src, long length, char* hash, uint32_t * off){
  uint32_t size = 0;
  if (strlen(src) == length && length < MAX_FILE_SIZE){
    uint32_t h;
    sscanf(hash,"%x", &h);
    // fprintf (stderr, "%s=%.8x\n", hash, h);
    size = winnowing_off_find (src, h, MAX_FILE_SIZE, off);
  }
  return size;
}


int get_fingerprints_off (char *src, long length){
  if (strlen(src) == length && length < MAX_FILE_SIZE){
    uint32_t *hashes = malloc(MAX_FILE_SIZE);
    uint32_t *offs = malloc(MAX_FILE_SIZE);
    uint32_t *lines = malloc(MAX_FILE_SIZE);
    uint32_t *line1s = malloc(MAX_FILE_SIZE);
    uint32_t last_line = 0;

    uint32_t size = winnowing_off (src, hashes, offs, lines, line1s, MAX_FILE_SIZE);
    // printf ("%ld, %u, %u %u %u;", length, size, lines[0], lines[1], last_line);
    
    for (int i = 0; i < size; i++){
      if (last_line != lines[i]){
        if (last_line != 0) printf(";");
        // fprintf (stderr, "%ld, %u, %u %u %u\n", length, size, lines[0], lines[1], last_line);
        printf("%d-%d-%d=%08x", line1s[i]-93+offs[i], lines[i], line1s[i], hashes[i]);
        last_line = lines[i];
      }else
	      printf(",%08x", hashes[i]);
    }
    printf(";");
    free(hashes);
    free(offs);
    free(lines); 
    free(line1s); 
    return 0;
  }else{
    fprintf (stderr, "length=%ld, actual=%ld, %d\n", strlen(src), length, MAX_FILE_SIZE);
    return 1;
  } 
}


int proc_blob (char *path, long fr, long to){
  char pi [255];
  char * line = NULL;
  size_t len = 0;

  //sprintf(pi, "%s.idx", path);
  FILE * fi = stdin;//fopen(pi, "rb");
  sprintf(pi, "%s.bin", path);
  FILE * fb = fopen(pi, "rb");
  if (fr > 0){
    for (int i = 0; i < fr; i++){
      line = NULL;
      int res = getline (&line, &len, fi);
      if (res == -1){
        fprintf (stderr, "could not read line %d\n", i);
	return (-1);
      }
      free(line);
    }
  }
  long diff = -1;
  if (to > 0){
    if (fr > 0) diff = to-fr + 1;
    else diff = to;
  }else{
    diff = -1;
  }
  long i = 0;
  while ((i < diff || diff < 0) && !feof (fi)){  
    i++;
    line = NULL;
    int res = getline (&line, &len, fi);
    if (res == -1){
      fprintf (stderr, "could not read line %ld\n", i);
      return (-1);
    }
    char * cp = strdup (line);
    char * nn = strtok (cp, ";");
    // if (atoi (nn) != i) return 0;
    char * offf = strtok (NULL, ";");
    char * sz = strtok (NULL, ";");
    char * sha1 = strtok (NULL, ";");
    char * nm = strtok (NULL, ";");
    nm [strlen(nm)-1] = 0;
    fseek (fb, atol(offf), SEEK_SET);// check if atol works for large numbers
    unsigned len = atoi(sz);
    if (len < MAX_FILE_SIZE){
      char * buff = malloc (len);
      char * buffO;
      int err = fread (buff, 6, 1, fb);
      unsigned char mask = *buff;
      int len1 = mask & 127;
      int off = 1;
      if (mask&0x80){
        int k;
        for (k = 1; k < 5; k++){
          if (!((mask >> (7-k))&1)) break;
        }
        off = k;
      }
      if (off > 1){
        unsigned char msk = 0b11111;
        msk = msk >> (off-2);
        len1 = mask & msk;
        for (int k = 1; k < off; k++){
          mask = *(buff+k);
          len1 = len1*64 + (mask&0x3f);
        }
      }
      // read in the rest of the buffer
      // printf ("c=%x off=%d len1=%d fr=%ld to=%ld\n", mask, off, len1, fr, to);
      //if (off != 3 && len1 > 80 && len1 < MAX_FILE_SIZE){// got off 3 already, re-add for future
      if (len1 > 80 && len1 < MAX_FILE_SIZE){
        err = fread (buff+6, len-6, 1, fb);
        mask = *buff;
        if (!mask){
          buffO = buff+1;
          len1 = strlen(buff+1);
          err = len1;
        }else{
          buffO = malloc (len1+10);
          if (buffO == NULL){
            fprintf (stderr, "buffO==NULL len=%u\n", len1+10);
            exit (-1);
          }
        }
        err = lzf_decompress (buff+off, len-off, buffO, len1+1);
        if (err > 0 && len1 < MAX_FILE_SIZE){
          printf ("%s;%d;%s;%s;%s;%s;%d", nn, atoi (nn), offf, sz, sha1, nm, err);
          buffO[err] = 0;
          if (! get_fingerprints (buffO, err)){	  
            printf ("\n");
          }else
            printf ("\n"); // this should not happen unless file too short
          //printf ("err=%d, outLen=%lu, firstword=%x\n", err, strnlen(buffO, len*10-1), *ll);
        }else
          fprintf (stderr, "\ncould not decompress %s %lu %s %d %d\n", path, atol(offf), sz, err, len1);
        if (buff+1 != buffO) free (buffO);
      }
      free (buff);
    }
    free (cp);
    free (line);
  }
  return 0;
}


int proc_one_blob (char *path, unsigned long offf, int sz){
  char pi [255];

  sprintf(pi, "%s.bin", path);
  FILE * fb = fopen(pi, "rb");
  fseek (fb, offf, SEEK_SET);
  // printf (";in1=%lu;",off);
  unsigned len = sz;
  char * buff = malloc (len);
  char * buffO;
  int err = fread (buff, 6, 1, fb);
  unsigned char mask = *buff;
  int len1 = mask & 127;
  int off = 1;
  if (mask&0x80){
    int k;
    for (k = 1; k < 5; k++){
      if (!((mask >> (7-k))&1)) break;
    }
    off = k;
  }
  if (off > 1){
    unsigned char msk = 0b11111;
    msk = msk >> (off-2);
    len1 = mask & msk;
    for (int k = 1; k < off; k++){
      mask = *(buff+k);
      len1 = len1*64 + (mask&0x3f);
    }
  }
  // read in the rest of the buffer
  // printf ("c=%x off=%d len1=%d fr=%ld to=%ld\n", mask, off, len1, fr, to);
  //if (off != 3 && len1 > 80 && len1 < MAX_FILE_SIZE){// got off 3 already, re-add for future
  if (len1 > 80 && len1 < MAX_FILE_SIZE){
    err = fread (buff+6, len-6, 1, fb);
    mask = *buff;
    if (!mask){
      buffO = buff+1;
      len1 = strlen(buff+1);
      err = len1;
    }else{
      buffO = malloc (len1+10);
      if (buffO == NULL){
        fprintf (stderr, "buffO==NULL len=%u\n", len1+10);
        exit (-1);
      }
    }
    err = lzf_decompress (buff+off, len-off, buffO, len1+1);
    if (err > 0 && len1 < MAX_FILE_SIZE){
      //printf ("%s;%s;%s;%s;%d", offf, sz, sha1, nm, err);
      buffO[err] = 0;
      if (! get_fingerprints (buffO, err)){	  
        printf ("\n");
      }else
        printf ("\n"); // this should not happen unless file too short
        //printf ("err=%d, outLen=%lu, firstword=%x\n", err, strnlen(buffO, len*10-1), *ll);
    }else
      fprintf (stderr, "\ncould not decompress %s %lu %d %d %d\n", path, offf, sz, err, len1);
    if (buff+1 != buffO) free (buffO);
  }
  free (buff);
  return 0;
}



