//stub

.section .text
.global vector_table
.global irq_handler
.global setup_core_for_irq

.extern serial_puts 
.type setup_core_for_irq, %function

.align 5			   
vector_table:			
    b _start			// 0x00   |   Reset	  |  supervisor
    b .    	
    b svc_handler           	// 0x08   |   SVC  	  |  supervisor
    b .                     	
    b .  	
    b .                     	
    b .
    b .                     
   

setup_core_for_irq:
    ldr r0, =vector_table
    mcr p15, 0, r0, c12, c0, 0
    mrs r1, cpsr
    bic r0, r1, #0x1f
    orr r0, r0, #0xd2
    msr cpsr_c, r0
    ldr sp, =stack_top
    sub sp, sp, #8192
    bic sp, sp, #7
    msr cpsr_c, r1
    bx lr

// no arquivo real precisa ter esta função
svc_handler:
    STMFD sp!, {r1-r12, lr}  // salva o estado de usuario
    bl syscall_Handler       //chama o handler
    LDMFD sp!, {r1-r12, pc}^ // retorna para o estado de usuario
                             // após executar a chamada          