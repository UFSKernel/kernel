#ifndef SYSCALL_H
#define SYSCALL_H

/*
Serve apenas como definição das funções que fazem chamadas de sistema,
permite o acesso delas a partir de código C.
*/

//int printf(const char *s);
int abort(void);

int fork(void);
void exec(void (*programa)(int, char**), int argc, char** argv);
void exit(void);
int waitpid(int pid);

int read(char* buffer, unsigned max_len);
int write(char* buffer, unsigned max_len);

int socket(unsigned short porta);
unsigned recv(int sock_id, void *buffer_user, unsigned tam_max);
int send(int sock_id, void *buffer_user, unsigned len);

#endif // SYSCALL_H
