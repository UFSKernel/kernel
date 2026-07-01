#include <process.h>
#include <scheduler.h>


void sys_exit(){
    pll_node* current_pll_node = pcb_get_node_pid(current->pid);     // Atualiza list_location e retorna o no da fila
    pcb_remove(list_location, current_pll_node);                     // Remove o no da fila
    
    free_pid(current->pid);                                          // Libera o pid
    current->state = ZOMBIE;                                         // Muda o estado para Zombie
    pcb_elect();                                                     // Chama o escalonador
}
