/* Aqui definimos que o contador de programa deve ir ao _start do startup.S. */
ENTRY(_start)

/* Aqui é definida a ordem das seções no binário final. */
SECTIONS {
    /* O endereço base da RAM na máquina virtual. */
    . = 0x40000000;

    /* A seção de código é Read / Execute (RX). */
    /* Então, tudo que deve ser apenas lido ou executado */
    /* fica aqui. Nada pode ser modificado (Write). */
    /* A seção terá: */
    .text : {
        KEEP(*(.text.startup))  /* o código de inicialização, */
        *(.text)                /* as seções de código, */
        *(.rodata)              /* as seções de constante. */
    }

    
    /* As seções .data e .bss são Read / Write (RW). */
    /* Elas armazenam as variáveis globais. */
    .data : {
        *(.data) /* Variáveis inicializadas. */
    }

    .bss : {
        *(.bss)  /* Variáveis não inicializadas. */
    }

    /* Pulamos ("alocamos") 16KB para a pilha usar. */
    stack_bottom = .; /* Rótulo para não estourarmos a pilha. */
    . = . + 0x4000;
    stack_top = .; /* Aqui é o rótulo usado no startup.S. */

    end = .;
}