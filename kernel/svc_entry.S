.section .text
.equ TRAPFRAME_OFFSET, 12
.extern current
.global svc_entry 
.type svc_entry, %function

svc_entry:
   @ código considera que o hardware fez automaticamente:
   @ trocou para SP_irq, que é a stack do kernel nesse momento
   @ salvou o CPSR do processo em SPSR_irq
   @ salvou o PC do processo + 4 em LR_irq


   @ r0–r12 ainda pertencem ao contexto interrompido 

   str r0, [sp, #-4]             @ salva momentaneamente r0 e r1
   str r1, [sp, #-8]

   mov r1, sp                    @ usamos o r1 recém-liberado para guardar o SP_irq
   
   ldr r0, =current              @ carrega o endereço da variável global estática current
   ldr r0, [r0]                  @ carrega o valor contido em current (endereço do processo atual)

   ldr sp, [r0, #TRAPFRAME_OFFSET]  @ carrega o valor do campo tf do processo atual como nova pilha
   
   ldr r0,[r1, #-4]              @ restaura r0 e r1
   ldr r1,[r1, #-8]

   sub sp, sp, #(17 * 4)         @ temos 17 variáveis na struct
   stmia sp, {r0-r12}            @ salva R0–R12 na stack

   add r12, sp, #(13*4)     
   stmia r12, {sp, lr}^           @ salva sp_usr e lr_usr

   str lr, [sp, #(15 * 4)]       @ salva o PC na pilha
   
   mrs r12, spsr                  @ pega o CPSR do processo
   
   str r12, [sp, #(16 * 4)]       @ salva o CPSR na pilha

   ldr r12, =current              @ carrega o endereço da variável global estática current
   ldr r12, [r12]                 @ carrega o valor contido em current (endereço do processo atual)

   str sp,[r12, #TRAPFRAME_OFFSET] @ atualiza o valor de tf

   bl syscall_Handler     
   b irq_return                  @ código que vai retornar ao processo