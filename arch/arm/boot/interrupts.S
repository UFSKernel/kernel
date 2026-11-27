.section .text
.global vector_table
.global irq_handler
.global setup_core_for_irq
.global print_undefined_pc

.extern kputs 

.type irq_handler, %function
.type setup_core_for_irq, %function


.align 5			// (32 bits)     
vector_table:			// offset |  interrupcao  |  modo de execucao 
    b _start			// 0x00   |   Reset	  |  supervisor
    b undefined_handler     	// 0x04   |   Undefined	  |  undefined
    b svc_entry           	// 0x08   |   SVC  	  |  supervisor
    b prefetch_abort_handler   	// 0x0C   |   Prefetch    |  abort
    b data_abort_handler    	// 0x10   |   Data Abort  |  abort 
    b .                     	// 0x14   |   Hyp Trap    |  hypervisor
    b irq_entry                	// 0x18   |   IRQ         |  interrupt
    b .                     	// 0x1C   |   FIQ         |  fast interrupt

irq_handler:
    sub lr, lr, #4              
    stmfd sp!, {r0-r12, lr}     
    
    blx irq_dispatcher_c      
    
    ldmfd sp!, {r0-r12, pc}^    

setup_core_for_irq:
    // Aponta o VBAR para nossa tabela
    ldr r0, =vector_table
    mcr p15, 0, r0, c12, c0, 0

    // Backup do modo atual
    mrs r1, cpsr

    // 1. Pilha do Modo IRQ (Desce 8 KB)
    bic r0, r1, #0x1f
    orr r0, r0, #0xd2
    msr cpsr_c, r0
    ldr sp, =stack_top
    sub sp, sp, #8192

    // 2. Pilha do Modo Abort (Desce 16 KB)
    bic r0, r1, #0x1f
    orr r0, r0, #0xd7
    msr cpsr_c, r0
    ldr sp, =stack_top
    sub sp, sp, #16384  

    // 3. Pilha do Modo Undefined (Desce 24 KB)
    bic r0, r1, #0x1f
    orr r0, r0, #0xdb
    msr cpsr_c, r0
    ldr sp, =stack_top
    sub sp, sp, #24576  

    // Restaura o modo original e alinha
    bic sp, sp, #7
    msr cpsr_c, r1
    bx lr


// tratadores de erro

undefined_handler:
    ldr r0, =msg_undef
    bl kputs
    sub lr, lr, #4
    b .                     

data_abort_handler:
    ldr r0, =msg_abort
    bl kputs
    b .

prefetch_abort_handler:
    ldr r0, =msg_prefetch
    bl kputs
    b .

svc_handler:
    ldr r0, =msg_svc
    bl kputs
    movs pc, lr             

// mensagens
.section .rodata
msg_undef:  .asciz ">>> PANIC: Instrucao indefinida! <<<\n"
msg_svc:    .asciz ">>> INFO: SVC Solicitada! <<<\n"
msg_abort:  .asciz ">>> PANIC: Data Abort! <<<\n"
msg_prefetch: .asciz ">>> PANIC: Prefetch Abort! <<<\n"
