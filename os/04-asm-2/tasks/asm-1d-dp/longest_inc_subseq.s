  .text
  .global longest_inc_subseq

longest_inc_subseq:
  cmp x2, 0
  beq return_zero

  mov x3, 0
  mov x16, 1

init_dp:
  cmp x3, x2
  bge reuse_i

  mov x4, 1
  str x4, [x1, x3, lsl 3]

  add x3, x3, 1
  b init_dp

reuse_i:
  mov x3, 0

outer_loop:
  cmp x2, x3
  ble return_max

  ldr x5, [x0, x3, lsl 3]

  mov x4, 0

inner_loop:
  cmp x3, x4
  ble next_i

  ldr x6, [x0, x4, lsl 3]

  cmp x6, x5
  bge next_j

  ldr x7, [x1, x4, lsl 3]
  add x8, x7, 1
  ldr x9, [x1, x3, lsl 3]

  cmp x8, x9
  ble next_j

  str x8, [x1, x3, lsl 3]

  cmp x8, x16
  ble next_j

  mov x16, x8
  b next_j

return_max:
  mov x0, x16
  ret

return_zero:
  mov x0, 0
  ret

next_j:
  add x4, x4, 1
  b inner_loop

next_i:
  add x3, x3, 1
  b outer_loop
