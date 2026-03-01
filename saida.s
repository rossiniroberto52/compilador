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
  mov rax, 0
  push rax
  pop rax
  mov [rbp - 8], rax
.L0:
  mov rax, [rbp - 8]
  push rax
  mov rax, 5
  push rax
  pop rbx
  pop rax
  cmp rax, rbx
  sete al
  movzx rax, al
  push rax
  mov rax, 0
  push rax
  pop rbx
  pop rax
  cmp rax, rbx
  sete al
  movzx rax, al
  push rax
  pop rax
  cmp rax, 0
  je .L1
  mov rax, [rbp - 8]
  push rax
  mov rax, 1
  push rax
  pop rbx
  pop rax
  add rax, rbx
  push rax
  pop rax
  mov [rbp - 8], rax
  mov rax, [rbp - 8]
  push rax
  pop rsi

  lea rdi, [rip + .LC0]
  mov rax, 0
  call printf@PLT
  jmp .L0
.L1:
  mov rax, 0
  mov rsp, rbp
  pop rbp
  ret
