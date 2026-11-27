#ifndef PROTOHEAD_H
#define PROTOHEAD_H

#include <types.h>



//O virtio tem um header especial de 10 bytes antes dos pacotes
struct virtio_net_hdr {
    uint8_t flags;
    uint8_t gso_type;
    uint16_t hdr_len;
    uint16_t gso_size;
    uint16_t csum_start;
    uint16_t csum_offset;
} __attribute__((packed));

struct eth_hdr {
    uint8_t dest_mac[6];
    uint8_t src_mac[6];
    uint16_t ethertype;
} __attribute__((packed));

struct ipv4_hdr {
    uint8_t ihl_version;
    uint8_t tos;
    uint16_t tot_len;
    uint16_t id;
    uint16_t frag_off;
    uint8_t ttl;
    uint8_t protocol; // UDP = 17
    uint16_t check;
    uint32_t saddr;
    uint32_t daddr;
} __attribute__((packed));

struct udp_hdr {
    uint16_t src_port;
    uint16_t dest_port;
    uint16_t len;
    uint16_t check;
} __attribute__((packed));


struct arp_hdr {
    uint16_t hw_type;
    uint16_t proto_type;
    uint8_t  hw_len;
    uint8_t  proto_len;
    uint16_t opcode;
    uint8_t  sender_mac[6];
    uint8_t  sender_ip[4];
    uint8_t  target_mac[6];
    uint8_t  target_ip[4];
} __attribute__((packed));

#endif