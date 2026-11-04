#ifndef VIRTIO_H
#define VIRTIO_H


#include <types.h>
#include <process.h>



#define MAX_SOCKETS 16
#define HASHTABLE_SIZE 32 //tamanho adequado para evitar divisoes e garantir bom espalhamento, caso ocupe muito troque para 31 ou 23
//HEADERS DOS PACOTES




//Um socket
struct socket {
    uint32_t em_uso;
    uint16_t porta_local;
    uint8_t rx_buffer[4096];
    uint32_t rx_tamanho;
    struct process *processo_bloqueado;
};




extern struct process *current;


// Funções de rede
int16_t virtio_net_init(void);
int32_t sys_socket(uint16_t porta);
uint32_t sys_recv(int32_t sock_id, void *buffer_user, uint32_t tam_max);
int32_t sys_send(int32_t sock_id, void *buffer_user, uint32_t len);

#define ntohs(x) __builtin_bswap16(x)

#endif