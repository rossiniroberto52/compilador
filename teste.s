.intel_syntax noprefix
.global main
main:
  mov rax, 2
  push rax
  mov rax, 3
  push rax
  mov rax, 4
  push rax
  pop rbx
  pop rax
  imul rax, rbx
  push rax
  pop rbx
  pop rax
  add rax, rbx
  push rax
  mov rax, 33
  push rax
  mov rax, 42
  push rax
  pop rbx
  pop rax
  cqo
  idiv rbx
  push rax
  pop rbx
  pop rax
  add rax, rbx
  push rax
 pop rax
  ret
