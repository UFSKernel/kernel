#ifndef MEM_API_H
#define MEM_API_H

#include "pmm.h"
#include "VMM.h"

#define KERNEL_HEAP_START 0x10000000
#define KERNEL_HEAP_SIZE  (1024 * 1024)
#define SIZE_2K 2048
#define SIZE_4K 4096

typedef struct buddy_block {
    uint16_t size;
    uint8_t is_free;
    struct buddy_block *next;
    struct buddy_block *prev;
} buddy_block_t;

static buddy_block_t *free_list_2k = NULL;
static buddy_block_t *free_list_4k = NULL;

void kmalloc_init(void);
uintptr_t kmalloc(uint32_t size);
uint8_t kfree(uintptr_t addr);

#endif