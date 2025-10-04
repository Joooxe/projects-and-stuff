  .intel_syntax noprefix

  .text
  .global my_memcpy

my_memcpy:
  mov rax, rdi

  test edx, edx
  jz done

copy_qword:
  cmp edx, 8
  jb copy_byte

  mov r8, [rsi]
  mov [rdi], r8

  add rsi, 8
  add rdi, 8
  sub edx, 8

  jnz copy_qword
  test edx, edx
  jz done

copy_byte:
  mov r8b, [rsi]
  mov [rdi], r8b

  add rsi, 1
  add rdi, 1
  sub edx, 1

  jnz copy_byte

done:
  ret
