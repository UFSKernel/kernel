#include <serial.h>
#include <interrupts_handler.h>

void kmain(void) {
    serial_init();
    serial_puts("Bem-vindo ao UFSKernel!\n");
    serial_puts("Executando em modo ARM bare-metal no QEMU.\n");
    
    serial_puts("Preparando CPU (VBAR e Pilha IRQ)...\n");
    setup_core_for_irq();

    serial_puts("Configurando GIC e Timer...\n");
    init_gic();
    init_timer();
    
    serial_puts("Ligando interrupções...\n");
    enable_cpu_interrupts();

    while(1) {
        serial_puts("Kernel rodando na main...\n");
        // Um delay apenas para não flodar a tela
        for(volatile int i = 0; i < 50000000; i++); 
    }
}