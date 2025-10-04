  .text
  .global add_scanf

  .macro push Xn
    sub sp, sp, 16
    str \Xn, [sp]
  .endm

  .macro pop Xn
    ldr \Xn, [sp]
    add sp, sp, 16
  .endm

add_scanf:
  //prologue
  push x29  // rbp
  push x30
  mov x29, sp
  sub sp, sp, 16

  ldr x0, =scanf_format_string
  mov x1, x29
  sub x1, x1, 8
  mov x2, x29
  sub x2, x2, 16

  bl scanf

  ldr x1, [x29, -8]
  ldr x2, [x29, -16]
  add x0, x1, x2

  //epilogue
  mov sp, x29
  pop x30
  pop x29
  ret

  .section .rodata

scanf_format_string:
  .string "%lld %lld"
