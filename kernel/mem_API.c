#include "mem_API.h"

extern Bitmap bitmap; // Declaração do bitmap para uso nas funções da API
extern page_directory_t *vmm_get_kernel_directory(void); //Declaração

// funções da API
static void list_add(buddy_block_t **list_head, buddy_block_t *block) {
    block->next = *list_head;
    block->prev = NULL;
    if (*list_head != NULL) {
        (*list_head)->prev = block;
    }
    *list_head = block;
}
static void list_remove(buddy_block_t **list_head, buddy_block_t *block) {
    if (block->prev != NULL) {
        block->prev->next = block->next;
    } else {
        *list_head = block->next; // Era o primeiro da lista
    }
    if (block->next != NULL) {
        block->next->prev = block->prev;
    }
    
    block->next = NULL;
    block->prev = NULL;
}
void kmalloc_init(void) {
    page_directory_t *kernel_dir = vmm_get_kernel_directory();
    uint32_t num_pages = KERNEL_HEAP_SIZE / PAGE_SIZE;
    //Pede páginas físicas ao PMM e mapeia contiguamente no Virtual
    for (uint32_t i = 0; i < num_pages; i++) {
        void *phys_addr = pmm_alloc_block();  
        if (phys_addr == NULL) {
            serial_puts("[PANIC] Sem memoria fisica para o kmalloc_init!\n");
            while(1);
        }
        uintptr_t virt_addr = KERNEL_HEAP_START + (i * PAGE_SIZE); 
        // Mapeia na VMM
        vmm_map_page(kernel_dir, virt_addr, (uintptr_t)phys_addr, VMM_PAGE_PRESENT | VMM_PAGE_WRITABLE);
        
        //Transforma a página mapeada em um bloco livre de 4KB
        buddy_block_t *block = (buddy_block_t *)virt_addr;
        block->size = SIZE_4K;
        block->is_free = 1;
        //Adiciona à lista de 4KB
        list_add(&free_list_4k, block);
    }
}
uintptr_t kmalloc(uint32_t size) {
    if (size == 0) return 0;
    // Tamanho total necessário = Dados do usuário + Cabeçalho
    uint32_t total_size = size + sizeof(buddy_block_t);
    buddy_block_t *allocated_block = NULL;
    // Caso 1: Cabe em 2KB
    if (total_size <= SIZE_2K) {
        if (free_list_2k != NULL) {
            // Pega direto da lista de 2KB
            allocated_block = free_list_2k;
            list_remove(&free_list_2k, allocated_block);
        } 
        else if (free_list_4k != NULL) {
            // Pega de 4KB e RACHA NO MEIO
            buddy_block_t *big_block = free_list_4k;
            list_remove(&free_list_4k, big_block);

            // A primeira metade é o bloco que vamos usar
            allocated_block = big_block;
            allocated_block->size = SIZE_2K;
            // A segunda metade vira um novo bloco livre de 2KB
            buddy_block_t *split_buddy = (buddy_block_t *)((uintptr_t)big_block + SIZE_2K);
            split_buddy->size = SIZE_2K;
            split_buddy->is_free = 1;
            // Coloca a metade livre na lista correspondente
            list_add(&free_list_2k, split_buddy);
        }
    } 
    // Caso 2: Precisa de 4KB
    else if (total_size <= SIZE_4K) {
        if (free_list_4k != NULL) {
            allocated_block = free_list_4k;
            list_remove(&free_list_4k, allocated_block);
        }
    }
    // Se não achou bloco, ou se pediram mais de 4KB (não suportado por este Micro-Buddy)
    if (allocated_block == NULL) {
        serial_puts("[ERROR] kmalloc: Memoria insuficiente ou pedido muito grande!\n");
        return 0;
    }

    // Marca como ocupado e retorna o ponteiro LOGO APÓS o cabeçalho
    allocated_block->is_free = 0;
    return (uintptr_t)allocated_block + sizeof(buddy_block_t);
}
uint8_t kfree(uintptr_t addr) {
    if (addr == 0) return 0;
    // Recua o ponteiro para ler o cabeçalho invisível
    buddy_block_t *block = (buddy_block_t *)(addr - sizeof(buddy_block_t));
    block->is_free = 1;
    //Se o bloco é de 4KB, volta direto pra lista
    if (block->size == SIZE_4K) {
        list_add(&free_list_4k, block);
        return 1;
    }
    if (block->size == SIZE_2K) {
        uintptr_t buddy_addr = (uintptr_t)block ^ SIZE_2K;
        buddy_block_t *buddy = (buddy_block_t *)buddy_addr;
        if (buddy->is_free == 1 && buddy->size == SIZE_2K) {
            list_remove(&free_list_2k, buddy);
            buddy_block_t *merged_block = (buddy_block_t *)((uintptr_t)block & ~2047);       
            merged_block->size = SIZE_4K;
            merged_block->is_free = 1;
            list_add(&free_list_4k, merged_block);
        } else {
            list_add(&free_list_2k, block);
        }
        return 1;
    }
    return 0;
}