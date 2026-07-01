/*
 * Isso daqui é só a implementação de uma lista ligada com processos,
 * usada pelo nosso escalonador, nada interessante aqui, é só uma
 * lista ligada comum.
*/

#ifndef NULL
#define NULL ((void*)0)
#endif

#include <process.h>
#include <scheduler.h>

pll_node* pll_node_new(process proc)
{
    process_blocks_nodes[proc->pid] = (pll_node){
        .next = NULL,
        .proc = proc
    };

    return &process_blocks_nodes[proc->pid];
}


pll_node* pll_add_end(pll_node* start_node, pll_node* adding_node)
{
    if(start_node == NULL){ 
        adding_node->next = NULL;
        return adding_node;
    }
    pll_node *current_node = start_node;
    while(current_node->next)
    {
        current_node = current_node->next;
    }
    current_node->next = adding_node;
    return start_node;
}

pll_node* pll_add_stt(pll_node* start_node, pll_node* adding_node)
{
    if(start_node == NULL) return adding_node;
    adding_node->next = start_node;
    return adding_node;
}

pll_node* pll_rem_end(pll_node* start_node)
{
    if(start_node->next == NULL) return NULL;
    pll_node *current_node = start_node;
    pll_node *prev;
    while(current_node->next)
    {
        prev = current_node;
        current_node = current_node->next;
    }
    prev->next = NULL;
    return start_node;
}

pll_node* pll_rem_stt(pll_node* start_node)
{
    if(start_node->next == NULL) return NULL;
    start_node->proc = start_node->next->proc;
    start_node->next = start_node->next->next;
    return start_node;
}

pll_node* pll_rem(pll_node* start_node, pll_node* removing_node)
{
    if(start_node == removing_node)
    {
        return start_node->next;
    }
    pll_node* current_node = start_node;
    while(current_node->next != removing_node)
    {
        current_node = current_node->next;
    }

    current_node->next = removing_node->next;
    return start_node;
}

pll_node* pll_idx(pll_node* start_node, unsigned index)
{
    unsigned current_idx = 0;
    pll_node *current_node = start_node;
    if(current_node == NULL) return NULL;
    while(current_idx != index)
    {
        current_idx++;
        if(current_node->next == NULL) return NULL;
        current_node = current_node->next;
    }
    return current_node;
}


