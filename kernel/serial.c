// Volatile pois pode mudar a qualquer momento.
// Impede o compilador de otimizar leituras e escritas.
#define UART0_DR ((volatile unsigned int *)0x09000000)

// É muito importante frisar que esse endereço não será
// o mesmo em um hardware real!! Isso é especificamente
// para o QEMU.

// Simplesmente altera o valor daquela posição de memória.
void serial_putc(char c) { *UART0_DR = c; }

void serial_puts(const char *s) {
    while(*s) serial_putc(*s++);
}

void serial_init(void) {}
