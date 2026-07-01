#ifndef TIMER_H
#define TIMER_H

typedef unsigned int uint32_t;

// frequencia das interrupcoes (Hz)
#define SYSTEM_HZ 1

static inline uint32_t read_cntfrq(void);

static void timer_callback(void);

// funcao publica de inicializacao
void init_timer(void);
void disable_timer(void);

#endif
