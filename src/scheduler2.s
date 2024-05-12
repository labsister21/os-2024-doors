global process_context_switch

process_context_switch:
    mov ax, 0x28 | 0
    mov eax, 0x20 | 0x3
    mov  ds, ax
    mov  es, ax
    mov  fs, ax
    mov  gs, ax

    ; base address for ctx
    lea ecx, [esp+0x4]

    ; setup IRET stack frame
    push eax ; GDT_USER_CODE_SELECTOR
    mov eax, 0x400000-4
    push eax ; last 4 MiB
    mov eax, [ecx+52]
    push eax ; eflags
    mov eax, 0x18 | 0x3
    push eax ; Code segment selector USER CODE SELECTOR
    mov eax, [ecx+48]
    push eax ; eip

    ; load all general purpose register
    mov ebx, [ecx+16]
    mov edx, [ecx+20]
    mov eax, [ecx+28]
    mov ecx, [esp+28]

    iret