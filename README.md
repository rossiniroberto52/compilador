<div align="center">
  <img src="./images/Gemini_Generated_Image_sqe50ssqe50ssqe5-removebg-preview.png" alt="Quartz Compiler Logo" width="200"/>

  # Quartz Compiler
  **Um compilador de tipagem estrita construído do zero em C, traduzindo lógica direta para Assembly x86_64.**
</div>

---

## ⚙️ A Física do Sistema

O Quartz (`.qz`) não é um interpretador. É um compilador de dois passos (Two-Pass Compiler) de baixo nível que converte código fonte diretamente para código de máquina nativo Linux (x86_64), sem depender de máquinas virtuais intermediárias.

### O Pipeline
1. **Análise Léxica (Lexer):** Processamento bruto da fita de texto em Tokens estritos.
2. **Análise Sintática (Parser):** Construção de uma Árvore Sintática Abstrata (AST) com suporte a precedência matemática e delegação absoluta de blocos.
3. **Binding & Análise Semântica:** Congelamento no tempo de endereços físicos (Offsets) da Pilha de Memória diretamente na Árvore Sintática.
4. **Geração de Código (CodeGen):** Tradução direta da AST para instruções Assembly `x86_64` (Sintaxe Intel).

---

## 🏗️ Decisões de Arquitetura (O Silício)

* **A Tabela de Símbolos Plana (Flat Symbol Table):** [POR QUE VOCÊ ESCOLHEU ESSA ABORDAGEM EM VEZ DE UMA PILHA DE TABELAS? EXPLIQUE AQUI O CACHE DA CPU]
    
* **Controle de Escopo Léxico (Amnésia Programada):**
    [COMO AS VARIÁVEIS NASCEM E MORREM NOS BLOCOS { }? EXPLIQUE AQUI A MECÂNICA DE DEPTH]

* **O Curto-Circuito Lógico (Short-Circuit):**
    [COMO OS OPERADORES && E || IMPEDEM QUE O KERNEL EXPLODA POR DIVISÃO POR ZERO? EXPLIQUE AQUI OS SALTOS JE/JNE]

---

## 🚀 Como Compilar e Executar

**Pré-requisitos:** `gcc`, `make` e ambiente Linux (ou WSL).

```bash
# 1. Construir o motor do compilador
make

# 2. Escrever o seu código Quartz
echo "x = 10; if (x == 10) { print(1); }" > script.qz

# 3. Compilar para Assembly (.s)
./compiler script.qz > saida.s

# 4. Linkar e Executar o binário nativo
gcc saida.s -o script_executavel
./script_executavel