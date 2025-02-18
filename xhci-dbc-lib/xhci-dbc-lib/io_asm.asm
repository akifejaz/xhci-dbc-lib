.CODE

io_outl PROC
    mov eax, edx
    mov dx, cx
    out dx, eax
    ret
io_outl ENDP

io_inl PROC
    xor rax, rax
    mov dx, cx
    in eax, dx
    ret
io_inl ENDP

END
