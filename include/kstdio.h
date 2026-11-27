#ifndef KSTDIO_H
#define KSTDIO_H

#include <stdarg.h>

// Inicializacao
void kstdio_init(void);

// Output
void kprintf(const char *format, ...);
void kputs(const char *str);
void kputc(char c);

// Input
int kgetc(void);
void kgets(char *buf, int max_len);

// Utilidade
int kstdio_has_data(void);

int sys_write(char* buffer, unsigned max_len);
int sys_read(char* buffer, unsigned max_len);



#endif // KSTDIO_H
