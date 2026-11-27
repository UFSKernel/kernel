#include <programas.h>
#include <syscall.h>
#include <stringutils.h>
#include <stdio.h>
#include <virtio.h>

program program_table[] = {
    {"programa1", "Imprime texto de teste várias vezes na tela.", programa1},
    {"programa2", "Imprime texto de teste algumas vezes na tela.", programa2},
    {"count", "Conta de 1 até o número passado como argumento.", prog_count},
    {"prime", "Imprime os n primeiros números primos.", prog_prime},
    {"multitask", "Exemplo de paralelismo.", prog_multitask},
    {"exit", "Desliga o sistema.", prog_exit},
    {"help", "Imprime os comandos do sistema.", prog_help},
    {"evento", "Imprime o visual de evento do UFSKernel.", prog_evento},
    {"rede1", "teste de rede, use um numero de porta como argumento e envie algo para ela.", socket_listen},
    {"rede2", "teste de rede, envia uma mensagem padrão atravez da rede para o endereço local.", socket_send},
    {0, 0, 0}
};

void prog_init_idle(int argc, char** argv) {
    (void)argc; (void)argv;

    if (!fork()) exec(prog_shell, 0, 0); // Cria a shell

    // O processo original (PID 0) vira o idle process
    // Ele só vai rodar quando nenhum outro processo quiser a CPU.
    while(1) {
        // TODO: wait for interrupt, por enquanto CPU fica em 100% aqui
    }
}


void socket_listen(int argc, char** argv){
    (void)argv;
     
    if(argc>1){

    char m[65];
    

    volatile unsigned bytes_lidos = 0;
    int id = atoi(argv[argc-1]);
    bytes_lidos = recv(id, m, 40);

    if(bytes_lidos > 0){

            printf("Chegou algo.\n");

            m[bytes_lidos] = 0;
            puts(m);
            
    }
    else
        printf("Chegou nada.\n");
    }

    exit();
}

void socket_send(int argc, char** argv){
    (void)argc; (void)argv;

    char m[] = "Mensagem enviada do UFSKernel!";

    send(0, m, sizeof(m));

    exit();
}





void programa1(int argc, char** argv) {
    (void)argc; (void)argv;
    for(int i = 0; i < 10000; i++) {
        write("Processo rodando programa 1\n", 50);
    }
    //exit();
}

void programa2(int argc, char** argv) {
    (void)argc; (void)argv;
    for(int i = 0; i < 10; i++) {
        write("Processo rodando programa 2\n", 50);
    }
    exit();
}

void prog_count(int argc, char** argv) {
    if (argc < 2) {
        printf("Esse programa espera um argumento.\n");
        exit();
    }

    int limite = atoi(argv[1]);
    for(int i = 1; i < limite; i++)
        printf("%d...\n", i);
    printf("%d!!!\n", limite);
}

void prog_prime(int argc, char** argv) {
    if (argc < 2) {
        printf("Esse programa espera um argumento.\n");
        exit();
    }

    int n_primos = atoi(argv[1]);
    int n = 2;
    for (int i = 0; i < n_primos; n++) {
        char eh_primo = 1;
        for (int j = 2; j < n; j++) {
            eh_primo = (n % j) != 0;
            if (!eh_primo) break;
        }
        if (!eh_primo) continue;
        printf("Primo número %d: %d\n", i + 1, n);
        i++;
    }
}

void prog_multitask(int argc, char** argv) {
    (void)argc; (void)argv;
    puts("Processo pai iniciando...\n");
    volatile int pid1, pid2, pid3;

    pid1 = fork();
    printf("Filho 1: PID %d\n", pid1);
    if (!pid1) {
        puts("   -> Filho 1 nasceu!\n");
        for(volatile int delay = 0; delay < 5000000; delay++);
        puts("   <- Filho 1 finalizou!\n");
        exit();
    }

    pid2 = fork();
    printf("Filho 2: PID %d\n", pid2);
    if (!pid2) {
        puts("   -> Filho 2 nasceu!\n");
        for(volatile int delay = 0; delay < 5000000; delay++);
        puts("   <- Filho 2 finalizou!\n");
        exit();
    }

    pid3 = fork();
    printf("Filho 3: PID %d\n", pid3);
    if (!pid3) {
        puts("   -> Filho 3 nasceu!\n");
        for(volatile int delay = 0; delay < 5000000; delay++);
        puts("   <- Filho 3 finalizou!\n");
        exit();
    }

    puts("Pai esperando os filhos...\n");
    
    printf("Esperando 1: PID %d\n", pid1);
    waitpid(pid1);
    printf("Esperando 2: PID %d\n", pid2);
    waitpid(pid2);
    printf("Esperando 3: PID %d\n", pid3);
    waitpid(pid3);
    
    puts("Todos os filhos terminaram. Pai saindo.\n");
    exit();
}

