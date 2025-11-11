ENTRY(_start)

PHDRS
{
    text PT_LOAD FLAGS(5);  /* Segmento de texto: RX */
    data PT_LOAD FLAGS(6);  /* Segmento de dados: RW */
}

SECTIONS {
    . = 0x40000000;

    .text : {
        KEEP(*(.text.startup))
        *(.text)
        *(.rodata)
    } :text /* Atribui esta seção ao segmento 'text' */

    . = ALIGN(4K);

    .data : {
        *(.data)
        .bss_start = .;
        *(.bss)
        .bss_end = .;
    } :data /* Atribui esta seção ao segmento 'data' */
    
    . = . + 0x4000;
    stack_top = .;
    
    end = .;
}
