#include <interrupts_handler.h>
#include <vboard.h>

static isr_t interrupt_handlers[MAX_INTERRUPTS] = {0};

int register_interrupt_handler(uint32_t irq, isr_t handler) {
  if (irq < MAX_INTERRUPTS) {
    interrupt_handlers[irq] = handler;
    return 0;
  }
  return -1; // erro
}

void gic_enable_interrupt(uint32_t irq) {
  if (irq >= MAX_INTERRUPTS)
    return;

  // cada registrador tem 32 bits
  uint32_t reg_index = irq / 32;  // qual registrador
  uint32_t bit_offset = irq % 32; // qual bit do registrador

  GICD_ISENABLER[reg_index] |= (1 << bit_offset);
}

void gic_disable_interrupt(uint32_t irq) {
  if (irq >= MAX_INTERRUPTS)
    return;

  uint32_t reg_index = irq / 32;
  uint32_t bit_offset = irq % 32;

  // No ICENABLER escrever 1 desliga a interrupção
  GICD_ICENABLER[reg_index] = (1 << bit_offset);
}

void init_gic(void) {
  GICD_CTLR = 1;
  GICC_CTLR = 1;
  GICC_PMR = 0xFF;
}

// sinal do hardware: 1 para Edge-triggered, 0 para Level-sensitive
void gic_config_interrupt(uint32_t irq, int edge_triggered) {
  // cada IRQ ocupa 2 bits, pacha o indice dividindo or 16
  uint32_t reg_index = irq / 16;

  // shift do par de bits
  uint32_t shift = (irq % 16) * 2;

  // Primeiro, limpamos o par de bits atual usando uma máscara (11 em binário
  // invertido)
  GICD_ICFGR[reg_index] &= ~(0b11 << shift);

  // Se for sensível à borda, aplicamos o valor 10 (em binário)
  if (edge_triggered) {
    GICD_ICFGR[reg_index] |= (0b10 << shift);
  }
}

void enable_cpu_interrupts(void) { __asm__ volatile("cpsie i"); }

void irq_dispatcher_c(void) {
  uint32_t irq_id = GICC_IAR & 0x3FF;

  if (irq_id < MAX_INTERRUPTS && interrupt_handlers[irq_id] != 0) {
    interrupt_handlers[irq_id]();
  }

  GICC_EOIR = irq_id;
}
