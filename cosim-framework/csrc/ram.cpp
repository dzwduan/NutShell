#include "ram.h"
#include "difftest.h"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <pthread.h>
#include <sys/mman.h>
#include <assert.h>


static uint64_t img_size;
static uint64_t *ram;
static pthread_mutex_t ram_lock = PTHREAD_MUTEX_INITIALIZER;
uint64_t EMU_RAM_SIZE = (8 * 1024 * 1024 * 1024UL); // 8 GB

void * get_image_start() {
  return &ram[0];
}

uint64_t get_image_size() {
  return img_size;
}

// 使用mmap降低系统调用开销，不基于FILE*的文件读写
void init_ram(const char * img) {
    assert(img != NULL);
    printf("The image is %s\n", img);
    
    // initialize memory with linux mmap
    ram = (uint64_t *)mmap(NULL, EMU_RAM_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ram == (uint64_t *)MAP_FAILED) {
        printf("Warning: Insufficient physical memory\n");
        EMU_RAM_SIZE = 128 * 1024 * 1024UL;
        ram = (uint64_t *)mmap(NULL, EMU_RAM_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (ram == (uint64_t *)MAP_FAILED) {
            printf("Error: Failed to allocate 0x%lx bytes memory\n", EMU_RAM_SIZE);
            exit(1);
        }
    }
    
    printf("Using simulated %luMB RAM\n", EMU_RAM_SIZE / (1024 * 1024));
    int ret;

    FILE *fp = fopen(img, "rb");
    if (fp == NULL) {
        printf("Error: Failed to open file %s\n", img);
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    img_size = ftell(fp);
    if (img_size > EMU_RAM_SIZE) {
        printf("Error: Image size 0x%lx is larger than RAM size 0x%lx\n", img_size, EMU_RAM_SIZE);
        exit(1);
    }

    fseek(fp, 0, SEEK_SET);
    ret = fread(ram, 1, img_size, fp);

    assert (ret == 1);
    fclose(fp);

    pthread_mutex_init(&ram_lock, 0);
}


/**
 * @brief unmap and destroy the lock
 * 
 */
void ram_finish(){
    munmap(ram, EMU_RAM_SIZE);
    pthread_mutex_destroy(&ram_lock);
}


/**
 * @brief read from ram
 * 
 */
extern "C" inline uint64_t ram_read_helper(uint8_t en, uint64_t rIdx) {
    if (!ram) return 0 ;
    rIdx %= EMU_RAM_SIZE / sizeof(uint64_t);
    uint64_t rdata = (en) ? ram[rIdx] : 0;
    return rdata;
}

/**
 * @brief write to ram, check the address and write enable
 * 
 */
extern "C" inline void ram_write_helper(uint64_t wIdx, uint64_t wdata, uint64_t wmask, uint8_t wen) {
    assert(ram);
    wIdx %= EMU_RAM_SIZE / sizeof(uint64_t);
    assert (wIdx < EMU_RAM_SIZE / sizeof(uint64_t));
    if (wen) {
        ram[wIdx] = (ram[wIdx] & ~wmask) | (wdata & wmask);
    }
}

/**
 * @brief physical memory read, pmem start from 0x8000_0000
 * 
 * @param raddr 
 * @return uint64_t 
 */
inline uint64_t pmem_read(uint64_t raddr) {
    assert (raddr > 0x80000000);
    assert (raddr < 0x80000000 + EMU_RAM_SIZE);
    assert (raddr % sizeof(uint64_t) == 0);

    raddr -= 0x80000000;
    return ram_read_helper(1, raddr / sizeof(uint64_t));
}

/**
 * @brief physical memory write, pmem start from 0x8000_0000
 * 
 * @param waddr 
 * @param wdata 
 * @return uint64_t 
 */
inline void pmem_write(uint64_t waddr, uint64_t wdata) {
    assert (waddr > 0x80000000);
    assert (waddr < 0x80000000 + EMU_RAM_SIZE);
    assert (waddr % sizeof(uint64_t) == 0);

    waddr -= 0x80000000;
    ram_write_helper(waddr / sizeof(uint64_t), wdata, -1UL, 1);
}