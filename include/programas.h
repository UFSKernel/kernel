#ifndef PROGRAMAS_H
#define PROGRAMAS_H

typedef struct {
    const char* name;
    const char* description;
    void (*func)(int, char**);
} program;

extern program program_table[];

void programainicio(int argc, char** argv);
void programa1(int argc, char** argv);
void programa2(int argc, char** argv);
void prog_init_idle(int argc, char** argv);
void prog_count(int argc, char** argv);
void prog_prime(int argc, char** argv);
void prog_multitask(int argc, char** argv);
void prog_exit(int argc, char** argv);
void prog_help(int argc, char** argv);
void prog_evento(int argc, char** argv);
void prog_shell(int argc, char** argv);
void socket_listen(int argc, char** argv);
void socket_send(int argc, char** argv);

#endif
