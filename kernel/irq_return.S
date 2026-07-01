.section .text
.equ TRAPFRAME_OFFSET, 12
.global irq_return
.type irq_return, %function

irq_return:
   ldr r0, =current              @ carrega o endereço da variável global estática current
   ldr r0, [r0]                  @ carrega o valor contido em current (endereço do processo atual)

   ldr sp, [r0, #TRAPFRAME_OFFSET]  @ carrega o valor do campo tf do processo atual como nova pilha


   ldr lr, [sp, #(16 * 4)]          @ lr = cpsr_usr
   msr spsr_cxsf, lr                @ spsr = cpsr_usr (_cxsf para setar flags especificas)

   ldr lr, [sp, #(15 * 4)]          @ lr = pc_usr


   ldmia sp!, {r0-r12}              @ carrega os registradores r0-r12

   ldmia sp, {sp, lr}^              @ restaura sp_usr e lr_usr

   movs pc, lr                      @ volta para pc_usr e copia SPSR = CPSR

