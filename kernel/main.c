#include <kstdio.h>
#include <syscall.h>
#include <interrupts_handler.h>
#include <timer.h>
#include <pmm.h>
#include <VMM.h>
#include <process.h>
#include <scheduler.h>
#include <programas.h>
#include <teste_io.h>
#include <virtio.h>

extern page_directory_t *vmm_get_kernel_directory(void);

void kmain(void) {
    setup_core_for_irq();
    // Inicialização do GIC e interrupção por timer
    kputs("Configurando GIC e Timer...\n");
    init_gic();

    init_timer();

    kstdio_init();

    // TESTES DO GERENCIADOR DE MEMÓRIA //
    //Testando o Gerenciador Físico (PMM)
    kputs("Inicializando PMM\n");
    pmm_init(); 
    kputs("Pedindo uma pagina fisica\n");
    void* phys_page = pmm_alloc_block();
    //Testando a Ativação da MMU (VMM Init)
    kputs("\nInicializando VMM e ativando MMU\n");
    vmm_init();
    kputs("Rodando em modo de Memoria Virtual.\n");
    //Testando o Mapeamento de Página
    kputs("\n[3] Testando mapeamento (Virtual -> Fisico)...\n");
    // Pegamos o diretório do kernel e escolhemos um endereço virtual qualquer
    page_directory_t *kernel_dir = vmm_get_kernel_directory();
    uintptr_t virtual_addr = 0xCAFE0000; 
    vmm_map_page(kernel_dir, virtual_addr, (uintptr_t)phys_page, VMM_PAGE_PRESENT | VMM_PAGE_WRITABLE);
    kputs("Escrevendo e lendo do endereco virtual 0xCAFE0000\n");
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
    kputs("Resultado da leitura: ");
    kputs((const char *)mem_teste);
    // FIM DOS TESTES DO GERENCIADOR DE MEMORIA //


    // TESTES DA PLACA DE REDE //
    kputs("Iniciando placa de rede.\n");
    int16_t virtio = virtio_net_init();
    if(virtio == 1){
        kputs("Placa de rede iniciada.\n");
    }
    
    else
        kputs("Placa de rede não encontrada, inicie com make run-net!\n");
    
    // FIM DOS TESTES DA PLACA DE REDE //



    kputs("Executando em modo ARM bare-metal no QEMU.\n");
    kputs("Iniciando shell...\n");
    kputs("Tente digitar help\n\n\n");

    first_process(prog_init_idle);

    return;
}


