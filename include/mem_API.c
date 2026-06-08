#include "mem_API.h"

extern Bitmap bitmap; // Declaração do bitmap para uso nas funções da API

// funções da API
uintptr_t kmalloc(uint32_t size){ //Função equivalente ao malloc.
    uintptr_t addr;
    uint32_t num_pages = (size + PAGE_SIZE - 1) / PAGE_SIZE; //calcula o número de páginas necessárias para alocar o tamanho solicitado

    for (uint32_t i = 0; i < bitmap.size; i++) { //percorre o bitmap procurando por um bloco livre (bit 0)
        if(bitmap.map[i] == 0xFFFFFFFF) { // se o bloco estiver cheio, passa pro próximo
            continue;
        }
    }


    return addr;// retorna o endereço do primeiro bloco alocado
}
uint8_t kfree(uintptr_t addr){
    pmm_free_block(addr);
    
    return 1;
}