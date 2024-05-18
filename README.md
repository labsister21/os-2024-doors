# doOrS

![Demo GIF](./doOrS_demo.gif)

## Project Description

This Repository is meant for the Operating System Course that serve as an introduction to kernel development and concretely demonstrate the subsystems within an operating system. The target platform for the operating system is the x86 32-bit Protected Mode, which will later be run using QEMU.

Milestone 0 focused on preparing the tools necessary for working on this operating system.

Milestone 1 focused on the initial development of the operating system, including enabling and handling interrupts, creating simple drivers for the display and keyboard, and developing the file system.

Milestone 2 focused on preparing the environment for user process. This included creating a memory management system, entering user mode, and developing shell.

Milestone 3 focused on enabling the operating system to run multiple processes concurrently. This included implementing process switching with context switches, creating a process manager to handle scheduling algorithms, and managing the creation, termination, and cleanup of processes.

## Running The Program

Run the makefile using `make run` command, the program will automatically be compiled, and a kernel window will pop up using the QEMU Emulator.

for creating a disk image, use `make disk` command, this will create a new disk image and delete the last one.

for inserting the shell, use `make insert-shell` command, this will insert a shell into the user space

## Program Structure

```
.
├── README.md
├── bin
│   ├── OS2024.iso
│   ├── inserter
│   ├── kernel
│   ├── sample-image.bin
│   ├── sample-image.bin:Zone.Identifier
│   ├── shell
│   ├── shell_elf
│   └── storage.bin
├── makefile
├── other
│   └── grub1
└── src
    ├── cmos.c
    ├── crt0.s
    ├── disk.c
    ├── external
    │   └── external-inserter.c
    ├── fat32.c
    ├── framebuffer.c
    ├── gdt.c
    ├── header
    │   ├── cpu
    │   │   ├── gdt.h
    │   │   ├── idt.h
    │   │   ├── interrupt.h
    │   │   └── portio.h
    │   ├── driver
    │   │   ├── cmos.h
    │   │   ├── disk.h
    │   │   ├── framebuffer.h
    │   │   └── keyboard.h
    │   ├── filesystem
    │   │   └── fat32.h
    │   ├── kernel-entrypoint.h
    │   ├── memory
    │   │   └── paging.h
    │   ├── process
    │   │   ├── process.h
    │   │   └── scheduler.h
    │   └── stdlib
    │       └── string.h
    ├── idt.c
    ├── interrupt.c
    ├── intsetup.s
    ├── kernel-entrypoint.s
    ├── kernel.c
    ├── keyboard.c
    ├── linker.ld
    ├── menu.lst
    ├── paging.c
    ├── portio.c
    ├── process.c
    ├── scheduler.c
    ├── stdlib
    │   └── string.c
    ├── user
    │   ├── clock.c
    │   ├── command.c
    │   ├── command.h
    │   ├── syscall.c
    │   ├── syscall.h
    │   └── user-shell.c
    └── user-linker.ld

```

## Team Member

| NIM      | Name                        |
| -------- | --------------------------- |
| 13522068 | Adril Putra Merin Manurung  |
| 13522098 | Suthasoma Mahardhika Munthe |
| 13522112 | Dimas Bagoes Hedrianto      |
| 13522118 | Berto Richardo Togatorop    |
