// kernel/main.c
#include "serial.h"
#include "types.h"
#include "pmm.h"
#include "VMM.h"
#include "mem_API.h"

void kmain(void) {
    // 1. Inicializa a comunicação para podermos ver os testes
    serial_init();
    serial_puts("\n--- KERNEL BOOT: TESTE DE MEMORIA ---\n");

    // 2. Inicializa o gerenciador físico (onde seu bitmap será configurado)
    serial_puts("[INIT] Inicializando PMM...\n");
    pmm_init(); 

    // 3. Teste Unitário da sua API (kmalloc / kfree)
    serial_puts("[TEST] Testando kmalloc...\n");
    
    // Vamos pedir 8KB (deve exigir 2 páginas de 4KB)
    uintptr_t ptr1 = kmalloc(8000); 
    
    serial_puts("[TEST] kmalloc alocou memoria com sucesso!\n");

    // 4. Inicializa a Memória Virtual (MMU / Tabelas de Página)
    serial_puts("[INIT] Inicializando VMM e ativando MMU...\n");
    vmm_init();
    
    serial_puts("[SUCCESS] MMU ativa e rodando em modo mapeado!\n");

    // Loop eterno para o kernel não morrer
    while(1) {
        // ...
    }
}