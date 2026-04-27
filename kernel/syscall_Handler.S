.equ PRINTF_CALL, 6
.text
.global syscall_Handler
.type syscall_Handler, %function
.extern serial_puts


syscall_Handler:
    // é necessario implementar uma tabela para permitir a checagem das chamadas de sistema
    // a checagem atual é apenas por um if por conveniência de testes e simplicidade

    //modo supervisor
    mrs r12, cpsr // O registrador cpsr é o que armazena o modo atual do processador.
    bic r12, r12, #0x1f // Limpa seja lá o que tinha nos 5 primeiros bits.
    orr r12, r12, #0xd3 // Coloca o valor que queremos.
    msr cpsr, r12


    cmp r0, #PRINTF_CALL
    beq printf_handler

    mov r0, #0
    SUB r0,r0, #1
    bx lr

printf_handler:

    //N esta retornando ao modo usuario
    mov r0,r1
    push {lr}
    bl serial_puts
    mov r0,#1
    pop {pc}



