#include <vmm.h>
#include <sync.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <error.h>
#include <pmm.h>
#include <x86.h>
#include <swap.h>
#include <shmem.h>
//#include <kmalloc.h>
#include <slab.h>

static void check_vmm(void);
static void check_vma_struct(void);
static void check_pgfault(void);

// mm_create -  alloc a mm_struct & initialize it.
// do_fork---->copy_mm----->
// do_execve-->load_icode-->
//                          mm_create
struct mm_struct *
mm_create(void) {
    struct mm_struct *mm = kmalloc(sizeof(struct mm_struct));

    if (mm != NULL) {
        list_init(&(mm->mmap_list));
        mm->mmap_cache = NULL;
        mm->pgdir = NULL;
        mm->map_count = 0;

        if (swap_init_ok) swap_init_mm(mm);
        else mm->sm_priv = NULL;

        mm->locked_by = 0;
        mm->brk_start = mm->brk = 0;
        list_init(&(mm->proc_mm_link));
        set_mm_count(mm, 0);
        sem_init(&(mm->mm_sem), 1);
    }
    return mm;
}

// vma_create - alloc a vma_struct & initialize it. (addr range: vm_start~vm_end)
// 除了测试代码之外，这个函数只会在dup_mmap和mm_map才会调用
// copy_mm----->dup_mmap->
// load_icode-->mm_map--->
//                        vma_create
struct vma_struct *
vma_create(uintptr_t vm_start, uintptr_t vm_end, uint32_t vm_flags) {
    struct vma_struct *vma = kmalloc(sizeof(struct vma_struct));

    if (vma != NULL) {
        vma->vm_start = vm_start;
        vma->vm_end = vm_end;
        vma->vm_flags = vm_flags;
        vma->shmem = NULL;
        vma->shmem_off = 0;
    }
    return vma;
}

// vma_destroy - free vma_struct
static void
vma_destroy(struct vma_struct *vma) {
    if (vma->vm_flags & VM_SHARE) {
        if (shmem_ref_dec(vma->shmem) == 0) {
            shmem_destroy(vma->shmem);
        }
    }
    kfree(vma);
}

// find_vma - find a vma (vma->vm_start <= addr <= vma_vm_end)
struct vma_struct *
find_vma(struct mm_struct *mm, uintptr_t addr) {
    struct vma_struct *vma = NULL;
    if (mm != NULL) {
        vma =mm->mmap_cache;
        if (!(vma != NULL && vma->vm_start <= addr && vma->vm_end > addr)) {
            bool found = 0;
            list_entry_t *list = &(mm->mmap_list), *le = list;
            while((le = list_next(le)) != list) {
                vma = le2vma(le, list_link);
                if (vma->vm_start<=addr && addr < vma->vm_end) {
                        found = 1;
                        break;
                }
            }
            if (!found) {
                vma = NULL;
            }
        }
        if (vma != NULL) {
            mm->mmap_cache = vma;
        }
    }
    return vma;
}

struct vma_struct *
find_vma_intersection(struct mm_struct *mm, uintptr_t start, uintptr_t end) {
    struct vma_struct *vma = find_vma(mm, start);
    if (vma != NULL && end <= vma->vm_start) {
        vma = NULL;
    }
    return vma;
}

// check_vma_overlap - check if vma1 overlaps vma2 ?
static inline void
check_vma_overlap(struct vma_struct *prev, struct vma_struct *next) {
    assert(prev->vm_start < prev->vm_end);
    assert(prev->vm_end <= next->vm_start);
    assert(next->vm_start < next->vm_end);
}

