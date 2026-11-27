#include <interrupts_handler.h>
#include <vboard.h>
#include <bitutils.h>

static isr_t interrupt_handlers[MAX_INTERRUPTS] = {0};

uint32_t current_irq;

int register_interrupt_handler(uint32_t irq, isr_t handler) {
  if (irq >= MAX_INTERRUPTS) return -1; // ID inválido
  if (interrupt_handlers[irq]) return -2; // Já existe
  
  interrupt_handlers[irq] = handler; // Registra tratador
  return 0;
}

void gic_enable_interrupt(uint32_t irq) {
  if (irq >= MAX_INTERRUPTS) return; // ID inválido

  volatile uint32_t* isenabler = (volatile uint32_t*) GICD_ISENABLER;
  isenabler[irq / 32] |= (1 << (irq % 32));
  if (irq >= 32) {
        volatile uint8_t* itargetsr = (volatile uint8_t*) GICD_ITARGETSR_BASE; 
        itargetsr[irq] = 1; // 1 = CPU 0
    }
}

void gic_disable_interrupt(uint32_t irq) {
  if (irq >= MAX_INTERRUPTS) return; // ID inválido

  enable_bit(GICD_ICENABLER, irq); // Ativa o bit, desliga a interrupção
}

void init_gic(void) {
  GICD_CTLR = 1; // Ativa distribuidor global de interrupções
 
  GICC_PMR = 0xFF; // Tratar interrupções de qualquer prioridade

  GICC_CTLR = 1; // Ativa interface de interrupções da CPU
}

void gic_config_interrupt(uint32_t irq, int edge_triggered) {
  // Cada IRQ ocupa 2 bits, acha o indice dividindo por 16
  uint32_t reg_index = irq >> 4;
  // Primeiros 4 bits são quantos pares tem que pular
  uint32_t shift = (irq & BIT_MASK(4)) << 1;

  // Primeiro, limpamos o par de bits da IRQ
  GICD_ICFGR[reg_index] &= ~(0b11 << shift);

  // Se for sensível à borda, aplicamos o valor 10
  if (edge_triggered) GICD_ICFGR[reg_index] |= (0b10 << shift);
}

void enable_cpu_interrupts(void) { __asm__ volatile("cpsie i"); }

void irq_dispatcher_c(void) {
  uint32_t irq_id = GICC_IAR & 0x3FF; // Pega ID da interrupção do registrador
  current_irq = irq_id;

 

  if (irq_id < MAX_INTERRUPTS && interrupt_handlers[irq_id])
    interrupt_handlers[irq_id](); // Trata, se possível

  GICC_EOIR = irq_id; // Mudança: isso aqui ficou pra baixo, pois ela não significa
  //''estamos tratando'', e sim ''ja tratamos''
}

void irq_end_current(void){
    GICC_EOIR = current_irq; // Avisa que ultima interrupção foi tratada
}
