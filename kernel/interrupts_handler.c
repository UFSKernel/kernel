#include <interrupts_handler.h>
#include <serial.h>

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