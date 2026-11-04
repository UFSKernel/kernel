#include "virtio.h"
#include <pmm.h>
#include <types.h>
#include <interrupts_handler.h>
#include <VMM.h>
#include <process.h>
#include <scheduler.h>
#include <stdio.h>
#include <protocol_headers.h>


volatile struct socket tabela_sockets[HASHTABLE_SIZE];



#define VIRTIO0_BASE ((uint32_t *) 0x0A003e00)

/* Offsets dos Registradores Virtio (em words / 4 bytes) */
#define VIRTIO_MAGIC                0x000 // 0x000 bytes
#define VIRTIO_VERSION              0x001 // 0x004 bytes
#define VIRTIO_DEVICE_ID            0x002 // 0x008 bytes
#define VIRTIO_STATUS               0x01C // 0x070 bytes
#define VIRTIO_GUEST_PAGE_SIZE      0x00A // 0x028 bytes
#define VIRTIO_QUEUE_SEL            0x00C // 0x030 bytes
#define VIRTIO_QUEUE_NUM_MAX        0x00D // 0x034 bytes
#define VIRTIO_QUEUE_NUM            0x00E // 0x038 bytes
#define VIRTIO_QUEUE_ALIGN          0x00F // 0x03C bytes
#define VIRTIO_QUEUE_PFN            0x010 // 0x040 bytes
#define VIRTIO_QUEUE_READY          0x011 // 0x044 bytes
#define VIRTIO_QUEUE_NOTIFY         0x014 // 0x050 bytes
#define VIRTIO_INTERRUPT_STATUS     0x018 // 0x060 bytes P.S: na documentação oficial esse valor esta como 0x60 por algum motivo
#define VIRTIO_INTERRUPT_ACK        0x019 // 0x064 bytes


/*Flags de Status do Virtio */
#define VIRTIO_STATUS_ACKNOWLEDGE 1
#define VIRTIO_STATUS_DRIVER      2
#define VIRTIO_STATUS_FEATURES_OK 8
#define VIRTIO_STATUS_DRIVER_OK   4
#define VIRTIO_STATUS_FAILED      128

//IRQ
#define VIRTIO_NET_IRQ            48 // SPI = IRQ - 32


uint8_t virtiommio_on = 0; 


/* Estruturas da Virtqueue */
struct virtq_desc {
    uint64_t addr;
    uint32_t len;
    uint16_t flags;
    uint16_t next;
} __attribute__((packed));

struct virtq_avail {
    uint16_t flags;
    uint16_t idx;
    uint16_t ring[];
} __attribute__((packed));

struct virtq_used_elem {
    uint32_t id;
    uint32_t len;
} __attribute__((packed));

struct virtq_used {
    uint16_t flags;
    uint16_t idx;
    struct virtq_used_elem ring[];
} __attribute__((packed));


#define VIRTQ_SIZE 128

/* Estrutura para o kernel controlar o estado da fila */
struct virtqueue_info {
    struct virtq_desc *desc;
    struct virtq_avail *avail;
    struct virtq_used *used;
    uint16_t last_used_idx; // Controla até onde o kernel já leu
    uint16_t avail_idx;     // Controla onde colocar o próximo buffer livre
};

// Filas globais (0 = RX, 1 = TX)
struct virtqueue_info virtqueues[2];

static inline uint32_t socket_hash(uint16_t port) {
    // Multiplicação por número mágico de Fibonnaci para espalhar bem os bits
    return (port * 2654435761U) & (HASHTABLE_SIZE - 1);
}

