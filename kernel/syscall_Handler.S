
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
    .word printf_handler
    .word abort_handler



printf_handler:

    mov r0,r1
    push {lr}
    bl serial_puts
    mov r0, #10
    pop {pc}

abort_handler:
    mov r0, #0x84000000          //Codigo base da especificação PSCI
    orr r0, r0, #0x00000009      //System Reset command ID (0x84000009)
    hvc #0                       //hypervisor call, pois estamos usando uma cpu virtual


.global syscall_Handler
.type syscall_Handler, %function
.extern serial_puts

.align 2
syscall_Handler:
   
    ldr r3, =Syscall_Table //carrega a tabela
    ldr r3, [r3,r0,LSL #2] // calcula o endereço da chamada
    bx r3








