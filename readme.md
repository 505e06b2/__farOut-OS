# [Test in your browser](https://505e06b2.github.io/__farOut-OS/)

# Build and Run in QEMU
```bash
make init
make run
```

# Operational Info
- Only 2880 sector (1.44MB) floppy disks are currently supported
- Only FAT12 read operations are currently supported
- Tiny Memory Model - Code, Data, Stack, Heap all in one segment
- One segment per binary, but frequent use of Far Functions
- Libc has access to Kernel functions
- Executables have access to Libc functions

### Assumptions
- When loading files, it is assumed that are under 64k as they would not entirely fit into a segment

## Layout Of Memory By Segment
| 0          | 1                               | 2 | 3 | 4 | 5            | 6    | 7      | 8+         |
|------------|---------------------------------|---|---|---|--------------|------|--------|------------|
| [Unusable] | Scratch Space<br>(File Loading) | * | * | * | Init (Shell) | Libc | Kernel | [Unusable] |