// insert_vma_struct - insert vma in mm's list link
// vma 中的地址从小到大排序
//   copy_mmp-->dup_mmap-->
//   mm_map---->
//                         insert_vma_struct
void
insert_vma_struct(struct mm_struct *mm, struct vma_struct *vma) {
    assert(vma->vm_start < vma->vm_end);
    list_entry_t *list = &(mm->mmap_list);
    list_entry_t *le_prev = list, *le_next;

    list_entry_t *le = list;
    while((le = list_next(le)) != list) {
        struct vma_struct *mmap_prev = le2vma(le, list_link);
        if (mmap_prev->vm_start > vma->vm_start) {
            break;
        }
        le_prev = le;
    }

    le_next = list_next(le_prev);

    /* check overlap */
    if (le_prev != list) {
        check_vma_overlap(le2vma(le_prev, list_link), vma);
    }
    if (le_next != list) {
        check_vma_overlap(vma, le2vma(le_next, list_link));
    }

    vma->vm_mm = mm;
    list_add_after(le_prev, &(vma->list_link));

    mm->map_count ++;
}

// remove_vma_struct - remove vma from mm's list link
static int
remove_vma_struct(struct mm_struct *mm, struct vma_struct *vma) {
    list_del(&(vma->list_link));
    if (vma == mm->mmap_cache) {
        mm->mmap_cache = NULL;
    }
    mm->map_count --;
    return 0;
}

// mm_destroy - free mm and mm internal fields
void
mm_destroy(struct mm_struct *mm) {
    assert(mm_count(mm) == 0);

    list_entry_t *list = &(mm->mmap_list), *le;
    while((le = list_next(list)) != list) {
        list_del(le);
        kfree(le2vma(le, list_link));  //kfree vma        
    }
    kfree(mm); //kfree mm
    mm=NULL;
}


//load_icode-->
//             mm_map
//mm_map只是给proc->mm建立了vma结构
int
mm_map(struct mm_struct *mm, uintptr_t addr, size_t len, uint32_t vm_flags,
       struct vma_struct **vma_store) {
    uintptr_t start = ROUNDDOWN(addr, PGSIZE), end = ROUNDUP(addr + len, PGSIZE);
    if (!USER_ACCESS(start, end)) {
        return -E_INVAL;
    }

    assert(mm != NULL);

    int ret = -E_INVAL;

    struct vma_struct *vma;
    if ((vma = find_vma(mm, start)) != NULL && end > vma->vm_start) {
        goto out;
    }
    ret = -E_NO_MEM;

    if ((vma = vma_create(start, end, vm_flags)) == NULL) {
        goto out;
    }
    insert_vma_struct(mm, vma);
    if (vma_store != NULL) {
        *vma_store = vma;
    }

    ret = 0;

out:
    return ret;
}

int
mm_map_shmem(struct mm_struct *mm, uintptr_t addr, uint32_t vm_flags,
        struct shmem_struct *shmem, struct vma_struct **vma_store) {
    if ((addr % PGSIZE) != 0 || shmem == NULL) {
        return -E_INVAL;
    }
    int ret;
    struct vma_struct *vma;
    shmem_ref_inc(shmem);
    if ((ret = mm_map(mm, addr, shmem->len, vm_flags, &vma)) != 0) {
        shmem_ref_dec(shmem);
        return ret;
    }
    vma->shmem = shmem;
    vma->shmem_off = 0;
    vma->vm_flags |= VM_SHARE;
    if (vma_store != NULL) {
        *vma_store = vma;
    }
    return 0;
}

static void
vma_resize(struct vma_struct *vma, uintptr_t start, uintptr_t end) {
    assert(start % PGSIZE == 0 && end % PGSIZE == 0);
    assert(vma->vm_start <= start && start < end && end <= vma->vm_end);
    if (vma->vm_flags & VM_SHARE) {
        vma->shmem_off += start - vma->vm_start;
    }
    vma->vm_start = start, vma->vm_end = end;
}

