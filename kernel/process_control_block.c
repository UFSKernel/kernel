#include <scheduler.h>
#include <serial.h>


/*
 * Função temporária só para testes, o nosso escalonador deveria adicionar
 * o tempo desde a última chamada dessa função para a variável "running_for",
 * por enquanto, adiciona sempre 1ms.
*/
int time_elapsed(void) {return 5;}

int proc_is_valid(process proc)
{
    return proc != NULL && proc->state != ZOMBIE;
}

unsigned running_for = 0; // por quanto tempo o processo atual está rodando. Volta para 0 sempre que um processo diferente do atual executa
unsigned list_location = SCHEDULER_PCB_ENTRIES_AMOUNT; /* em qual nivel de prioridade a chamada mais recente de 'pcb_get_node_pid()' encontrou o processo *
                                                        * usado nas funções 'pcb_climb()' e 'pcb_fall()'. Um valor igual a SCHEDULER_PCB_ENTRIES_AMOUNT   *
                                                        * indica erro.                                                                                    */

/* O bloco de controle de processo, uma lista de prioridade que o nosso vai fazer revezamento circular */
struct pcb_entry pcb[SCHEDULER_PCB_ENTRIES_AMOUNT] = {
    (struct pcb_entry) {NULL, 20, 0, 0},
    (struct pcb_entry) {NULL, 25, 0, 0},
    (struct pcb_entry) {NULL, 30, 0, 0},
    (struct pcb_entry) {NULL, 35, 0, 0},
    (struct pcb_entry) {NULL, 40, 0, 0},
    (struct pcb_entry) {NULL, 45, 0, 0},
    (struct pcb_entry) {NULL, 50, 0, 0},
    (struct pcb_entry) {NULL, 55, 0, 0},
    (struct pcb_entry) {NULL, 60, 0, 0},
    (struct pcb_entry) {NULL, 65, 0, 0},
    (struct pcb_entry) {NULL, 70, 0, 0},
    (struct pcb_entry) {NULL, 75, 0, 0},
    (struct pcb_entry) {NULL, 80, 0, 0},
    (struct pcb_entry) {NULL, 85, 0, 0},
    (struct pcb_entry) {NULL, 90, 0, 0},
    (struct pcb_entry) {NULL, 95, 0, 0},
    (struct pcb_entry) {NULL, 100, 0, 0},
    (struct pcb_entry) {NULL, 105, 0, 0},
    (struct pcb_entry) {NULL, 110, 0, 0},
    (struct pcb_entry) {NULL, 115, 0, 0},
    (struct pcb_entry) {NULL, 120, 0, 0},
    (struct pcb_entry) {NULL, 125, 0, 0},
    (struct pcb_entry) {NULL, 130, 0, 0},
    (struct pcb_entry) {NULL, 135, 0, 0},
    (struct pcb_entry) {NULL, 140, 0, 0},
    (struct pcb_entry) {NULL, 145, 0, 0},
    (struct pcb_entry) {NULL, 150, 0, 0},
    (struct pcb_entry) {NULL, 155, 0, 0},
    (struct pcb_entry) {NULL, 160, 0, 0},
    (struct pcb_entry) {NULL, 165, 0, 0},
    (struct pcb_entry) {NULL, 170, 0, 0},
    (struct pcb_entry) {NULL, 175, 0, 0},
};

/*
 * Alocação estática de 64 pll_node
*/
pll_node process_blocks_nodes[64] = {0};



/*
 * Retorna o nó da lista ligada do processo que corresponde ao pid fornecido
*/
pll_node* pcb_get_node_pid(pid_t pid)
{
    for(unsigned i=0; i < SCHEDULER_PCB_ENTRIES_AMOUNT; ++i)
    {
        pll_node *current_node = pcb[i].process_list;
        if(current_node == NULL) continue;

        do
        {
            if(current_node->proc->pid == pid)
            {
                list_location = i;
                return current_node;
            }
            current_node = current_node->next;
            if(current_node == NULL) break;
        } while(current_node->next);
    }
    return NULL;
}

/*
 * Insere um processo no pcb, com nível de prioridade "priority_level".
*/
void pcb_insert(unsigned priority_level, pll_node* inserting_proc)
{
    pcb[priority_level].process_list = pll_add_end(pcb[priority_level].process_list, inserting_proc);
    pcb[priority_level].process_count += 1;
}

/*
 * Remove um processo do pcb, "priority_level" é a prioridade do processo.
 * Para obter a prioridade de um processo com o seu pid, chamar pcb_get_node_pid(>PID_DO_PROCESSO<),
 * e o nível de prioridade do processo fica salva na varíavel global "list_location".
*/
void pcb_remove(unsigned priority_level, pll_node* removing_proc)
{
    pcb[priority_level].process_list = pll_rem(pcb[priority_level].process_list, removing_proc);

    if(pcb[priority_level].process_count) pcb[priority_level].process_count -= 1;
    if(pcb[priority_level].next_process >= pcb[priority_level].process_count) pcb[priority_level].next_process = 0;
}

/*
 * Faz um processo subir sua prioridade em 1.
*/
void pcb_climb(pll_node *climbing_proc)
{
    if(list_location == 0) return;

    pcb_insert(list_location-1, climbing_proc);
    pcb_remove(list_location, climbing_proc);
}

/*
 * Faz um processo perder sua prioridade em 1.
*/
void pcb_fall(pll_node *falling_proc)
{
    if(list_location == SCHEDULER_PCB_ENTRIES_AMOUNT-1) return;

    pcb_remove(list_location, falling_proc);
    pcb_insert(list_location+1, falling_proc);
}

/*
 * Roda o processo que estava rodando caso ainda não tenha batido seu quantum,
 * ou roda o processo com a maior prioridade disponível.
*/
void pcb_elect(void)
{
    running_for += time_elapsed(); // time_elapsed é uma pseudo função, não sei como a gente pegaria tempo agora
    
    if(proc_is_valid(current))
    {
        pll_node *current_process_node = pcb_get_node_pid(current->pid); // atualiza "list_location"
        unsigned quantum = pcb[list_location].quantum;

        if(current->state == RUNNING && running_for < (quantum + SCHEDULER_QUANTUM_MARGIN))
        {
            return;
        } else
        {
            if(running_for <= quantum-SCHEDULER_QUANTUM_MARGIN){
                pcb_climb(current_process_node);
            }
            else if(running_for >= quantum-SCHEDULER_QUANTUM_MARGIN){
                pcb_fall(current_process_node);
            }
            running_for = 0;
            current->state = READY;
            current->blocked_by = BT_TIMER;
        }
    }

    for(unsigned i=0; i < SCHEDULER_PCB_ENTRIES_AMOUNT; ++i)
    {
        pll_node *current_node = pll_idx(pcb[i].process_list, pcb[i].next_process);
        if(current_node == NULL) continue;
        short next_process = pcb[i].next_process+1;

        while(current_node->proc->state != READY)
        {
            next_process++;
            current_node = current_node->next;
            if(current_node == NULL) goto end;
        }
        process old_current = current;
        running_for = 0;
        pcb[i].next_process = next_process % pcb[i].process_count;
        current = current_node->proc;
        current->state = RUNNING;
        context_switch(&old_current->context, &current->context);
        break;
        end:
    }
    /* não sei oq aconteceria caso nenhum processo fosse elegível, acho que *
     * o kernel só continuaria rodando até um desbloquear                   */ 
    return;
}


