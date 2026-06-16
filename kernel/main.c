#include "serial.h"
#include "types.h"
#include "pmm.h"
#include "VMM.h"

extern page_directory_t *vmm_get_kernel_directory(void);

void kmain(void) {
    serial_init();
    //Testando o Gerenciador Físico (PMM)
    serial_puts("Inicializando PMM\n");
    pmm_init(); 
    serial_puts("Pedindo uma pagina fisica\n");
    void* phys_page = pmm_alloc_block();
    //Testando a Ativação da MMU (VMM Init)
    serial_puts("\nInicializando VMM e ativando MMU\n");
    vmm_init();
    serial_puts("Rodando em modo de Memoria Virtual.\n");
    //Testando o Mapeamento de Página
    serial_puts("\n[3] Testando mapeamento (Virtual -> Fisico)...\n");

    // Pegamos o diretório do kernel e escolhemos um endereço virtual qualquer
    page_directory_t *kernel_dir = vmm_get_kernel_directory();
    uintptr_t virtual_addr = 0xCAFE0000; 
    vmm_map_page(kernel_dir, virtual_addr, (uintptr_t)phys_page, VMM_PAGE_PRESENT | VMM_PAGE_WRITABLE);
    serial_puts("Escrevendo e lendo do endereco virtual 0xCAFE0000\n");
    // Criamos um ponteiro para a memória virtual, onde vamos escrever e tentar ler pra ver se deu certo
    volatile char *mem_teste = (volatile char *)virtual_addr;
    mem_teste[0] = 'V';
    mem_teste[1] = 'M';
    mem_teste[2] = 'M';
    mem_teste[3] = ' ';
    mem_teste[4] = 'O';
    mem_teste[5] = 'K';
    mem_teste[6] = '!';
    mem_teste[7] = '\n';
    mem_teste[8] = '\0';

    serial_puts("Resultado da leitura: ");
    serial_puts((const char *)mem_teste);
    // Loop eterno
    while(1) {
    }
}