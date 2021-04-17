.code

extern vmexit_handler : proc
extern processor_initialize_vmx : proc
extern prepare_vmxoff_globals : proc
extern g_guest_rip : QWORD
extern g_guest_rsp : QWORD


SAVE_GP macro
        push     r15
        push     r14
        push     r13
        push     r12
        push     r11
        push     r10
        push     r9
        push     r8
        push     rdi
        push     rsi
        push     rbp
        push     rbx
        push     rdx
        push     rcx
        push     rax
endm

RESTORE_GP macro
        pop    rax
        pop    rcx
        pop    rdx
        pop    rbx
        pop    rbp
        pop    rsi
        pop    rdi
        pop    r8
        pop    r9
        pop    r10
        pop    r11
        pop    r12
        pop    r13
        pop    r14
        pop    r15
endm

__readcs PROC
	mov ax, cs
	ret
__readcs ENDP


__readds PROC
	mov ax, ds
	ret
__readds ENDP


__reades PROC
	mov ax, es
	ret
__reades ENDP


__readss PROC
	mov ax, ss
	ret
__readss ENDP


__readfs PROC
	mov ax, fs
	ret
__readfs ENDP


__readgs PROC
	mov ax, gs
	ret
__readgs ENDP


__readldtr PROC
	sldt ax
	ret
__readldtr ENDP


__readtr PROC
        str     ax
        ret
__readtr ENDP


__read_segment_access_rights PROC
        lar     rax, rcx
        ret
__read_segment_access_rights ENDP


__readrip PROC
    mov rax, [rsp]
    ret
__readrip ENDP


__readrsp PROC
    mov rax, rsp
    add rax, 8h
    ret
__readrsp ENDP

__invept PROC
	invept rcx, oword ptr [rdx]
    jz invept_break
    jc invept_break
	ret

invept_break:
    int 3
    ret
__invept ENDP

;vmexit_stub PROC
;        int 3
;        movaps  xmmword ptr [rsp], xmm0
;        movaps  xmmword ptr [rsp - 10h], xmm1
;        movaps  xmmword ptr [rsp - 20h], xmm2
;        movaps  xmmword ptr [rsp - 30h], xmm3
;        movaps  xmmword ptr [rsp - 40h], xmm4
;        movaps  xmmword ptr [rsp - 50h], xmm5
;
;        sub rsp, 58h
;        SAVE_GP
;        mov     rcx, rsp
;
;        sub     rsp, 20h
;        call    vmexit_handler
;        add     rsp, 20h
;
;        RESTORE_GP
;        add rsp, 58h
;
;        movaps  xmm0, xmmword ptr [rsp]
;        movaps  xmm1, xmmword ptr [rsp - 10h]
;        movaps  xmm2, xmmword ptr [rsp - 20h]
;        movaps  xmm3, xmmword ptr [rsp - 30h]
;        movaps  xmm4, xmmword ptr [rsp - 40h]
;        movaps  xmm5, xmmword ptr [rsp - 50h]
;
;        sub rsp, 100h
;        vmresume
;vmexit_stub ENDP

vmexit_stub PROC
        ;int 3

        SAVE_GP
        mov     rcx, rsp

        sub     rsp, 28h
        call    vmexit_handler
        add     rsp, 28h

        test al, al
	    jnz vmxoff_handler

        RESTORE_GP

        sub rsp, 0100h
        vmresume
vmexit_stub ENDP

vmm_setup_stub PROC
    SAVE_GP
    sub rsp, 28h

    mov rdx, rsp

    call processor_initialize_vmx

    ret
vmm_setup_stub ENDP

vmxoff_handler PROC
    sub rsp, 20h
    call prepare_vmxoff_globals
    add rsp, 20h

    RESTORE_GP
    
    vmxoff

    mov rsp, g_guest_rsp
    jmp g_guest_rip
vmxoff_handler ENDP

vmm_entrypoint PROC
    ; TODO: verify rsp 16-byte alinged ?
    add rsp, 28h
    RESTORE_GP

    ret

vmm_entrypoint ENDP

END