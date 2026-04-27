#define FORK_CALL 1
#define EXEC_CALL 2
#define WAIT_CALL 3
#define EXIT_CALL 4
#define MALLOC_CALL 5
#define PRINTF_CALL 6


int syscall(int id);
int printf(const char *s, int sizeBuffer);