int
mm_unmap(struct mm_struct *mm, uintptr_t addr, size_t len ) {
    uintptr_t start = ROUNDDOWN(addr, PGSIZE), end = ROUNDUP(addr + len, PGSIZE);
    if (!USER_ACCESS(start, end)) {
        return -E_INVAL;
    }

    assert(mm != NULL);

    struct vma_struct *vma;
    if ((vma = find_vma(mm, start)) == NULL || end <= vma->vm_start) {
        return 0;
    } 

    if (vma->vm_start < start && end < vma->vm_end) {
        struct vma_struct *nvma;
        if ((nvma = vma_create(vma->vm_start, start, vma->vm_flags)) == NULL) {
            return -E_NO_MEM;
        }
        vma_resize(vma, end, vma->vm_end);
        insert_vma_struct(mm, nvma);
        unmap_range(mm->pgdir, start, end);
        return 0;
    }

    list_entry_t free_list, *le;
    list_init(&free_list);
    while (vma->vm_start < end) {
        le = list_next(&(vma->list_link));
        remove_vma_struct(mm, vma);
        list_add(&free_list, &(vma->list_link));
        if (le == &(mm->mmap_list)) {
            break;
        }
        vma = le2vma(le, list_link);
    }

    le = list_next(&free_list);
    while (le != &free_list) {
        vma = le2vma(le, list_link);
        le = list_next(le);
        uintptr_t un_start, un_end;
        if (vma->vm_start < start) {
            un_start = start, un_end = vma->vm_end;
            vma_resize(vma, vma->vm_start, un_start);
            insert_vma_struct(mm, vma);
        }
        else {
            un_start = vma->vm_start, un_end = vma->vm_end;
            if (end < un_end) {
                un_end = end;
                vma_resize(vma, un_end, vma->vm_end);
                insert_vma_struct(mm, vma);
            }
            else {
                vma_destroy(vma);
            }
        }
        unmap_range(mm->pgdir, un_start, un_end);
    }
    return 0;
}

/*
 * 复制from的vma到to
 * sys_fork------>
 * kernel_thread->do_fork-->copy_mm-->
 *                     dup_mmap
 */
int
dup_mmap(struct mm_struct *to, struct mm_struct *from) {
    assert(to != NULL && from != NULL);
    list_entry_t *list = &(from->mmap_list), *le = list;
    //dump_mm("to first ",to);
    //dump_mm("from first ",from);
    while ((le = list_prev(le)) != list) {
        struct vma_struct *vma, *nvma;
        vma = le2vma(le, list_link);
        nvma = vma_create(vma->vm_start, vma->vm_end, vma->vm_flags);
        if (nvma == NULL) {
            return -E_NO_MEM;
        }
        else {
            if (vma->vm_flags & VM_SHARE) {
                //cprintf("copy share vma , vm_start:%x vm_end:%x \n",nvma->vm_start, nvma->vm_end);
                nvma->shmem = vma->shmem;
                nvma->shmem_off = vma->shmem_off;
                shmem_ref_inc(vma->shmem);
            }
        }
        //cprintf("copy vma , vm_start:%x vm_end:%x \n",nvma->vm_start, nvma->vm_end);
        insert_vma_struct(to, nvma);

        bool share = 0;
        if (copy_range(to->pgdir, from->pgdir, vma->vm_start, vma->vm_end, share) != 0) {
            return -E_NO_MEM;
        }
    }

    //dump_mm("to last ",to);
    //dump_mm("from last ",from);
    return 0;
}

void
exit_mmap(struct mm_struct *mm) {
    assert(mm != NULL && mm_count(mm) == 0);
    pde_t *pgdir = mm->pgdir;
    list_entry_t *list = &(mm->mmap_list), *le = list;
    while ((le = list_next(le)) != list) {
        struct vma_struct *vma = le2vma(le, list_link);
        unmap_range(pgdir, vma->vm_start, vma->vm_end);
    }
    while ((le = list_next(le)) != list) {
        struct vma_struct *vma = le2vma(le, list_link);
        exit_range(pgdir, vma->vm_start, vma->vm_end);
    }
}

void dump_mm(char* str, struct mm_struct* mm) {
    cprintf("dump_vma : %s \n",str);
    list_entry_t *list = &(mm->mmap_list), *le = list;
    while ((le = list_next(le)) != list) { 
        struct vma_struct *vma = le2vma(le, list_link);
        //cprintf(" dump vm_start:%x   vm_end:%x\n",vma->vm_start, vma->vm_end);
        cprintf("vm_start : %x page: %x \n",vma->vm_start, kva2page((void *)vma->vm_start));
    } 
    cprintf("dump_vma end\n");
}

