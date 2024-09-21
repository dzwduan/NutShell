#ifndef __RAM_H__
#define __RAM_H__

#include "common.h"
#include "difftest.h"
#include <cstdint>



void * get_image_start();
uint64_t get_image_size();

void init_ram(const char * img);
void addPageSv39(); //TODO:

inline uint64_t pmem_read(uint64_t raddr);
inline void pmem_write(paddr_t addr, uint64_t data);



#endif