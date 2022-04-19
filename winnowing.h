#ifndef __WINNOWING_H
    #define __WINNOWING_H

#include <stdint.h>    

uint8_t normalize (uint8_t byte);
uint32_t winnowing (char *src, uint32_t *hashes, uint32_t *lines, uint32_t limit);
uint32_t winnowing_off (char *src, uint32_t *hashes, uint32_t *offs, uint32_t *lines, uint32_t *line1s, uint32_t limit);
uint32_t winnowing_off_find (char *src, uint32_t h, uint32_t limit, uint32_t * off);

#endif
