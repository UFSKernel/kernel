#ifndef VBOARD_H
#define VBOARD_H

typedef unsigned int uint32_t;

/* Mapeamento de memoria */

#define RAM_BASE 0x40000000

// GIC (Interrupt Controller)
#define GICD_BASE 0x08000000
#define GICC_BASE 0x08010000

// Vetores de configuracao (cada [n] tem 32 bits de configuracao)
#define GICD_ISENABLER                                                         \
  ((volatile uint32_t *)(GICD_BASE + 0x100)) // Habilitar IRQ
#define GICD_ICENABLER                                                         \
  ((volatile uint32_t *)(GICD_BASE + 0x180)) // Desabilitar IRQ
#define GICD_ICFGR                                                             \
  ((volatile uint32_t *)(GICD_BASE + 0xC00)) // Configuração (Edge/Level)

// Registradores
#define GICD_CTLR (*(volatile uint32_t *)(GICD_BASE + 0x000))
#define GICC_CTLR (*(volatile uint32_t *)(GICC_BASE + 0x000))
#define GICC_PMR (*(volatile uint32_t *)(GICC_BASE + 0x004))
#define GICC_IAR (*(volatile uint32_t *)(GICC_BASE + 0x00C))
#define GICC_EOIR (*(volatile uint32_t *)(GICC_BASE + 0x010))

/* Mapeamento de interrupcoes de dispositivos */
// Posteriormente a deteccao de dispositivos deve ser feita via device tree
// blob, fora do escopo nessa fase do tratador de interrupcoes
#define TIMER_IRQ 30

#define UART0_BASE 0x09000000
#define UART0_IRQ 33

#define VIRTIO_BASE 0x0a000000
#define VIRTIO_IRQ 48

#endif
