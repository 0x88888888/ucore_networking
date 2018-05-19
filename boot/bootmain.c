#include <defs.h>
#include <x86.h>
#include <elf.h>

#define SECTSIZE        512
#define ELFHDR          ((struct elfhdr *)0x10000)      // scratch space

/* waitdisk - wait for disk ready */
static void
waitdisk(void) {
	while((inb(0x1F7) & 0xC0) != 0x40)
		/* do nothing */;
}

/* readsect - read a single sector at @secno into @dst */
static void
readsect(void *dst, uint32_t secno) {
	// wait for disk to be ready
	waitdisk();

	outb(0x1F2, 1);                         // count = 1
    outb(0x1F3, secno & 0xFF);
    outb(0x1F4, (secno >> 8) & 0xFF);
    outb(0x1F5, (secno >> 16) & 0xFF);
    outb(0x1F6, ((secno >> 24) & 0xF) | 0xE0);
    outb(0x1F7, 0x20);                      // cmd 0x20 - read sectors

    // wait for disk to be ready
    waitdisk();

    // read a sector
    insl(0x1F0, dst, SECTSIZE / 4);
}

/**
 * readseg - read @count bytes at @offset from kernel into virtual address @va,
 * might copy more than asked.
 **/
static void
readseg(uintptr_t va, uint32_t count, uint32_t offset) {
	uintptr_t end_va = va + count;

	// round down to sector boundary
	va -= offset % SECTSIZE;

	// translate from bytes to sectors; kernel starts at sector 1
	uint32_t secno = (offset / SECTSIZE) + 1;

	// If this is too slow, we could read lots of sectors at a time.
	// We'd write more to memory than asked, but it doesn't matter --
	// we load in increasing ording.
	for (; va < end_va; va += SECTSIZE, secno ++) {
		readsect((void *)va, secno);
	}
}

/* bootmain - the entry of bootloader */
void
bootmain(void) {
	// read the 1st page off disk
    // elf文件头信息读到 物理内存0x10000处,读一个Page的大小
	readseg((uintptr_t)ELFHDR, SECTSIZE * 8, 0);

	// is this a valid ELF?
	if (ELFHDR->e_magic != ELF_MAGIC) {
        goto bad;
    }

    struct proghdr *ph, *eph;

    /* load each program segment (ignores ph flags) */
    ph = (struct proghdr *)((uintptr_t)ELFHDR + ELFHDR->e_phoff);
    eph = ph + ELFHDR->e_phnum;

    /*  编译出来的kernel的头信息 
     *  $ readelf.exe -l kernel
     *
     *  Elf file type is EXEC (Executable file)
     *  Entry point 0xc0100000
     *  There are 2 program headers, starting at offset 52
     *
     *  Program Headers:
     *    Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
     *    LOAD           0x001000 0xc0100000 0xc0100000 0x2d4aa 0x2d4aa R E 0x1000
     *    LOAD           0x02f000 0xc012e000 0xc012e000 0x933f9 0x966a4 RW  0x1000
     *
     *   Section to Segment mapping:
     *    Segment Sections...
     *     00     .text .rodata .stab .stabstr
     *     01     .data .bss
    */
    for (;ph < eph; ph ++) {
        //从上面的信息可以得出kernel代码段放在物理地址0x100000处
        //                          数据段放在物理地址0x12e000处
    	readseg(ph->p_va & 0xFFFFFF, ph->p_memsz, ph->p_offset);
    }
    
    // call the entry point from the ELF header
    // note: does not return
    ((void (*)(void))(ELFHDR->e_entry & 0xFFFFFF))();    

bad:
    outw(0x8A00, 0x8A00);
    outw(0x8A00, 0x8E00);

    /* do nothing */
    while (1);    
}
