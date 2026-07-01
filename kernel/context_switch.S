.section .text
.global context_switch
.type context_switch, %function

context_switch:
    @ recebe como parametros:   1- endereco do contexto do processo a ser substituido,  2- endereco do contexto do processo substituto
    
    @ verifica se o current não é nulo
    cmp r0, #0                    
    beq if_null
    
    @ salva os registradores na ordem da struct
    stmia r0!, {r4-r11}
    str sp, [r0], #4
    str lr, [r0]

        if_null:
    @ carrega os registradores na ordem da struct
    ldmia r1!, {r4-r11}
    ldr sp, [r1], #4
    ldr lr, [r1]
    
    @ salta para o retorno
    bx lr
