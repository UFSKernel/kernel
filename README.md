# Projeto UFSKernel - O Kernel

![Build e Teste do Kernel](https://github.com/UFSKernel/kernel/actions/workflows/build-and-test.yml/badge.svg)

Este repositório contém o código-fonte do **UFSKernel**, um sistema operacional didático desenvolvido como uma atividade de extensão na Universidade Federal de São Carlos.

## Começando

Antes de começar, você precisa ter o ambiente de desenvolvimento configurado.

### Pré-requisitos

- **Toolchain ARM:** `arm-none-eabi-gcc`
- **Emulador QEMU:** `qemu-system-arm`

As instruções detalhadas de instalação do ambiente podem ser encontradas na **[documentação oficial do projeto](https://github.com/UFSKernel/docs/releases/latest)**.

## Como Contribuir

A contribuição para o kernel segue um fluxo de trabalho padrão baseado em Fork e Pull Request. É fundamental seguir todos os passos para garantir a qualidade e a organização do projeto.

1.  **Faça o Fork do Repositório**
   
    Clique no botão "Fork" no canto superior direito desta página para criar uma cópia do repositório na sua conta do GitHub.

2.  **Clone o Seu Fork**
  
    Em seu ambiente local, clone o repositório que você acabou de criar:
    ```bash
    git clone https://github.com/<seu_nome_de_usuário>/kernel.git
    cd kernel
    ```
    Criem apenas um fork por grupo, todos os membros devem implementar no mesmo fork.

3.  **Crie uma Nova Branch**
  
    Crie uma branch específica para a sua funcionalidade:
    ```bash
    git switch -c <nome_da_funcionalidade>
    ```

4.  **Implemente sua Funcionalidade**
 
    Siga a estrutura de arquivos do projeto:
    - **APIs e Declarações (`.h`):** Devem ser criadas no diretório `include/` para fazer parte das bibliotecas padrão.
    - **Implementações (`.c`):** Devem ser criadas no diretório `kernel/`.
    - **Testes:** Todo o código de teste para sua nova funcionalidade deve ser adicionado na função `kmain()` dentro de `kernel/main.c`. Não substitua os testes já existentes! Suas alterações devem apenas **adicionar** linhas.

5.  **Compile e Teste Localmente**
 
    A qualquer momento, você pode compilar e executar o kernel no QEMU com um único comando:
    ```bash
    make run
    ```
    Garanta que o kernel compila sem erros e que a saída no QEMU é a esperada para os seus testes.

6.  **Faça o Commit e o Push**
 
    Quando estiver satisfeito com sua implementação, adicione seus arquivos e envie para o seu fork:
    
    ```bash
    git add .
    git commit -m "Implementação de <nome_da_funcionalidade>."
    git push origin <nome_da_funcionalidade>
    ```

7.  **Abra um Pull Request**
    
    Volte para a página do seu fork no GitHub. Um botão para "Compare & pull request" aparecerá. Clique nele, escreva uma descrição clara das suas mudanças e abra o Pull Request para a branch `main` do repositório `UFSKernel/kernel`.

## ⚙️ Automação e Testes (CI)

Este repositório utiliza **GitHub Actions** para garantir a integridade do código. Toda vez que um Pull Request é aberto, um workflow automático é executado para:
1.  Compilar todo o projeto com a flag `-Werror` (warnings são tratados como erros).
2.  Executar o kernel no QEMU e verificar se a mensagem de inicialização é impressa corretamente.

O botão **"Merge" de um Pull Request só será habilitado se todos os testes passarem**. Isso garante que a branch `main` esteja sempre funcional.

## ⚖️ Licença

Este projeto é licenciado sob a **GPLv3**. Veja o arquivo `LICENSE` para mais detalhes.