void prog_exit(int argc, char** argv) {
    (void)argc; (void)argv;
    abort();
}

void prog_help(int argc, char** argv) {
    (void)argc; (void)argv;
    puts("Olá do UFSKernel!!\n");
    puts("Aqui está a lista de comandos e programas existentes:\n");
    for(int i = 0; program_table[i].name; i++) {
        printf("    %s : %s\n", program_table[i].name, program_table[i].description);
    }
}

void prog_evento(int argc, char** argv) {
    puts(" _____________________________________________________________________\n");
    puts("|  ┆　┆　┆　┆　┆                                                      |\n");
    puts("|  ┆　┆  ࣪ ˖☆ ࣪⭑┆ ݁˖ .☆ . ݁           __     _                        _   |\n");
    puts("| ☆⊹ ࣪ ┆ ˖ ࣪　⊹ ࣪ ★ ⋆.˚  ⊹ ࣪   _   _ / _|___| | _____ _ __ _ __   ___| |  |\n");
    puts("|    ࣪ ˖⋆˚★ ₊ ⊹　  ࣪˖ ࣪ ₊  ࣪  | | | | |_/ __| |/ / _ \\ '__| '_ \\ / _ \\ |  |\n");
    puts("| . ݁　⊹ ࣪ ˖　　　 ࣪ ˖       | |_| |  _\\__ \\   <  __/ |  | | | |  __/ |  |\n");
    puts("|　　.                     \\__,_|_| |___/_|\\_\\___|_|  |_| |_|\\___|_|  |\n");
    puts("|　　.                                                                |\n");

    if(argc == 1){
        puts("\\_____________________________________________________________________/\n");
        return;
    }

    const char* whitespace = "|                                                                     |\n";
    puts("|  _________________________________________________________________  |\n");
    puts(whitespace);
    printf("|  ~ > evento: %s%s", argv[1], whitespace + strlen(argv[1]) + 15);

    if(argc >= 3){
        puts("|                                                                     |\n");
        printf("|  ~ > quando e onde: %s%s", argv[2], whitespace + strlen(argv[2]) + 22);
    }

    puts("\\_____________________________________________________________________/\n");
}

void prog_shell(int argc, char** argv) {
    (void)argc; (void)argv;

    #define MAX_COMMAND 256
    #define MAX_WORD 64

    char command[MAX_COMMAND];
    char* child_argv[MAX_WORD];
    int pid, waitForChild = 1;

    while(1){
        puts("> ");
        gets(command);

        int child_argc = 0;
        char* p = command;
        char aspas = 0;
        while(*p && *p != '\n'){
            while(*p == ' ') p++;
	    if(!aspas && *p == '"') {aspas = 1; p++;}
            child_argv[child_argc] = p;
            child_argc++;
            while(*p && (*p != ' ' || aspas) && (*p != '"' || !aspas) && *p != '\n') p++;
            if(*p != ' ' && *p != '"') continue;
	    if(*p == '"') aspas = 0;
            *p = 0;
            p++;
            while(*p == ' ') p++;
        }
        child_argv[child_argc] = 0;
        if(!child_argc) continue;

        for(int i = 0; i < MAX_WORD; i++)
            if(child_argv[child_argc - 1][i] == '\n') { child_argv[child_argc - 1][i] = 0; break; }

        if(!strcmp(child_argv[child_argc - 1], "&")){
            waitForChild = 0;
            child_argc--;
            child_argv[child_argc] = 0;
        }

        pid = fork();
        if(pid){
            if(waitForChild) waitpid(pid); 
            else waitForChild = 1;
        } else {
            for(int i = 0; program_table[i].name; i++){
                if(strcmp(*child_argv, program_table[i].name)) continue;
                exec(program_table[i].func, child_argc, child_argv);
            }
            puts("Erro ao executar comando!\n");
            exit();
        }
    }
}
