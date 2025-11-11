// Pedimos ao assembler para que coloque esse código
// em uma seção especial do arquivo final.
// Fazemos isso para que possamos pedir ao linker que
// coloque essa seção especial bem no início do executável.
.section .text.startup
.global _start

// Esse é o código inicial.
// O linker vai falar "ei, comece por aqui".
// A linha 2 deixa esse rótulo visível ao linker.
_start:
    mrs r0, cpsr // O registrador cpsr é o que armazena o modo atual do processador.
    bic r0, r0, #0x1f // Limpa seja lá o que tinha nos 5 primeiros bits.
    orr r0, r0, #0xd3 // Coloca o valor que queremos.
    msr cpsr, r0

    // O valor 11010011 no byte menos significativo faz duas coisas:
    //   - 11 nos bits mais a esquerda desabilitam interrupções.
    //   Se houver uma interrupção antes de sabermos tratá-la,
    //   nosso processador vai ser desviado para um lugar
    //   desconhecido e travar tudo.
    //   - 10011 nos bits mais a direita deixam o processador no
    //   modo supervisor!! Lembra? O kernel deve rodar no modo
    //   supervisor.

    ldr sp, =stack_top // Stack pointer aponta pro rótulo stack_top (fim do arquivo).

    bl kmain // Pula para onde está o código binário da função kmain, que será linkada.

hang:
    b hang // Loop infinito caso kmain retorne para não executar lixo de memória.

// Sinaliza ao assembler que o que segue é uma seção de dados.
.section .bss
.align 4 // Faz o próximo endereço ser múltiplo de 4.

stack_bottom:
    .space 16384 // Reserva 16KB para a pilha.

stack_top: // A pilha cresce pra baixo daqui até o stack_bottom
