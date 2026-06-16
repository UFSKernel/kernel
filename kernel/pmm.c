// Gerencia o array de bits (Bitmap)
#include "pmm.h"

 extern uint8_t end[]; // Símbolo vindo do linker script, representa o final do kernel

Bitmap bitmap;

void pmm_init() {
    bitmap.map = (uint32_t *) end; //tem que alinhar aqui

    uint32_t num_pages  = RAM_SIZE / PAGE_SIZE;
    bitmap.size = num_pages / 32;
    if(num_pages % 32 != 0) {
        bitmap.size++; //Adiciona um bloco extra se não for múltiplo de 32
    }
 
    for (uint32_t i = 0; i < bitmap.size; i++) {
        bitmap.map[i] = 0xFFFFFFFF; //para evitar problemas de inicialização, todos os blocos são marcados como alocados inicialmente (bits setados para 1)
    }

    uintptr_t free_mem_start = (uintptr_t) end + bitmap.size * sizeof(uint32_t);
    
    if (free_mem_start % PAGE_SIZE != 0) {
        free_mem_start = ((free_mem_start / PAGE_SIZE) + 1) * PAGE_SIZE;
    }

    for(uintptr_t addr = free_mem_start; addr < RAM_END; addr += PAGE_SIZE) {
        pmm_free_page(addr); //marca os blocos de memória livre a partir do final do kernel até o final da RAM (bits setados para 0)
    }

}


void pmm_free_page(uintptr_t addr) {
    uint32_t page = (addr - RAM_START) / PAGE_SIZE;
    uint32_t idx = page / 32;
    uint32_t bit = page % 32;

    uint32_t mask = (1 << bit); //cria uma mascara com bit - Ex: para o bit 2, a mascara seria ...00000100, para o bit 1 seria ...00000010
    mask = ~mask; // inverte a máscara para que o bit correspondente seja 0 e os outros sejam 1 (ex ...00100 -> ...11011)
    bitmap.map[idx] &= mask; // faz uma operação and pra zerar o bit e marcar o bloco como livre
}

void pmm_alloc_page_at(uintptr_t addr) {
    uint32_t page = (addr - RAM_START) / PAGE_SIZE;
    uint32_t idx = page / 32;
    uint32_t bit = page % 32;
    
    uint32_t mask = (1 << bit); //cria uma mascara com bit - Ex: para o bit 2, a mascara seria ...00000100, para o bit 1 seria ...00000010
    bitmap.map[idx] |= mask; // faz uma operação de or para setar o bit da mask pra 1
}

void* pmm_alloc_block() {
    //Percorre o bitmap procurando por um índice que não esteja totalmente cheio (diferente de 0xFFFFFFFF)
    for (uint32_t i = 0; i < bitmap.size; i++) {
        if (bitmap.map[i] != 0xFFFFFFFF) {
            
            //Achou um mapa com espaço. Agora acha qual bit está em 0
            for (int bit = 0; bit < 32; bit++) {
                uint32_t mask = (1 << bit);
                
                if (!(bitmap.map[i] & mask)) { // Se o bit for 0 (bloco livre)
                    bitmap.map[i] |= mask;     // Seta o bit para 1 (marca como alocado)
                    
                    //Calcula o endereço físico real baseado no índice e no bit
                    uint32_t page_frame_number = (i * 32) + bit;
                    uintptr_t phys_addr = RAM_START + page_frame_number * PAGE_SIZE;
                    
                    return (void*)phys_addr; // Retorna o endereço físico do bloco alocado
                }
            }
        }
    }
    return NULL; // Sem memória física livre!
}
