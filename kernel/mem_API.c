#include "mem_API.h"

// funções da API

uintptr_t kmalloc(uint32_t size) {
    // Ignora o 'size' desde que não passe de 4KB, e devolve 1 página inteira limpa
    if (size == 0 || size > PAGE_SIZE) return 0; //Por questões de simplicidade, atualmente o kmalloc faz uma simples alocação de uma única página física de 4KB,
                                                 // por conta de como foi feito o mapeamento da memória virtual em VMM.C, não acontecerá exceções de mapeamento
    return (uintptr_t)pmm_alloc_block(); 
}

uint8_t kfree(uintptr_t addr) {
    if (addr == 0) return 0;
    
    // Como sabemos que tudo tem 4KB, só devolvemos pro PMM direto
    pmm_free_page(addr); 
    return 1;
}
