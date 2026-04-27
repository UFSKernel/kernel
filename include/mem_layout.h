// Armazena constantes que definem os limites físicos do seu sistema no QEMU.

#ifndef MEM_LAYOUT_H
#define MEM_LAYOUT_H

/* Endereço onde a RAM começa na máquina 'virt' do QEMU (ARM 32 bits) */
#define RAM_START        0x40000000

/* Tamanho da RAM - Vamos definir 128MB por padrão */
#define RAM_SIZE         (128 * 1024 * 1024) 

/* Onde a RAM termina */
#define RAM_END          (RAM_START + RAM_SIZE)

/* Endereço da UART (que você já usa no serial.c) */
#define UART0_ADDRESS    0x09000000

/* Tamanho de cada página/bloco de memória (4 KB) */
#define PAGE_SIZE        4096

#endif