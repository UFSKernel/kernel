.text

// precaução caso seja apagado no interrupts.S
/*svc_handler:
    STMFD sp!, {r0-r12, lr}// salva o estado de usuario
    bl syscall_Handler      //chama o handler
    LDMFD sp!, {r0-r12, pc}^// retorna para o estado de usuario
                            // após executar a chamada   
*/

//Tabela de chamadas de sistema, aqui é onde efetivamente é executada a chamada,
//para criar uma nova adiciona a função e a coloque na tabela
//lembre-se de manter a ordem igual na tabela e no arquivo ponte entre
//modo usuário e supervisor, syscall.S
.global Syscall_Table
Syscall_Table:
    .word abort_handler
    .word fork_handler
    .word exec_handler
    .word exit_handler
    .word read_handler
    .word write_handler
    .word waitpid_handler
    .word socket_handler
    .word recv_handler
    .word send_handler


abort_handler:
    mov r0, #0x84000000          //Codigo base da especificação PSCI
    orr r0, r0, #0x00000008      //System Off command ID (0x84000008)
    hvc #0                       //hypervisor call, pois estamos usando uma cpu virtual

fork_handler:
    push {lr}
    bl sys_fork
    pop {pc}

exec_handler:
    push {lr}
    bl sys_exec
    pop {pc}

exit_handler:
    push {lr}
    bl sys_exit
    pop {pc}

read_handler:
    push {lr}
    bl sys_read
    pop {pc}

write_handler:
    push {lr}
    bl sys_write
    pop {pc}

waitpid_handler:
    push {lr}
    bl sys_waitpid
    pop {pc}

socket_handler:
    push {lr}
    bl sys_socket
    pop {pc}

recv_handler:
    push {lr}
    bl sys_recv
    pop {pc}

send_handler:
    push {lr}
    bl sys_send
    pop {pc}


.global syscall_Handler
.type syscall_Handler, %function
.extern serial_puts

.align 2
syscall_Handler:
    cmp r7, #10                     @ 10 é a quantidade de syscalls (0 a 9)
    bhs syscall_end                @ Branch se Higher or Same para o fim

    ldr r3, =Syscall_Table         @ carrega a tabela
    ldr r3, [r3, r7, LSL #2]       @ calcula o endereço lendo o R7 (R7 * 4)
    bx r3                          @ pula para o handler

syscall_end:
    bx lr
