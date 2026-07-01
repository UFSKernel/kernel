// adicionar aqui codigo da chamada de sistema, 
//a ordem deve ser igual a da tabela
.equ PRINTF_CALL, 0
.equ ABORT_CALL, 1
.equ SYS_FORK, 2
.equ SYS_EXEC, 3
.equ SYS_EXIT, 4
.text


.global printf
.type printf, %function
printf:
    push {lr}
    mov r1, r0 
    mov r0, #PRINTF_CALL
    SVC #0
    pop {pc} 


.global abort
.type abort, %function
abort:
    push {lr}
    mov r0, #ABORT_CALL
    SVC #0
    pop {pc} 


.global fork
.type fork, %function
fork:
    push {lr}
    mov r0, #SYS_FORK
    SVC #0
    pop {pc} 


.global exec
.type exec, %function
exec:
    push {lr}
    mov r1, r0 
    mov r0, #SYS_EXEC
    SVC #0
    pop {pc} 


.global exit
.type exit, %function
exit:
    push {lr}
    mov r0, #SYS_EXIT
    SVC #0
    pop {pc} 


/*

exemplo:
    push {lr} guardar o endereço da chamada

    swap de registradores para organizar parametros

    mov r0, #CHAMADA //r0 é definido como registrador com o codigo da chamada,
                     //e o resto do sistema so funciona seguindo esse padrão

    SVC #0 // É a instrução trap, 
           // necessária para ir ao modo supervisor e executar qualquer chamada

    pop {pc} retorna para quem chamou
    
 */



    


    
