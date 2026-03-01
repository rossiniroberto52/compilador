.intel_syntax noprefix
.data
.LC0:
  .string "%d\n"
.text
.global main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 8
  mov rax, 10
  push rax
  pop rax
  mov [rbp - 8], rax
  mov rax, [rbp - 8]
  push rax
  mov rax, 10
  push rax
  pop rbx
  pop rax
  cmp rax, rbx
  sete al
  movzx rax, al
  push rax
    pop rax
    cmp rax, 0
    je .L0
  mov rax, 5
  push rax
  pop rax
  mov [rbp - 8], rax
  mov rax, [rbp - 8]
  push rax
  pop rsi

  lea rdi, [rip + .LC0]
  mov rax, 0
  call printf@PLT
.L0:
  mov rax, [rbp - 8]
  push rax
  pop rsi

  lea rdi, [rip + .LC0]
  mov rax, 0
  call printf@PLT
  mov rax, 0
  mov rsp, rbp
  pop rbp
  ret
