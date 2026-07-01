#include <process.h>
#include <scheduler.h>


// salvar o contexto do processo
// chamar fork
// fork:
// criar uma struct de processo
// copiar tudo do current
// mudar o registrador de retorno do current para o pid do processo criado
// mudar o registrador de retorno do processo criado para 0
// inserir o processo no bloco de processos ou inserir em uma fila do escalonador

// sem malloc temos memoria estatica:
struct process structProcessos[MAX_PROCESS_COUNT];   // 64 structs de processos
char memProcessos[MAX_PROCESS_COUNT][SIZE_1MB];       // 64 espacos de memoria de 1MB
char kstackProcessos[MAX_PROCESS_COUNT][SIZE_16KB];      // 64 pilhas de kernel de 16KB

// funcao de retorno para processos recém clonados
extern void irq_return(void);



int sys_fork(){
    pid_t pid = create_pid(); // gera um pid 

    process newprocess = &structProcessos[pid];  // ira ser substituido por malloc
    
    newprocess->parent = current;  // define o pai

    newprocess->mem = memProcessos[pid];  // ira ser substituido por malloc
    newprocess->kstack = kstackProcessos[pid] ;  // ira ser substituido por malloc

    newprocess->msize = newprocess->parent->msize; // copia o tamanho da memoria
    newprocess->state = READY; // coloca como pronto
    newprocess->pid = pid; 

    // copia toda a memoria do pai para o filho
    for(unsigned int i = 0; i < sizeof(memProcessos[0]); i++){
        newprocess->mem[i] = newprocess->parent->mem[i];
    }

    // copia toda a kstack do pai para o filho
    for(int i = 0; i < KSTACK_SIZE; i++){
        newprocess->kstack[i] = newprocess->parent->kstack[i];
    }

    newprocess->tf = (struct trapframe*)((char *)newprocess->kstack + ((char *)newprocess->parent->tf - (char *)newprocess->parent->kstack)); // calcula o topo da pilha do filho com base na do pai

    // calcula os offsets necessarios para o filho ficar na sua regiao de memoria

    unsigned int pc_offset = newprocess->parent->tf->pc_usr - (unsigned int)newprocess->parent->mem;
    newprocess->tf->pc_usr = (unsigned int)newprocess->mem + pc_offset;

    unsigned int lr_offset = newprocess->parent->tf->lr_usr - (unsigned int)newprocess->parent->mem;
    newprocess->tf->lr_usr = (unsigned int)newprocess->mem + lr_offset;

    unsigned int sp_offset = newprocess->parent->tf->sp_usr - (unsigned int)newprocess->parent->mem;
    newprocess->tf->sp_usr = (unsigned int)newprocess->mem + sp_offset;

    // prepara os retornos diferentes
    newprocess->parent->tf->r0 = newprocess->pid;
    newprocess->tf->r0 = 0;   

    newprocess->context.lr = (unsigned int)irq_return; // prepara o retorno da troca de contexto para fork return

    // inserir na fila de maior prioridade do escalonador
    pll_node* newprocess_block_node = pll_node_new(newprocess);
    pcb_insert(0, newprocess_block_node);

   return current->tf->r0;
}



void first_process(struct arquivo arquivo){

    pid_t pid = create_pid(); // gera um pid 

    process newprocess = &structProcessos[pid];  // ira ser substituido por malloc
    
    newprocess->parent = current;

    newprocess->mem = memProcessos[pid];  // ira ser substituido por malloc
    newprocess->kstack = kstackProcessos[pid];  // ira ser substituido por malloc
    
    newprocess->state = READY; // comeca pronto
    newprocess->pid = pid; 
    
    // inserir na fila de maior prioridade do escalonador
    pll_node* newprocess_block_node = pll_node_new(newprocess);
    pcb_insert(0, newprocess_block_node);
    
    current = newprocess;
    unsigned int usrstack_top = (unsigned int) (current->mem + SIZE_1MB);
    current->tf = (struct trapframe*)((unsigned)current->kstack + SIZE_16KB - sizeof(struct trapframe));
    current->tf->sp_usr = usrstack_top;

    exec(&arquivo); //executa o programa inicial
    irq_return();
}