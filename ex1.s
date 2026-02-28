.intel_syntax noprefix
.global main
main:
  mov rax, 1
  push rax
  mov rax, 1
  push rax
  pop rbx
  pop rax
  add rax, rbx
  push rax
 pop rax
  ret
