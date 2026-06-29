#ifndef VMM_H
#define VMM_H

#include "pmm.h"


// Definições específicas da MMU ARM (Short-Descriptor Format)
#define NUM_FIRST_LEVEL_ENTRIES  4096   // 4GB / 1MB = 4096 entradas
#define NUM_SECOND_LEVEL_ENTRIES 256    // 1MB / 4KB = 256 entradas

// Atributos de configuração de página (Flags clássicas da MMU)
#define VMM_PAGE_PRESENT   (1 << 0)
#define VMM_PAGE_WRITABLE  (1 << 1)
#define VMM_PAGE_USER      (1 << 2)
#define VMM_PAGE_NOCACHE   (1 << 3)

// Tipos para clareza de código
typedef uint32_t pd_entry_t; // Entrada do diretório de páginas (L1)
typedef uint32_t pt_entry_t; // Entrada da tabela de páginas (L2)

// Alinha as tabelas conforme exigido pelo hardware ARM, L1 para 16KB (14 bits zerados) e o L2 paraa 1KB (10 bits zerados).
// A MMU usa esses bits zerados dos ponteiros para armazenar
// as flags de hardware (como cache e permissões) sem gastar registradores extras.
typedef struct {
    pd_entry_t entries[NUM_FIRST_LEVEL_ENTRIES] __attribute__((aligned(16 * 1024)));
} page_directory_t;

typedef struct {
    pt_entry_t entries[NUM_SECOND_LEVEL_ENTRIES] __attribute__((aligned(1024)));
} page_table_t;

/* --- API Principal da VMM --- */

// Inicializa a paginação do kernel, configurando o diretório de páginas base
void vmm_init(void);

// Ativa a MMU modificando os registradores de controle do sistema (CP15 no ARM)
void vmm_enable(void);

// Cria um mapeamento entre um endereço virtual e um físico
int vmm_map_page(page_directory_t *dir, uintptr_t virtual_addr, uintptr_t physical_addr, uint32_t flags);

// Remove um mapeamento existente, não implementada ainda
void vmm_unmap_page(page_directory_t *dir, uintptr_t virtual_addr);

// Troca o diretório de páginas atual (usado na troca de contexto entre processos)
void vmm_switch_directory(page_directory_t *dir);

page_directory_t *vmm_get_kernel_directory(void); //para testes apenas
#endif  