void virtio_send_arp_reply(uint8_t *target_mac, uint8_t *target_ip) {
    struct virtqueue_info *tx = &virtqueues[1];
    uint32_t desc_id = tx->avail_idx % VIRTQ_SIZE;
    
    char *tx_buf = (char *) pmm_alloc_block();
    
    for(int i = 0; i < 10; i++) tx_buf[i] = 0;
    
    for(int i = 0; i < 6; i++) tx_buf[10 + i] = target_mac[i];
    
  
    uint8_t my_mac[6] = {0x52, 0x54, 0x00, 0x12, 0x34, 0x56};
    for(int i = 0; i < 6; i++) tx_buf[16 + i] = my_mac[i];
    
    //ARP (0x0806)
    tx_buf[22] = 0x08; tx_buf[23] = 0x06;
    
    // 3. Monta o Payload ARP (28 bytes) no offset 24
    struct arp_hdr *arp_reply = (struct arp_hdr *)(tx_buf + 24);
    
    arp_reply->hw_type    = 0x0100; // 1 (Ethernet) em Big Endian
    arp_reply->proto_type = 0x0008; // 0x0800 (IPv4) em Big Endian
    arp_reply->hw_len     = 6;
    arp_reply->proto_len  = 4;
    arp_reply->opcode     = 0x0200; // 2 (Reply) em Big Endian
    
    //Sender
    for(int i = 0; i < 6; i++) arp_reply->sender_mac[i] = my_mac[i];
    uint8_t my_ip[4] = {10, 0, 2, 15}; // IP padrão do QEMU Guest
    for(int i = 0; i < 4; i++) arp_reply->sender_ip[i] = my_ip[i];
    
    // Target
    for(int i = 0; i < 6; i++) arp_reply->target_mac[i] = target_mac[i];
    for(int i = 0; i < 4; i++) arp_reply->target_ip[i] = target_ip[i];
    

    tx->desc[desc_id].addr = (uint64_t)(uintptr_t)tx_buf;
    tx->desc[desc_id].len = 52;
    tx->desc[desc_id].flags = 0; // Read-Only para o hardware
    tx->desc[desc_id].next = 0;
    
    tx->avail->ring[desc_id] = desc_id;
    tx->avail_idx++;
    tx->avail->idx = tx->avail_idx;
    
    volatile uint32_t *mmio = VIRTIO0_BASE;
    mmio[VIRTIO_QUEUE_NOTIFY] = 1; // Avisa a Fila 1 (TX)
}

/*
Função interna para tratar a interrupção vinda da placa de rede.
*/
void virtio_net_irq_handler(void) {

    

    volatile uint32_t *mmio = VIRTIO0_BASE;

    //Lê o registrador INTERRUPT_STATUS
    if(mmio[VIRTIO_INTERRUPT_STATUS] & 1){
        struct virtqueue_info *rx = &virtqueues[0];

        while(rx->last_used_idx != rx->used->idx){


            uint16_t used_ring_index = rx->last_used_idx % VIRTQ_SIZE;

            struct virtq_used_elem *elem = &rx->used->ring[used_ring_index];

            uint32_t desc_id = elem->id;
            
            char *buffer_recebido = (char *)(uintptr_t) rx->desc[desc_id].addr;
            struct eth_hdr *eth = (struct eth_hdr *) (buffer_recebido + sizeof(struct virtio_net_hdr));

            uint16_t eth_type = ntohs(eth->ethertype);

           
            if(eth_type == 0x0800){
                struct ipv4_hdr *ip = (struct ipv4_hdr *) ((uint8_t*)eth + sizeof(struct eth_hdr));

                if(ip->protocol){ //UDP
                    int32_t ip_hdr_len = (ip->ihl_version & 0x0F) * 4;
                    struct udp_hdr *udp = (struct udp_hdr *) ((uint8_t*)ip + ip_hdr_len);

                    uint16_t porta_destino = ntohs(udp->dest_port);
                    uint8_t *payload = (uint8_t*)udp + sizeof(struct udp_hdr);
                    int32_t payload_len = ntohs(udp->len) - sizeof(struct udp_hdr);
                    uint32_t id = socket_hash(porta_destino);
                    if(tabela_sockets[id].em_uso){
                        for (int32_t j = tabela_sockets[id].rx_tamanho, k = 0; k < payload_len && j < 4096; j++, k++) 
                            tabela_sockets[id].rx_buffer[j] = payload[k];
                        tabela_sockets[id].rx_tamanho += payload_len;
                            //acordar o processo
                        if (tabela_sockets[id].processo_bloqueado) {
                            tabela_sockets[id].processo_bloqueado->state = READY;
                            tabela_sockets[id].processo_bloqueado = NULL;
                        }
                    }
                }
            }

            uint16_t novo_avail_idx = rx->avail_idx % VIRTQ_SIZE;
            rx->avail->ring[novo_avail_idx] = desc_id;
            rx->avail_idx++;
                
            rx->last_used_idx++;
        
            
        }

        rx->avail->idx = rx->avail_idx;
        

        mmio[VIRTIO_QUEUE_NOTIFY] = 0;    
    }
    
    // Avisar a placa Virtio que a interrupção foi tratada escrevendo no registrador INTERRUPT_ACK.
    mmio[VIRTIO_INTERRUPT_ACK] = 1;
}


