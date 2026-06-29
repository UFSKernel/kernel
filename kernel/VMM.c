#include "VMM.h"
#include <stddef.h>

// O diretório de páginas principal do próprio kernel
// A arquitetura define 16kb então fazer a alocação estática é mais fácil
static page_directory_t kernel_directory_static __attribute__((aligned(16 * 1024))); //Alinhar a 16KB seria deixar os últimos 14 bits com 0, ou seja, os únicos
                                                                                     //finais de endereço possíveis são 0000,4000,8000 e C000 (Em Hexadecimal)

void vmm_init(void) {
    page_directory_t *kernel_directory = &kernel_directory_static;
    if(kernel_directory == NULL) {
        serial_puts("Falha ao alocar diretório de páginas para o kernel!\n");
        while(1); 
    }
    for (int i = 0; i < NUM_FIRST_LEVEL_ENTRIES; i++) {
        kernel_directory->entries[i] = 0;
    }
    //Identity Mapping (Mapear Virtual == Físico) para regiões críticas, impede falha de execução logo após ativar a memória virtual
    vmm_map_page(kernel_directory, UART0_ADDRESS, UART0_ADDRESS, VMM_PAGE_PRESENT | VMM_PAGE_WRITABLE | VMM_PAGE_NOCACHE);
    
    //Mapeando a RAM onde o kernel reside
    for (uintptr_t addr = RAM_START; addr < RAM_END; addr += PAGE_SIZE) {
        vmm_map_page(kernel_directory, addr, addr, VMM_PAGE_PRESENT | VMM_PAGE_WRITABLE);
    }
    //Carrega o diretório do kernel na CPU e ativa a MMU
    vmm_switch_directory(kernel_directory);
    vmm_enable();
}

int vmm_map_page(page_directory_t *dir, uintptr_t virtual_addr, uintptr_t physical_addr, uint32_t flags) {
    uint32_t pd_idx = (virtual_addr >> 20) & 0xFFF; //faz um shift right com 20 bits e depois aplica a mascara de 12 bits (4096 blocos) pra achar o endereço no diretório
    uint32_t pt_idx = (virtual_addr >> 12) & 0xFF; // faz um shift right com 12 bits e depois aplica a mascara de 8 bits (256 páginas) pra achar o endereço da página

    page_table_t *pt = NULL;
    if (!(dir->entries[pd_idx] & VMM_PAGE_PRESENT)) {
        void *new_pt_phys = pmm_alloc_block(); 
        if (!new_pt_phys) return -1;

        uint32_t *clean_ptr = (uint32_t *)new_pt_phys;
        for(int i = 0; i < 256; i++) { // Tabelas L2 ARM têm 256 entradas
            clean_ptr[i] = 0;
        }

        // Configura a entrada L1: O ARM exige o padrão 0x1 nos bits inferiores para Page Table
        dir->entries[pd_idx] = ((uintptr_t)new_pt_phys) | 0x1; 
    }
    // Pega o endereço físico da tabela L2
    uintptr_t pt_phys = dir->entries[pd_idx] & 0xFFFFFC00;
    pt = (page_table_t *)pt_phys; 

    // Mapeia o endereço físico no L2
    // No ARM, uma Small Page (4KB) usa o bit 1 ativo (0x2) como flag de presente
    pt->entries[pt_idx] = (physical_addr & 0xFFFFF000) | flags | 0x2;
    return 0;
}
 
void vmm_switch_directory(page_directory_t *dir) {
    //Pega o endereço físico do diretório
    uint32_t phys_addr = (uint32_t)dir;
    
    //Invalida todo o cache do TLB antes de trocar o diretório, impede acesso de endereços virtuais mapeados em outro diretório
    asm volatile("MCR p15, 0, %0, c8, c7, 0" : : "r"(0));

    //Escreve o endereço base no TTBR0, efetivamente troca qual diretório está sendo usado
    asm volatile("MCR p15, 0, %0, c2, c0, 0" : : "r"(phys_addr));
}

void vmm_enable(void) {
    uint32_t sctlr;
    // Isso ignora checagens de permissão de nível inferior e impede o Data Abort imediato.
    uint32_t dacr = 3; 
    asm volatile("MCR p15, 0, %0, c3, c0, 0" : : "r"(dacr));

    //Lê o registrador de controle (SCTLR)
    asm volatile("MRC p15, 0, %0, c1, c0, 0" : "=r"(sctlr));

    //Modifica o bit 0 (MMU Enable)
    sctlr |= 0x1; 

    //Escreve o SCTLR de volta (A MMU LIGA EXATAMENTE AQUI)
    asm volatile("MCR p15, 0, %0, c1, c0, 0" : : "r"(sctlr));

    //Limpa o pipeline para forçar a CPU a usar a MMU a partir de agora
    asm volatile("ISB" : : : "memory");
}

//Essa função em si não é necessária para o funcionamento da memória, apenas facilita testes
page_directory_t *vmm_get_kernel_directory(void){
    // Declara que essa variável existe neste arquivo
    extern page_directory_t kernel_directory_static;
    return &kernel_directory_static;
}
