#include <serial.h>
#include <syscall.h>
// O linker manda o contador de programa para o binário daqui.
void kmain(void) {

    const char *c = "Bem-vindo ao UFSKernel!\n";
    serial_init();
    int a = printf(c,15);
    serial_puts("Executando em modo ARM bare-metal no QEMU.\n");

    return;
}
