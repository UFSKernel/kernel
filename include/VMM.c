#include "VMM.h"

// O diretório de páginas principal do próprio kernel
static page_directory_t *kernel_directory = NULL;

void vmm_init(void) {
    // 1. Aloca memória física para o diretório de páginas do kernel usando o seu PMM
    // Nota: Como o pmm_alloc_block retorna 4KB e o L1 precisa de 16KB,precisará de pmm_alloc_block, uma função no PMM que aloca blocos contíguos ou espaço estático reservado.
    kernel_directory = (page_directory_t *)pmm_alloc_block(); 
    
    // Zera o diretório para garantir que nenhum mapeamento lixo exista
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
    // Descobre os índices de nível 1 e nível 2 a partir do endereço virtual
    uint32_t pd_idx = (virtual_addr >> 20) & 0xFFF;       // Bits superiores determinam a entrada L1
    uint32_t pt_idx = (virtual_addr >> 12) & 0xFF;        // Bits do meio determinam a entrada L2

    page_table_t *pt = NULL;

    // Verifica se a tabela de páginas de segundo nível (L2) já existe para esse bloco de 1MB
    if (!(dir->entries[pd_idx] & VMM_PAGE_PRESENT)) {
        // Se não existir, aloca uma nova tabela física via PMM
        void *new_pt = pmm_alloc_block();
        if (!new_pt) return -1; // Sem memória física!

        // Limpa a nova tabela de páginas
        // ... zerar entradas ...

        // Configura a entrada do diretório apontando para a nova tabela de páginas física
        // (Aqui entra a formatação específica que o hardware ARM exige para descritores L1)
        dir->entries[pd_idx] = ((uintptr_t)new_pt) | VMM_PAGE_PRESENT; 
    }

    // Obtém o endereço da tabela L2 (limpando as flags inferiores de configuração)
    pt = (page_table_t *)(dir->entries[pd_idx] & 0xFFFFFC00); // Máscara ARM para base da PT

    // Mapeia o endereço físico na entrada L2 com as flags de permissão
    pt->entries[pt_idx] = (physical_addr & 0xFFFFF000) | flags;

    // Importante: Sempre que alterar as tabelas de página, limpe o TLB (Cache da MMU)
    // para evitar que a CPU use traduções antigas em cache.
    // asm volatile("MCR p15, 0, %0, c8, c7, 0" : : "r"(0)); // Comando ARM para invalidar TLB

    return 0;
}

void vmm_switch_directory(page_directory_t *dir) {
    // No ARM, isso envolve injetar o endereço físico do diretório no registrador TTBR0 (Translation Table Base Register 0)
    // Exemplo de Assembly Inline para ARM:
    // asm volatile("MCR p15, 0, %0, c2, c0, 0" : : "r"(dir));
}

void vmm_enable(void) {
    // Configura o registrador de controle do sistema ARM (SCTLR / CP15 c1) setando o bit 0 (SCTLR.M) para 1
    uint32_t sctlr;
    // asm volatile("MRC p15, 0, %0, c1, c0, 0" : "=r"(sctlr));
    // sctlr |= 0x1; // Ativa bit da MMU
    // asm volatile("MCR p15, 0, %0, c1, c0, 0" : : "r"(sctlr));
}

