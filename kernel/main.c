#include <serial.h>

// O linker manda o contador de programa para o binário daqui.
void kmain(void) {
    serial_init();
    serial_puts("Bem-vindo ao UFSKernel!\n");
    serial_puts("Executando em modo ARM bare-metal no QEMU.\n");

    return;
}
