#ifndef INTERRUPTS_HANDLER_H
#define INTERRUPTS_HANDLER_H

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

void init_gic(void);

void init_timer(void);

void enable_cpu_interrupts(void);

void timer_interrupt_c(void);

#endif