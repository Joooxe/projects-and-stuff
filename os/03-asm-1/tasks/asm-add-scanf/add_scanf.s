  .intel_syntax noprefix

  .text
  .global add_scanf

add_scanf:
  # prologue
  push rbp
  mov rbp, rsp
  sub rsp, 16


  mov rdi, offset scanf_format_string
  mov rsi, rbp
  sub rsi, 8
  mov rdx, rbp
  sub rdx, 16

  call scanf

  mov rax, [rbp - 8]
  add rax, [rbp - 16]

  # epilogue
  mov rsp, rbp
  pop rbp
  ret

  .section .rodata

scanf_format_string:
  .string "%lld %lld"