/*
 * 选择一个区域，用来map，从用户态内存高地址开始
 *
 */
uintptr_t
get_unmapped_area(struct mm_struct *mm, size_t len) {
    if (len == 0 || len > USERTOP) {
        return 0;
    }
    uintptr_t start = USERTOP - len;
    list_entry_t *list = &(mm->mmap_list), *le = list;
    while((le = list_prev(le)) != list ) {
        struct vma_struct *vma = le2vma(le, list_link);
        if (start >= vma->vm_end) {
            return start;
        }
        if (start + len > vma->vm_start) {
            if (len >= vma->vm_start) {
                return 0;
            }
            start = vma->vm_start - len;
        }
    }
    return (start >= USERBASE) ? start : 0;
}

bool
copy_from_user(struct mm_struct *mm, void *dst, const void *src, size_t len, bool writable) {
    if (!user_mem_check(mm, (uintptr_t)src, len, writable)) {
        return 0;
    }
    memcpy(dst, src, len);
    return 1;
}

bool
copy_to_user(struct mm_struct *mm, void *dst, const void *src, size_t len) {
    if (!user_mem_check(mm, (uintptr_t)dst, len, 1)) {
        return 0;
    }
    memcpy(dst, src, len);
    return 1;
}

// vmm_init - initialize virtual memory management
//          - now just call check_vmm to check correctness of vmm
void
vmm_init(void) {
    check_vmm();
}

// check_vmm - check correctness of vmm
static void
check_vmm(void) {
    check_vma_struct();
    check_pgfault();

    //assert(nr_free_pages_store == nr_free_pages());

    cprintf("check_vmm() succeeded.\n");
}

static void
check_vma_struct(void) {
    //size_t nr_free_pages_store = nr_free_pages();

    struct mm_struct *mm = mm_create();
    assert(mm != NULL);

    int step1 = 10, step2 = step1 * 10;

    int i;
    for (i = step1; i >= 1; i --) {
        struct vma_struct *vma = vma_create(i * 5, i * 5 + 2, 0);
        assert(vma != NULL);
        insert_vma_struct(mm, vma);
    }

    for (i = step1 + 1; i <= step2; i ++) {
        struct vma_struct *vma = vma_create(i * 5, i * 5 + 2, 0);
        assert(vma != NULL);
        insert_vma_struct(mm, vma);
    }

    list_entry_t *le = list_next(&(mm->mmap_list));

    for (i = 1; i <= step2; i ++) {
        assert(le != &(mm->mmap_list));
        struct vma_struct *mmap = le2vma(le, list_link);
        assert(mmap->vm_start == i * 5 && mmap->vm_end == i * 5 + 2);
        le = list_next(le);
    }

    for (i = 5; i <= 5 * step2; i +=5) {
        struct vma_struct *vma1 = find_vma(mm, i);
        assert(vma1 != NULL);
        struct vma_struct *vma2 = find_vma(mm, i+1);
        assert(vma2 != NULL);
        struct vma_struct *vma3 = find_vma(mm, i+2);
        assert(vma3 == NULL);
        struct vma_struct *vma4 = find_vma(mm, i+3);
        assert(vma4 == NULL);
        struct vma_struct *vma5 = find_vma(mm, i+4);
        assert(vma5 == NULL);

        assert(vma1->vm_start == i  && vma1->vm_end == i  + 2);
        assert(vma2->vm_start == i  && vma2->vm_end == i  + 2);
    }

    for (i =4; i>=0; i--) {
        struct vma_struct *vma_below_5= find_vma(mm,i);
        if (vma_below_5 != NULL ) {
           cprintf("vma_below_5: i %x, start %x, end %x\n",i, vma_below_5->vm_start, vma_below_5->vm_end); 
        }
        assert(vma_below_5 == NULL);
    }

    mm_destroy(mm);

    //assert(nr_free_pages_store == nr_free_pages());

    cprintf("check_vma_struct() succeeded!\n");
}

