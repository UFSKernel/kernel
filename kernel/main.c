#include <serial.h>

typedef unsigned int uint32_t;

// Endereços do GIC (Interrupt Controller)
#define GICD_BASE 0x08000000
#define GICC_BASE 0x08010000

#define GICD_CTLR        (*(volatile uint32_t *)(GICD_BASE + 0x000))
#define GICD_ISENABLER0  (*(volatile uint32_t *)(GICD_BASE + 0x100))

#define GICC_CTLR        (*(volatile uint32_t *)(GICC_BASE + 0x000))
#define GICC_PMR         (*(volatile uint32_t *)(GICC_BASE + 0x004))
#define GICC_IAR         (*(volatile uint32_t *)(GICC_BASE + 0x00C))
#define GICC_EOIR        (*(volatile uint32_t *)(GICC_BASE + 0x010))

#define TIMER_IRQ 30

// função do código assembly interrupts.S
extern void setup_core_for_irq(void);

void init_gic(void) {
    GICD_CTLR = 1; 
    GICC_CTLR = 1; 
    GICC_PMR = 0xFF; 
    GICD_ISENABLER0 = (1 << TIMER_IRQ); 
}

void init_timer(void) {
    uint32_t timer_ticks = 96000000; // tempo totalmente aleatorio da minha cabeça, mudar dps
    __asm__ volatile ("mcr p15, 0, %0, c14, c2, 0" :: "r" (timer_ticks));
    uint32_t ctl = 1;
    __asm__ volatile ("mcr p15, 0, %0, c14, c2, 1" :: "r" (ctl));
}

void enable_cpu_interrupts(void) {
    __asm__ volatile ("cpsie i"); // Destrava a CPU para ouvir o GIC
}

void timer_interrupt_c(void) {
    uint32_t irq_id = GICC_IAR; 

    if (irq_id == TIMER_IRQ) {
        serial_puts(">>> TICK DO TIMER! <<<\n");
        // Recarrega o timer para ele continuar tocando
        uint32_t timer_ticks = 96000000;
        __asm__ volatile ("mcr p15, 0, %0, c14, c2, 0" :: "r" (timer_ticks));
    }

    GICC_EOIR = irq_id; // Libera o GIC para a próxima
}

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