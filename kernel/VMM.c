#include "VMM.h"
#include <stddef.h>

// O diretório de páginas principal do próprio kernel
static page_directory_t kernel_directory_static __attribute__((aligned(16384)));

void vmm_init(void) {
    // 1. Aloca memória física para o diretório de páginas do kernel usando o seu PMM
    // Nota: Como o pmm_alloc_block retorna 4KB e o L1 precisa de 16KB,precisará de pmm_alloc_block, uma função no PMM que aloca blocos contíguos ou espaço estático reservado.
    page_directory_t *kernel_directory = &kernel_directory_static;
    // Zera o diretório para garantir que nenhum mapeamento lixo exista
    if(kernel_directory == NULL) {
        serial_puts("[ERROR] Falha ao alocar diretório de páginas para o kernel!\n");
        while(1); 
    }

    for (int i = 0; i < NUM_FIRST_LEVEL_ENTRIES; i++) {
        kernel_directory->entries[i] = 0;
    }
    // 2. Identity Mapping (Mapear Virtual == Físico) para regiões críticas:
    // Você PRECISA mapear o código do próprio kernel e a UART, senão o código trava no milissegundo em que a MMU for ligada.
    
    // Exemplo: Mapeando a UART (Dispositivo de I/O precisa de flag No-Cache)
    vmm_map_page(kernel_directory, UART0_ADDRESS, UART0_ADDRESS, VMM_PAGE_PRESENT | VMM_PAGE_WRITABLE | VMM_PAGE_NOCACHE);
    // Exemplo: Mapeando a RAM onde o kernel reside
    for (uintptr_t addr = RAM_START; addr < RAM_END; addr += PAGE_SIZE) {
        vmm_map_page(kernel_directory, addr, addr, VMM_PAGE_PRESENT | VMM_PAGE_WRITABLE);
    }
    // 3. Carrega o diretório do kernel na CPU e ativa a MMU
    vmm_switch_directory(kernel_directory);
    vmm_enable();
}

int vmm_map_page(page_directory_t *dir, uintptr_t virtual_addr, uintptr_t physical_addr, uint32_t flags) {
    uint32_t pd_idx = (virtual_addr >> 20) & 0xFFF; 
    uint32_t pt_idx = (virtual_addr >> 12) & 0xFF; 

    page_table_t *pt = NULL;
    if (!(dir->entries[pd_idx] & VMM_PAGE_PRESENT)) {
        // ATENÇÃO: pmm_alloc_block() precisa retornar memória física ALINHADA e limpa, 
        // ou você deve limpá-la manualmente aqui:
        void *new_pt_phys = pmm_alloc_block(); 
        if (!new_pt_phys) return -1;

        // SE ESTIVER COM A MMU LIGADA: Você precisa converter new_pt_phys para um endereço virtual válido
        // para conseguir limpá-lo. Exemplo temporário se ainda estiver em Identity Mapping:
        uint32_t *clean_ptr = (uint32_t *)new_pt_phys;
        for(int i = 0; i < 256; i++) { // Tabelas L2 ARM Coarse têm 256 entradas
            clean_ptr[i] = 0;
        }

        // Configura a entrada L1: O ARM exige o padrão 0x1 nos bits inferiores para Coarse Page Table
        dir->entries[pd_idx] = ((uintptr_t)new_pt_phys) | 0x1; 
    }

    // Pega o endereço físico da tabela L2
    uintptr_t pt_phys = dir->entries[pd_idx] & 0xFFFFFC00;

    // CORREÇÃO CRÍTICA: Se a MMU já estiver ativa, você DEVE converter pt_phys 
    // para o seu endereço virtual equivalente antes de usar como ponteiro!
    // Se ainda estiver na inicialização (Identity Mapping), pode usar direto:
    pt = (page_table_t *)pt_phys; 

    // Mapeia o endereço físico no L2
    // No ARM, uma Small Page (4KB) usa tipicamente o bit 1 ativo (0x2) como flag de presente
    pt->entries[pt_idx] = (physical_addr & 0xFFFFF000) | flags | 0x2;

    // Invalida a entrada modificada no TLB para que a CPU veja a alteração imediatamente
    // asm volatile("MCR p15, 0, %0, c8, c7, 1" : : "r"(virtual_addr)); // Invalida TLB por MVA

    return 0;
}

void vmm_switch_directory(page_directory_t *dir) {
    // 1. Pega o endereço físico do diretório
    uint32_t phys_addr = (uint32_t)dir;
    
    // 2. Invalida todo o cache do TLB antes de trocar o diretório (Segurança)
    asm volatile("MCR p15, 0, %0, c8, c7, 0" : : "r"(0));

    // 3. Escreve o endereço base no TTBR0
    asm volatile("MCR p15, 0, %0, c2, c0, 0" : : "r"(phys_addr));
}

void vmm_enable(void) {
    uint32_t sctlr;

    // 1. CONFIGURAÇÃO CRÍTICA: Define o Domínio 0 como "Manager" (Valor 3).
    // Isso ignora checagens de permissão de nível inferior e impede o Data Abort imediato.
    uint32_t dacr = 3; 
    asm volatile("MCR p15, 0, %0, c3, c0, 0" : : "r"(dacr));

    // 2. Lê o registrador de controle (SCTLR)
    asm volatile("MRC p15, 0, %0, c1, c0, 0" : "=r"(sctlr));

    // 3. Modifica o bit 0 (MMU Enable)
    sctlr |= 0x1; 

    // 4. Escreve o SCTLR de volta (A MMU LIGA EXATAMENTE AQUI)
    asm volatile("MCR p15, 0, %0, c1, c0, 0" : : "r"(sctlr));

    // 5. BARREIRA CRÍTICA: Limpa o pipeline para forçar a CPU a usar a MMU a partir de agora
    asm volatile("ISB" : : : "memory");
}

