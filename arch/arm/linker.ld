/* Aqui definimos que o contador de programa deve ir ao _start do startup.S */
ENTRY(_start)

SECTIONS {
    . = 0x00010000; /* Esse é o endereço onde o QEMU inicia quando usada a flag -kernel. */
    
    /* Definição da ordem das seções. */
    .text : {
        KEEP(*(.text.startup))
        *(.text)
        *(.rodata)
    }

    .data : {
        *(.data) /* Coloca dados inicializados (variáveis globais com valor). */
    }

    .bss : {
        *(.bss)  /* Coloca dados não inicializados (onde nossa pilha está). */
    }

    . = ALIGN(8);
    . = . + 0x1000; /* Espacinho por segurança. */
    stack_top = .;

    end = .;
}
