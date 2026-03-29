.section .text
.global vector_table
.global irq_handler
.global setup_core_for_irq

.type irq_handler, %function
.type setup_core_for_irq, %function

.align 5                
vector_table:
    b _start            // 0x00: Reset
    b .                 // 0x04: Undefined
    b .                 // 0x08: SVC
    b .                 // 0x0C: Prefetch
    b .                 // 0x10: Data Abort
    b .                 // 0x14: Hyp Trap
    b irq_handler       // 0x18: IRQ (interrupção do timer)
    b .                 // 0x1C: FIQ               

irq_handler:
    sub lr, lr, #4              
    stmfd sp!, {r0-r12, lr}     
    
    blx timer_interrupt_c      
    
    ldmfd sp!, {r0-r12, pc}^    

setup_core_for_irq:
    // Aponta o VBAR para nossa tabela
    ldr r0, =vector_table
    mcr p15, 0, r0, c12, c0, 0

    // Backup do modo atual
    mrs r1, cpsr

    // Entra no Modo IRQ para dar uma pilha a ele
    bic r0, r1, #0x1f
    orr r0, r0, #0xd2
    msr cpsr_c, r0

    // Configura a pilha de interrupções
    ldr sp, =stack_top
    sub sp, sp, #8192
    bic sp, sp, #7

    // Restaura o modo original
    msr cpsr_c, r1

    // Retorna para o C
    bx lr