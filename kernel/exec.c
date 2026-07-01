#include <process.h>
#include <serial.h>
#include <timer.h>
// programas em formato de funcoes para teste

void programainicio(){
    serial_puts("Processo rodando programa inicio\n");

    int retorno = fork();

    if(!retorno){
        exec(&arq1);
    }

    retorno = fork();
    
    if(!retorno){
        exec(&arq2);
    }

    
    exit();
}

void programa1(){
    while(1){
    serial_puts("Processo rodando programa 1\n");
    for(volatile int i = 0; i < 50000000; i++); 
    }
}

void programa2(){
    while(1){
    serial_puts("Processo rodando programa 2\n");
    for(volatile int i = 0; i < 50000000; i++); 
    }
}
// """""pseudo-arquivos"""""
struct arquivo arqinicio = {programainicio, 10000};
struct arquivo arq1 = {programa1, 10000};
struct arquivo arq2 = {programa2, 10000};


void sys_exec(struct arquivo* arquivo) {
    
    // copia as intrucoes das funcoes para a memoria do processo
    process p = current;
    
    /* 
    * essa gambiarra de pseudo arquivo necessita que todas as funcoes auxiliares sejam copiadas,
    * ou seja copiamos a memoria desde o seu inicio para garantir que todas as funcoes estao sendo incluidas
    */
    char* inicio = (char*)0x40000000;
    for(int i = 0; i < arquivo->size; i++){
        p->mem[i] = inicio[i];
    }
    
    p->msize = arquivo->size; // atualiza o tamanho da memoria

    // & ~1 significa "zerar o bit menos significativo"
    unsigned int entry_point = (unsigned int) p->mem + ((((unsigned int)arquivo->start) & ~1U) - 0x40000000);     
    unsigned int usrstack_top = (unsigned int) (p->mem + SIZE_1MB); 

    p->tf = (struct trapframe*)((unsigned)p->kstack + SIZE_16KB - sizeof(struct trapframe));

    // zeramos os registradores apenas por questoes de seguranca e para facilitar a verificacao da execucao do exec
    p->tf->r0 = 0; 
    p->tf->r1 = 0; 
    p->tf->r2 = 0; 
    p->tf->r3 = 0;
    p->tf->r4 = 0; 
    p->tf->r5 = 0; 
    p->tf->r6 = 0; 
    p->tf->r7 = 0;
    p->tf->r8 = 0; 
    p->tf->r9 = 0; 
    p->tf->r10 = 0; 
    p->tf->r11 = 0;
    p->tf->r12 = 0;

    // atualiza os ponteiros de pilha e pc
    p->tf->pc_usr = entry_point;
    p->tf->sp_usr = usrstack_top;

    p->tf->lr_usr = 0;
    // coloca o resultado do exec para executar em modo usuario
    p->tf->cpsr_usr = 0x30;

    return; 
}