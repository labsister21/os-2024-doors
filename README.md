<h1 align="center">🚪doOrS🚪</h1>

<div align="center">
  <img src="https://github.com/labsister21/os-2024-doors/blob/main/doOrS-demo.gif" /> <br />
</div>

## 🌟 Project Description

This Repository is meant for the Operating System Course that serve as an introduction to kernel development and concretely demonstrate the subsystems within an operating system. The target platform for the operating system is the x86 32-bit Protected Mode, which will later be run using QEMU.

Milestone 0 focused on preparing the tools necessary for working on this operating system.

Milestone 1 focused on the initial development of the operating system, including enabling and handling interrupts, creating simple drivers for the display and keyboard, and developing the file system.

Milestone 2 focused on preparing the environment for user process. This included creating a memory management system, entering user mode, and developing shell. dditionally, we implemented several bonus features such as recursive cp and rm, relative pathing, and a scrollable shell.

Milestone 3 focused on enabling the operating system to run multiple processes concurrently. This included implementing process switching with context switches, creating a process manager to handle scheduling algorithms, and managing the creation, termination, and cleanup of processes.

## 🏃 Running The Program

for creating a disk image, use `make disk` command, this will create a new disk image and delete the last one.

for inserting the shell, use `make insert-shell` command, this will insert a shell into the root directory

fot inserting the clock, use `make insert-clock` command, this will insert clock into the root directory

Run the makefile using `make run` command, the program will automatically be compiled, and a kernel window will podp up using the QEMU Emulator.

## 💻 Commands

In the user shell, you can run the following commands:

<table>
  <tr>
    <td>1. <code>cat [filename]</code></td>
    <td>: show content of file.</td>
  </tr>
  <tr>
    <td>2. <code>cd [target]</code></td>
    <td>: move current directory to target.</td>
  </tr>
  <tr>
    <td>3. <code>clear</code></td>
    <td>: clear current screen.</td>
  </tr>
  <tr>
    <td>4. <code>cls</code></td>
    <td>: clear current screen.</td>
  </tr>
  <tr>
    <td>5. <code>cp [-r] source dest</code></td>
    <td>: copy file/folder to destination.</td>
  </tr>
  <tr>
    <td>6. <code>ls [foldername]</code></td>
    <td>: show current or target directory contents</td>
  </tr>
  <tr>
    <td>7. <code>mkdir dirname</code></td>
    <td>: create new directory\n</td>
  </tr>
  <tr>
    <td>8. <code>mv source dest</code></td>
    <td>: move file/folder to destination</td>
  </tr>
  <tr>
    <td>9. <code>rm [-r] target</code></td>
    <td>: delete folder/ file.</td>
  </tr>
  <tr>
    <td>10. <code>find name</code></td>
    <td>: find file/folder with given name.</td>
  </tr>
  <tr>
    <td>11. <code>kill [pid]</code></td>
    <td>: kill process with process id pid.</td>
  </tr>
  <tr>
    <td>12. <code>exec [filename]</code></td>
    <td>: execute binary file.</td>
  </tr>
  <tr>
    <td>13. <code>./[filename]</code></td>
    <td>: execute binary file.</td>
  </tr>
  <tr>
    <td>14. <code>ps</code></td>
    <td>: show process list.</td>
  </tr>
  <tr>
    <td>15. <code>help</code></td>
    <td>: show available commands.</td>
  </tr>
</table>

## 📁 Program Structure

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

## 🧑‍🤝‍🧑 Team Member 

| NIM      | Name                        | Github Profile                                    |
| -------- | --------------------------- | ------------------------------------------------- |
| 13522068 | Adril Putra Merin           | [ninoaddict](https://github.com/ninoaddict)       |
| 13522098 | Suthasoma Mahardhika Munthe | [sotul04](https://github.com/sotul04)             |
| 13522112 | Dimas Bagoes Hendrianto     | [dimasb1954](https://github.com/dimasb1954)       |
| 13522118 | Berto Richardo Togatorop    | [BertoRichardo](https://github.com/BertoRichardo) |