/*
Função interna para fazer configuração de uma fila
*/
void virtqueue_config(volatile uint32_t *mmio, int32_t num){

    mmio[VIRTIO_QUEUE_SEL] = num; //Seleciona a fila
    uint32_t queue_max = mmio[VIRTIO_QUEUE_NUM_MAX];
    if (queue_max == 0) {
        // Fila nao suportada
        return;
    }
    uint32_t queue_size = VIRTQ_SIZE;
    mmio[VIRTIO_QUEUE_NUM] = queue_size;

    //Alocar as memorias da virtqueue

    mmio[VIRTIO_QUEUE_ALIGN] = 4096;

    
    // Atualmente isso só funciona por sorte de o pmm_alloc_block alocar 2 blocos consecutivos quando
    // O virtio é iniciado, futuramente isso deve se tornar um kmalloc
    void *fila_base = pmm_alloc_block();
    pmm_alloc_block();

    uint8_t *ptr = (uint8_t *)fila_base;
    for (int i = 0; i < 8192; i++) {
        ptr[i] = 0; //limpeza
    }

    virtqueues[num].desc  = (struct virtq_desc *) fila_base;
    virtqueues[num].avail = (struct virtq_avail *) (fila_base + (queue_size * sizeof(struct virtq_desc)));
    virtqueues[num].used  = (struct virtq_used *) (fila_base + 4096);
    virtqueues[num].last_used_idx = 0;
    virtqueues[num].avail_idx = 0;


    

    uint32_t page_frame_number = (uintptr_t)fila_base / 4096;
    mmio[VIRTIO_QUEUE_PFN] = page_frame_number;
    
    if (num == 0) { // Fila RX
        for (uint32_t i = 0; i < queue_size; i++) {
            // Aloca um buffer no heap do kernel
            // isso deve se tornar um kmalloc
            void *buffer = pmm_alloc_block();
            

            
            
            // Configura o descritor apontando para o buffer
            virtqueues[num].desc[i].addr = (uint64_t)(uintptr_t)buffer;
            virtqueues[num].desc[i].len = 4096;
            virtqueues[num].desc[i].flags = 2; // VIRTQ_DESC_F_WRITE (Dispositivo pode escrever aqui)
            virtqueues[num].desc[i].next = 0;
            
            // Coloca o ID do descritor no anel disponível
            virtqueues[num].avail->ring[i] = i;
        }
        // Avisa a placa que disponibilizamos os índices de 0 a queue_size
        virtqueues[num].avail->idx = queue_size;
        
        mmio[VIRTIO_QUEUE_NOTIFY] = num;
    }
    if (num == 1){
            virtqueues[num].avail->idx = 0;
    }
    

    
}


int16_t virtio_net_init(void){

    volatile uint32_t *mmio = (uint32_t *)0x0A003e00; // Como dito na documentação, no futuro isso deve ser achado usando o device tree blob

    //Verificar Magic value para checar se realmente é o endereço do virtio

    if (mmio[VIRTIO_MAGIC] != 0x74726976){ // "virt" em ASCI
        //printf("Nenhum dispositivo Virtio encontrado.\n");
        return -1;
    }
    //Vereficar versão e se é reconhecido como uma network card
    if (mmio[VIRTIO_VERSION] != 1){ 
        //printf("Versão não suportada.\n");
        return -1;
    }
    
    if (mmio[VIRTIO_DEVICE_ID] != 1){  // 1 = network card
        //printf("Dispositivo nao e uma placa de rede.\n");
        return -1;
    }


    /* 
    O processo de inicialização do virtio segue uma serie especifica de passos para avisar o driver
    de que ele vai ser usado como dispositivo de rede, os passos em sequência são:

    1.Virtio_status |= 0 (Reinicio)
    2.Virtio_status |= 1 (Acknowledge: O kernel reconhece o dispositivo virtio)
    3.virtio_status |= 2 (Driver: O kernel diz que sabe como controlar o dispositivo)
    4.O kernel avisa o dispositivo o tamanho da pagina onde ficam os pacotes.

    Depois disso, as virtqueues são configuradas
    neste programa, serão configuradas a queue 0 como recebimento RX e queue 1 como transmissão

    por fim, virtio_status recebe 4, onde o driver diz que terminou a configuração
    */

    
    mmio[VIRTIO_STATUS] = 0;
    mmio[VIRTIO_STATUS] |= VIRTIO_STATUS_ACKNOWLEDGE;
    mmio[VIRTIO_STATUS] |= VIRTIO_STATUS_DRIVER;

    mmio[VIRTIO_GUEST_PAGE_SIZE] = 4096;

    //Configuração da virtqueue
    virtqueue_config(mmio, 0); //Fila 0: RX
    virtqueue_config(mmio, 1); //Fila 1: TX

    mmio[VIRTIO_STATUS] |= VIRTIO_STATUS_DRIVER_OK;


    //registrar função de interrupção do IRQ
    if (register_interrupt_handler(VIRTIO_NET_IRQ, virtio_net_irq_handler) == 0) {
        //printf("Driver Virtio registrado no GIC com sucesso!\n");
    } else {
        return 2;
    }

    //habilitar a interrupção no hardware do GIC
    gic_enable_interrupt(VIRTIO_NET_IRQ);



    //Após inicializar a rede, inicializa os sockets como 0
    for (uint32_t i = 0; i < HASHTABLE_SIZE; i++)
        tabela_sockets[i].em_uso = 0;


    //para testes, abre um unico socket de id 0 porta 5555
    sys_socket(5555);
    for(int i =0; i<MAX_SOCKETS-1;i++){
        sys_socket(5000+i);
    }

    //ARP para a rede
    uint8_t mac_broadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t meu_ip[4] = {10, 0, 2, 15};

    // Envia o ARP Reply não-solicitado. 
    // O QEMU vai gravar essa informação na tabela dele.
    virtio_send_arp_reply(mac_broadcast, meu_ip);

    virtiommio_on = 1;
    return 1;
}