struct mm_struct *check_mm_struct;

// check_pgfault - check correctness of pgfault handler
static void
check_pgfault(void) {
    size_t nr_free_pages_store = nr_free_pages();

    check_mm_struct = mm_create();
    assert(check_mm_struct != NULL);

    struct mm_struct *mm = check_mm_struct;
    pde_t *pgdir = mm->pgdir = boot_pgdir;
    assert(pgdir[0] == 0);

    struct vma_struct *vma = vma_create(0, PTSIZE, VM_WRITE);
    assert(vma != NULL);

    insert_vma_struct(mm, vma);
    cprintf("vma.start:%x vma.end:%x\n", vma->vm_start,vma->vm_end);
    uintptr_t addr = 0x100;
    assert(find_vma(mm, addr) == vma);
    
    int i, sum = 0;
    for (i = 0; i < 100; i ++) {
        //cprintf("abc\n");
        *(char *)(addr + i) = i;
        //cprintf("bcd\n");
        //panic("debug exit");
        sum += i;
    }
    for (i = 0; i < 100; i ++) {
        sum -= *(char *)(addr + i);
    }
    assert(sum == 0);

    page_remove(pgdir, ROUNDDOWN(addr, PGSIZE));
    free_page(pde2page(pgdir[0]));
    pgdir[0] = 0;

    mm->pgdir = NULL;
    mm_destroy(mm);
    check_mm_struct = NULL;

    assert(nr_free_pages_store == nr_free_pages());

    cprintf("check_pgfault() succeeded!\n");
}

volatile unsigned int pgfault_num=0;

