// adicionar aqui codigo da chamada de sistema, 
//a ordem deve ser igual a da tabela
.equ ABORT_CALL, 0
.equ SYS_FORK, 1
.equ SYS_EXEC, 2
.equ SYS_EXIT, 3
.equ SYS_READ, 4
.equ SYS_WRITE, 5
.equ SYS_WAITPID, 6
.equ SYS_SOCKET, 7
.equ SYS_RECV, 8
.equ SYS_SEND, 9
.text


.global abort
.type abort, %function
abort:
    push {r7, lr}
    mov r7, #ABORT_CALL
    SVC #0
    pop {r7, pc} 


.global fork
.type fork, %function
fork:
    push {r7, lr}
    mov r7, #SYS_FORK
    SVC #0
    pop {r7, pc} 


.global exec
.type exec, %function
exec:
    push {r7, lr}
    mov r7, #SYS_EXEC
    SVC #0
    pop {r7, pc} 


.global exit
.type exit, %function
exit:
    push {r7, lr}
    mov r7, #SYS_EXIT
    SVC #0
    pop {r7, pc}


.global read
.type read, %function
read:
    push {r7, lr}
    mov r7, #SYS_READ
    SVC #0
    pop {r7, pc}


.global write
.type write, %function
write:
    push {r7, lr}
    mov r7, #SYS_WRITE
    SVC #0
    pop {r7, pc}


.global waitpid
.type waitpid, %function
waitpid:
    push {r7, lr}
    mov r7, #SYS_WAITPID
    SVC #0
    pop {r7, pc}

.global socket
.type socket, %function
socket:
    push {r7, lr}
    mov r7, #SYS_SOCKET
    SVC #0
    pop {r7, pc} 

.global recv
.type recv, %function
recv:
    push {r7, lr}
    mov r7, #SYS_RECV
    SVC #0
    pop {r7, pc}

.global send
.type send, %function
send:
    push {r7, lr}
    mov r7, #SYS_SEND
    SVC #0
    pop {r7, pc}

/*

exemplo:
    push {r7, lr} guardar o endereço da chamada

    swap de registradores para organizar parametros

    mov r0, #CHAMADA //r0 é definido como registrador com o codigo da chamada,
                     //e o resto do sistema so funciona seguindo esse padrão

    SVC #0 // É a instrução trap, 
           // necessária para ir ao modo supervisor e executar qualquer chamada

    pop {r7, pc} retorna para quem chamou
    
 */



    


    
