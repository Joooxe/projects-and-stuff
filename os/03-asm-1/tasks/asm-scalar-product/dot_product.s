    .intel_syntax noprefix

    .text
    .global dot_product

dot_product:
    test rdi, rdi
    jz sum_up

calc_float_qwords:
    cmp rdi, 8
    jb calc_remaining

    vmovups ymm1, [rsi]
    vmulps ymm1, ymm1, [rdx]
    vaddps ymm0, ymm0, ymm1

    add rsi, 32
    add rdx, 32
    sub rdi, 8

    jnz calc_float_qwords

    test rdi, rdi
    jz sum_up

calc_remaining:
    movss xmm1, [rsi]
    mulss xmm1, [rdx]
    addss xmm0, xmm1
    add rsi, 4
    add rdx, 4
    sub rdi, 1
    jnz calc_remaining

sum_up:
    sub rsp, 32
    vmovups [rsp], ymm0
    movups xmm1, [rsp + 16]  # get upper hafl bytes of ymm0

    # (xmm0 = [s0, s1, s2, s3]) += (xmm1 = [s4, s5, s6, s7])
    # xmm0 = [s0 + s4, s1 + s5, s2 + s6, s3 + s7]
    addps xmm0, xmm1

    haddps xmm0, xmm0
    haddps xmm0, xmm0

    add rsp, 32

    ret
