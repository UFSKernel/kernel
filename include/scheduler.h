#ifndef SCHEDULER_H
#define SCHEDULER_H

#define SCHEDULER_QUANTUM_MARGIN 5
#define SCHEDULER_PCB_ENTRIES_AMOUNT 32
#define SCHEDULER_PCB_INSERTION_PRIORITY (32 >> 2)

#ifndef NULL
#define NULL ((void*)0)
#endif

#include "process.h"

typedef struct process_node_ll
{
    struct process_node_ll *next;
    process proc;
} pll_node;

pll_node* pll_node_new(process proc);

pll_node* pll_add_end(pll_node* start_node, pll_node* adding_node);

pll_node* pll_add_stt(pll_node* start_node, pll_node* adding_node);

pll_node* pll_rem_end(pll_node* start_node);

pll_node* pll_rem_stt(pll_node* start_node);

pll_node* pll_rem(pll_node* start_node, pll_node* removing_node);

pll_node* pll_idx(pll_node* start_node, unsigned index);

extern unsigned running_for;

extern unsigned list_location;

extern pll_node process_blocks_nodes[64];

struct pcb_entry
{
    pll_node *process_list;
    unsigned quantum;
    short process_count;
    short next_process;
};

extern void context_switch(struct context* current_context, struct context* new_current_context);

int time_elapsed(void);

int proc_is_valid(process proc);

pll_node* pcb_get_node_pid(pid_t pid);

void pcb_insert(unsigned priority_level, pll_node* inserting_proc);

void pcb_remove(unsigned priority_level, pll_node* removing_proc);

void pcb_climb(pll_node *climbing_proc);

void pcb_fall(pll_node *falling_proc);

void pcb_elect(void);

#endif