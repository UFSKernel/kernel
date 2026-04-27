
.equ PRINTF_CALL, 6

.text
.global syscall
.type syscall, %function


syscall:

    bx lr

.global printf
.type printf, %function

printf:
    mov r2, r0 // swap de registradores, r0 -> r1, r1 -> r2
    mov r3, r1 // isso acontece para colocarmos o codigo do printf (ou qualquer outra chamada ) no r0, por questões de padronização

    mov r0, #PRINTF_CALL
    mov r1, r2
    mov r2, r3

    b syscall_Handler // Tail call optimization

    