int32_t sys_socket(uint16_t porta){

    uint32_t id = socket_hash(porta);
    
    if (!tabela_sockets[id].em_uso || tabela_sockets[id].porta_local == porta){
            tabela_sockets[id].em_uso = 1;
            tabela_sockets[id].porta_local = porta;
            tabela_sockets[id].rx_tamanho = 0;
            tabela_sockets[id].processo_bloqueado = 0;
            return id; 
        }
    return -1;
}


extern struct process *current;
uint32_t sys_recv(int32_t port, void *buffer_user, uint32_t tam_max){
    if (virtiommio_on == 0)
        return 0;
    

    volatile uint32_t id = socket_hash(port);
    volatile uint32_t *mmio_interrupt = (VIRTIO0_BASE + 0x060);
    volatile struct socket *sock = &tabela_sockets[id];

    if (mmio_interrupt[0] & 1){
        virtio_net_irq_handler();
    }

    uint32_t bytes = (sock->rx_tamanho < tam_max) ? sock->rx_tamanho : tam_max;

    //seria ideal ter uma rotina de memcpy aqui
    for(uint32_t i=0; i < bytes; i++) 
        ((uint8_t*)buffer_user)[i] = sock->rx_buffer[i];

    sock->rx_tamanho = 0;

    return bytes; //retorna quantidade de bytes lidos    
}



int32_t sys_send(int32_t sock_id, void *buffer_user, uint32_t len) {
    if (virtiommio_on == 0 || sock_id < 0 || sock_id >= MAX_SOCKETS || !tabela_sockets[sock_id].em_uso) {
        return -1;
    }

    struct virtqueue_info *tx = &virtqueues[1];
    
    // Pega o próximo descritor livre 
    uint32_t desc_id = tx->avail_idx % VIRTQ_SIZE;

    char *tx_buf = (char *) pmm_alloc_block();

    for(int i = 0; i < 10; i++) tx_buf[i] = 0;

    tx_buf[10] = 0xFF; tx_buf[11] = 0xFF; tx_buf[12] = 0xFF;
    tx_buf[13] = 0xFF; tx_buf[14] = 0xFF; tx_buf[15] = 0xFF;
    
    // MAC Origem: Padrão do QEMU (Offset 16 a 21)
    tx_buf[16] = 0x52; tx_buf[17] = 0x54; tx_buf[18] = 0x00;
    tx_buf[19] = 0x12; tx_buf[20] = 0x34; tx_buf[21] = 0x56;
    
    // EtherType: IPv4 - 0x0800 (Offset 22 e 23)
    tx_buf[22] = 0x08;
    tx_buf[23] = 0x00;

    // 3. Copia a mensagem do usuário LOGO APÓS o cabeçalho Ethernet (Offset 24)
    char *origem = (char *) buffer_user;
    for(uint32_t i = 0; i < len; i++) {
        tx_buf[24 + i] = origem[i];
    }

    // O tamanho do pacote agora é: 10 (Virtio) + 14 (Ethernet) + Tamanho do Texto
    

    // 3. Configura o descritor
    tx->desc[desc_id].addr = (uint64_t)(uintptr_t)tx_buf;
    tx->desc[desc_id].len = 10 + 14 + len;
    tx->desc[desc_id].flags = 0; // 0 = Placa apenas LÊ (Read-Only para o dispositivo)
    tx->desc[desc_id].next = 0;

    // 4. Adiciona ao anel Available
    tx->avail->ring[desc_id] = desc_id;
    tx->avail_idx++;
    
    // 5. Atualiza o índice visível para a placa (deve ser a última coisa antes do notify)
    tx->avail->idx = tx->avail_idx;

    // 6. "Chuta" a placa para ela ler os dados
    volatile uint32_t *mmio = VIRTIO0_BASE;
    mmio[VIRTIO_QUEUE_NOTIFY] = 1; // Avisa a fila 1 (TX)

    return len;
}