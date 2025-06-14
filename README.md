# GPTOS

**A Minimal x86 Operating System**  
Bootable on QEMU, VirtualBox, or real hardware (USB stick, old PC).

## Features
- Bootloader, 32-bit C kernel
- Real keyboard typing (backspace, enter, echo)
- Built-in CLI: calculator, multi-note, echo, static clock, fun facts
- Clear, documented source; easy to extend

## Quick Start (on Linux)

1. **Install tools:**  
   `sudo apt install gcc nasm qemu-system-x86`

2. **Build & run in QEMU:**  
   ```
   make
   make run
   ```

3. **Burn to USB (be careful!):**
   ```
   sudo dd if=gptos.img of=/dev/sdX bs=512 seek=0
   # Replace /dev/sdX with your USB device
   ```

4. **Boot on real hardware:**  
   - Plug USB in, boot from USB in BIOS.

## File Overview
- `bootloader.asm`: Tiny MBR boot sector (loads kernel from 2nd sector)
- `kernel.c`: Main kernel, shell, core CLI
- `keyboard.c/h`: Keyboard handler (real typing, backspace)
- `apps/`: CLI apps (`calc.c`, `notes.c`, `extras.c`)
- `linker.ld`: Linker script (places kernel at 0x1000)
- `Makefile`: Full build & run automation

## Extend it!
Add your own commands and apps in the `apps/` folder, and add function calls in `kernel.c:shell()`.

## Credits
- Based on OSDev.org patterns and OpenAI’s ChatGPT