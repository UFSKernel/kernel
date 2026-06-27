#include <serial.h>
#include <syscall.h>
#include <interrupts_handler.h>
// O linker manda o contador de programa para o binário daqui.
void kmain(void) {

    setup_core_for_irq();
    const char *c = "Testando alterações\n";
    serial_init();
    int teste = 15;
    teste = printf(c);
    if(teste == 10)
        serial_puts("Executando em modo ARM bare-metal no QEMU.\n");

    while(teste < 100000000)
    {
        teste++;
    }

    abort();

    return;
}
