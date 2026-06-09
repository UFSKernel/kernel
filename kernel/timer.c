#include <interrupts_handler.h>
#include <serial.h>
#include <timer.h>
#include <vboard.h>

static uint32_t ticks_p_q = 0;

static inline uint32_t read_cntfrq(void) {
  uint32_t val;
  // no boot (ARM>=7) grava a frequencia do clock base no registrador CNTFRQ
  // (Counter Frequency)
  __asm__ volatile("mrc p15, 0, %0, c14, c0, 0" : "=r"(val));
  return val;
}

// função que será chamada quando a interrupção for ativada
static void timer_callback(void) {
  serial_puts(">>> TICK DO TIMER <<<\n");

  // recarrega o timer
  __asm__ volatile("mcr p15, 0, %0, c14, c2, 0" ::"r"(ticks_p_q));

  // é aqui que o código fará a chamada para o
  // escalonador
}

void init_timer(void) {
  uint32_t hardware_freq = read_cntfrq();
  // quantos ticks formam um quantum no sistema
  ticks_p_q = hardware_freq / SYSTEM_HZ;

  // inicializa o timer com ticks_p_q
  __asm__ volatile("mcr p15, 0, %0, c14, c2, 0" ::"r"(ticks_p_q));
  uint32_t ctl = 1;
  __asm__ volatile("mcr p15, 0, %0, c14, c2, 1" ::"r"(ctl));

  // Toda interrupção deve se registrar no despachante do GIC
  if (!register_interrupt_handler(TIMER_IRQ, timer_callback))
    serial_puts("Registrando timer no GIC...\n");
  else
    serial_puts("Erro ao registrar timer no GIC!\n");

  // ativando a interrupção apos registrar
  gic_enable_interrupt(TIMER_IRQ);
}

void disable_timer(void) {
  gic_disable_interrupt(TIMER_IRQ);

  // 0 no registrador de controle (c14, c2, 1) para parar o timer
  uint32_t ctl = 0;
  __asm__ volatile("mcr p15, 0, %0, c14, c2, 1" ::"r"(ctl));

  register_interrupt_handler(TIMER_IRQ, 0);

  serial_puts("Timer desativado com sucesso!\n");
}
