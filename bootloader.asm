; bootloader.asm: MBR, loads kernel at 0x1000 (2nd sector, floppy/hdd)
BITS 16
ORG 0x7C00

xor ax, ax
mov ds, ax

mov si, msg
call print_string

mov ah, 0x02      ; BIOS read sector
mov al, 16        ; Sectors to read
mov ch, 0         ; Cylinder
mov cl, 2         ; Sector (start at 2)
mov dh, 0         ; Head
mov dl, 0x00      ; Floppy
mov bx, 0x1000    ; Buffer to load (0x1000)
int 0x13

jmp 0x0000:0x1000 ; Jump to loaded kernel

print_string:
    mov ah, 0x0E
.next:
    lodsb
    cmp al, 0
    je .done
    int 0x10
    jmp .next
.done:
    ret

msg db "GPTOS booting...", 0

times 510-($-$$) db 0
dw 0xAA55