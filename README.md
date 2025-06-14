# GPTOS

**A Minimal x86 Operating System**  
Bootable on QEMU, VirtualBox, or real hardware (USB stick, old PC).

## Features
- Bootloader, 32-bit C kernel
- Real keyboard typing
- Simple RAM file system (create/delete/copy/read files)
- Built-in CLI: calculator, notes, echo, file manager
- Third-party app support (`.prg` format, see `apps/thirdparty/`)
- File API available to third-party apps!

## CLI File Commands
- `ls`           — list all files
- `cat <file>`   — view file content
- `new <file>`   — create/edit a file (multi-line, finish with '~' on a line)
- `del <file>`   — delete a file
- `cp <a> <b>`   — copy file a → b

## File API for .prg Apps
See `api.h`:
- `file_create(filename, text)` — create/write
- `file_read(filename, buf, max)` — read to buf, returns bytes read
- `file_delete(filename)`
- `file_list(array, max)` — fills up to max filenames in array

**Example (see `apps/thirdparty/savehello.c`):**
```c
#include "../../api.h"
void main(struct API *api) {
    api->file_create("hi.letter", "Hello, Letter!");
    char buf[256];
    if (api->file_read("hi.letter", buf, 256) > 0)
        api->print(buf);
}
```

## Build & Run (Linux/macOS/Windows)
See previous instructions. Use QEMU, MSYS2, or Homebrew toolchains as before.

## Contributing
Add your `.prg` to `apps/thirdparty/`!

---