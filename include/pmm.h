// Physical Memory Manager - Protótipos das funções

#ifndef PMM_H
#define PMM_H

#include <stdint.h>

// Inicializa o gerenciador com a base da RAM e seu tamanho total
void pmm_init(uint32_t start_addr, uint32_t size);

// Aloca um bloco de 4KB e retorna seu endereço físico
void* pmm_alloc_block();

// Libera um bloco previamente alocado
void pmm_free_block(void* addr);

#endif

