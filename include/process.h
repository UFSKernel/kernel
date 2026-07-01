#ifndef PROCESS_H
#define PROCESS_H

// ***typedef's e enum's

typedef unsigned pid_t;

typedef struct
{
  pid_t ids[32]; // Como a gente ainda não tem malloc, tem que usar um array estático.
  unsigned count;
} pid_list;

typedef struct process* process;


enum State{
    RUNNING, 
    READY, 
    BLOCKED,
    ZOMBIE
};

enum Block_Reason{
    BT_TIMER
};

enum process_blocks {WAIT_SLEEP, WAIT_FILE};


// ***constantes e variaveis globais

#define MAX_PROCESS_COUNT 64

#define SIZE_16KB  (16 * 1024)
#define SIZE_1MB   (1024 * 1024)

#define KSTACK_SIZE 16384

extern process current;
extern struct arquivo arqinicio;
extern struct arquivo arq1;
extern struct arquivo arq2;


// **funcoes

void programainicio();

void programa1();

void programa2();

extern int fork();

extern void exec(struct arquivo* arquivo);

extern void exit();

void first_process(struct arquivo arquivo);

pid_t create_pid();

void free_pid(pid_t pid);


// ***structs

struct context{

    //apenas os registradores call-preserved
    unsigned int r4;
    unsigned  r5;
    unsigned int r6;
    unsigned int r7;
    unsigned int r8;
    unsigned int r9;
    unsigned int r10;
    unsigned int r11;

    unsigned int sp;
    unsigned int lr; 
};

struct trapframe{
    unsigned int r0;
    unsigned int r1;
    unsigned int r2;
    unsigned int r3;
    unsigned int r4;
    unsigned int r5;
    unsigned int r6;
    unsigned int r7;
    unsigned int r8;
    unsigned int r9;
    unsigned int r10;
    unsigned int r11;
    unsigned int r12;

    unsigned int sp_usr;
    unsigned int lr_usr;
    unsigned int pc_usr;
    unsigned int cpsr_usr;
};



struct process{
    char* mem; // começo da memória do processo
    unsigned msize; // tamanho da memória do processo 

    char* kstack; // ponteiro para o inicio da pilha no kernel para o processo
    struct trapframe *tf; // trapframe do processo (ponteiro para o topo da pilha no kernel)

    enum State state; // estado do processo
    enum Block_Reason blocked_by;
    // Informações para o Kernel
    pid_t pid;
    process parent;
    pid_list children_ids;
  
    struct context context; // contexto do kernel

    unsigned priority;
};


/*
  Esse struct é só um conceito de como avisar um processo
  que uma operação bloqueante foi finalizada para depois
  mudar o seu estado.
*/
struct process_block_list
{
  /*
  SLEEP
  processo foi bloqueado ao chamar uma função tipo sleep().
  */
  pid_t sleep_processes[32];
  float time[32];
  int sleeping_count;

  /*
  FILE
  processo está esperando a leitura/escrita de um arquivo se tornar disponível.
  Se for usar a convenção do Unix, isso também pode ser usado para processos que
  estão esperando input de mouse e teclado.
  */
  pid_t file_processes[32];
  int files[32];
  int waiting_for_file_count;
};

// simulacao de arquivo com funcoes
struct arquivo{
  void (*start)(void);
  int size;
};





#endif