int
do_pgfault(struct mm_struct *mm, uint32_t error_code, uintptr_t addr) {
    int ret = -E_INVAL;
    //cprintf("===========================\n");
    struct vma_struct *vma = find_vma(mm, addr);
    
    pgfault_num ++;
    
    // If the addr is in the range of a mm's vma
    if (vma == NULL || vma->vm_start > addr) {
        cprintf("not valid addr %x, and  can not find it in vma\n", addr);
        goto failed;
    }
    //cprintf("pgfault: vm_start:%x  vm_end:%x   error_code:%x\n",vma->vm_start,vma->vm_end,error_code);
    switch (error_code & 3) {
        default:
                /* error code flag : default is 3 ( W/R=1, P=1): write, present */
        case 2: /* 
                   error code flag : (W/R=1, P=0):write, not present
                   写内存，但是没有映射到物理内存 
                */
            if (!(vma->vm_flags & VM_WRITE)) {
                // vma对应的权限不对，不可写
                //cprintf("do_pgfault failed: error code flag = write AND not present, but the addr's vma cannot write\n");
                goto failed;
            }
            break;
        case 1: /* 
                 error code flag : (W/R=0, P=1): read, present 
                 */
                 //cprintf("do_pgfault failed: error code flag = read AND present\n");
            goto failed;
        case 0: /* 
                   error code flag : (W/R=0, P=0): read, not present 
                   读内存，但是没有映射到物理内存
                */
            if (!(vma->vm_flags & (VM_READ | VM_EXEC))) {
                // vma对应的权限不对，不可读、执行
                //cprintf("do_pgfault failed: error code flag = read AND not present, but the addr's vma cannot read or exec\n");
                goto failed;
            }
    }
    /* IF (write an existed addr ) OR
     *    (write an non_existed addr && addr is writable) OR
     *    (read  an non_existed addr && addr is readable)
     * THEN
     *    continue process
     */
    uint32_t perm = PTE_U;
    if (vma->vm_flags & VM_WRITE) {
        perm |= PTE_W;
    }
    addr = ROUNDDOWN(addr, PGSIZE);

    ret = -E_NO_MEM;

    pte_t *ptep=NULL;

    // try to find a pte, if pte's PT(Page Table) isn't existed, then create a PT.
    // (notice the 3th parameter '1')
    //设置 virtual addr到相应的 pte
    if ((ptep = get_pte(mm->pgdir, addr, 1)) == NULL) {
        cprintf("get_pte in do_pgfault failed\n");
        goto failed;
    }
    //cprintf("!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    //if(addr>= 0x2000 && addr <0x3000){
    //    cprintf("*ptep:%x\n",(*ptep));
    //}
    

    if (*ptep == 0) { 
        // (*ptep == 0)说明没有映射到相应的物理内存
        // if the phy addr isn't exist, 
        // then alloc a page & map the phy addr with logical addr
        
        // 给线性地址addr分配一个 page和相应的物理内存frame
        if (pgdir_alloc_page(mm->pgdir, addr, perm) == NULL) {
            cprintf("pgdir_alloc_page in do_pgfault failed\n");
            goto failed;
        }
        //todo shmem
        
    }
    else { 
        struct Page *page=NULL;
        //cprintf("do pgfault: ptep %x, pte %x\n",ptep, *ptep);
        if (*ptep & PTE_P) {
            // 写只读的内存
            //if process write to this existed readonly page (PTE_P means existed), then should be here now.
            //we can implement the delayed memory space copy for fork child process (AKA copy on write, COW).
            //we didn't implement now, we will do it in future.
            panic("error write a non-writable pte");
            //page = pte2page(*ptep);
        } else{
            /* 
               *ptep !=0 并且 present位 等于 0
               说明当前线性地址对应的物理内存已经被换到磁盘上去了，
               需要换入才可以正常运行
            */
            // if this pte is a swap entry, then load data from disk to a page with phy addr
            // and call page_insert to map the phy addr with logical addr
            if(swap_init_ok) {               
               if ((ret = swap_in(mm, addr, &page)) != 0) {
                   cprintf("swap_in in do_pgfault failed\n");
                   goto failed;
               }    

            }  
            else {
               cprintf("no swap_init_ok but ptep is %x, failed\n",*ptep);
               goto failed;
            }
       }
       page_insert(mm->pgdir, page, addr, perm);
       //可以swapin swapout
       swap_map_swappable(mm, addr, page, 1);
       //记录本page所指示物理帧的线性地址
       page->pra_vaddr = addr;
   }
   ret = 0;
failed:
    return ret;
}

bool
user_mem_check(struct mm_struct *mm, uintptr_t addr, size_t len, bool write) {
    if (mm != NULL) {
        if (!USER_ACCESS(addr, addr + len)) {
            return 0;
        }
        struct vma_struct *vma;
        uintptr_t start = addr, end = addr + len;
        while (start < end) {
            if ((vma = find_vma(mm, start)) == NULL || start < vma->vm_start) {
                return 0;
            }
            if (!(vma->vm_flags & ((write) ? VM_WRITE : VM_READ))) {
                return 0;
            }
            if (write && (vma->vm_flags & VM_STACK)) {
                if (start < vma->vm_start + PGSIZE) { //check stack start & size
                    return 0;
                }
            }
            start = vma->vm_end;
        }
        return 1;
    }
    //如果是从内核态进来的，就也放过
    return KERN_ACCESS(addr, addr + len);
}

bool
copy_string(struct mm_struct *mm, char *dst, const char *src, size_t maxn) {
    size_t alen, part = ROUNDDOWN((uintptr_t)src + PGSIZE, PGSIZE) - (uintptr_t)src;
    while (1) {
        if (part > maxn) {
            part = maxn;
        }
        if (!user_mem_check(mm, (uintptr_t)src, part, 0)) {
            return 0;
        }
        if ((alen = strnlen(src, part)) < part) {
            memcpy(dst, src, alen + 1);
            return 1;
        }
        if (part == maxn) {
            return 0;
        }
        memcpy(dst, src, part);
        dst += part, src += part, maxn -= part;
        part = PGSIZE;
    }
}
