
Data1 segment
    V1db16 db 73h
    V4db2 db 011100b
    V5dbS db 'Hello'

    V1dw16 dw 3f77h
    V2dw10 dw -16375

    V3dd2 dd 00001110b
Data1 ends

Data2 segment
    V2db10 db 123
    V3db10 db -123d
    V1dd16 dd 29a3dh
    V2dd10 dd 65000
    V3dw10 dw 3772d
    V4dw2 dw 01000000b
Data2 ends

code1 segment
assume cs:code1, ds:data1, gs:data2
    popad

    push eax
label1:
    neg dword ptr [eax + esi * 4 + 2]
    neg dword ptr gs:[eax + esi * 4 + 2]

    adc eax, 010101b
    adc al, 25h

    sbb eax, ecx
    sbb al, cl
    jnae label2

    lds ebx, [eax + esi * 4 + 6]
    lds ebx, gs:[eax + esi * 4 + 6]

    mov [ebx + ecx * 4 + 12], eax
    mov byte ptr [ebx + ecx * 4 + 12], al

    and DWORD PTR [ebx + ecx * 4 + 12], 010101b
    and DWORD ptr [eax + esi * 2 + 2], 11103
    and dword ptr [edx + ecx * 4 + 0], 29a3dh
label2:
    jnae label1


    jmp far ptr label3



code1 ends


code2 segment

assume cs:code2

label3:

code2 ends

end 