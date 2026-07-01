#ifndef INTERRUPTS_HANDLER_H
#define INTERRUPTS_HANDLER_H

typedef unsigned int uint32_t;

// função do código assembly interrupts.S
#define MAX_INTERRUPTS 1024
typedef void (*isr_t)(void);

void init_gic(void);
void enable_cpu_interrupts(void);
void setup_core_for_irq(void);

// API para os outros módulos
int register_interrupt_handler(uint32_t irq, isr_t handler);
void gic_config_interrupt(uint32_t irq, int edge_triggered);
void gic_enable_interrupt(uint32_t irq);
void gic_disable_interrupt(uint32_t irq);

// Chamada pelo assembly
void irq_dispatcher_c(void);

#endif
