.intel_syntax noprefix
.global main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 24
  mov rax, 10
  push rax
  pop rax
  mov [rbp - 8], rax
  mov rax, 5
  push rax
  pop rax
  mov [rbp - 16], rax
  mov rax, [rbp - 8]
  push rax
  mov rax, [rbp - 16]
  push rax
  mov rax, 2
  push rax
  pop rbx
  pop rax
  imul rax, rbx
  push rax
  pop rbx
  pop rax
  add rax, rbx
  push rax
  pop rax
  mov [rbp - 24], rax
  mov rax, 0
  mov rsp, rbp
  pop rbp
  ret
