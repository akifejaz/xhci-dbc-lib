.CODE

; static inline void xue_sys_outd(void *sys, uint32_t port, uint32_t val)
xue_sys_outd PROC
    ; mov rdx, rdx
    mov rax, r8
    out dx, eax
    ret
xue_sys_outd ENDP

; static inline uint32_t xue_sys_ind(void *sys, uint32_t port)
xue_sys_ind PROC
    xor rax, rax
    ; mov rdx, rdx
    in eax, dx
    ret
xue_sys_ind ENDP

END
