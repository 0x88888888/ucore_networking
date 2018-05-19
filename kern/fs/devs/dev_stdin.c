#include <defs.h>
#include <stdio.h>
#include <wait.h>
#include <sync.h>
#include <proc.h>
#include <sched.h>
#include <dev.h>
#include <vfs.h>
#include <iobuf.h>
#include <inode.h>
#include <unistd.h>
#include <error.h>
#include <assert.h>

#define STDIN_BUFSIZE               4096

static char stdin_buffer[STDIN_BUFSIZE];
static off_t p_rpos, p_wpos;
static wait_queue_t __wait_queue, *wait_queue = &__wait_queue;

void
dev_stdin_write(char c) {
	bool intr_flag;
	if (c != '\0') {
		local_intr_save(intr_flag);
		{
			stdin_buffer[p_wpos % STDIN_BUFSIZE] =c;
            if (p_wpos - p_rpos < STDIN_BUFSIZE) {
            	p_wpos ++;
            }
            if (!wait_queue_empty(wait_queue)) {
                //cprintf("dev_stdin_write wakeup_queue\n");
                //loop_wait_queue(wait_queue);
            	wakeup_queue(wait_queue, WT_KBD, 1);
            }
		}
		local_intr_restore(intr_flag);
	}
}

static int
dev_stdin_read(char *buf, size_t len) {
	int ret = 0;
	bool intr_flag;
	local_intr_save(intr_flag);
	{
		for (; ret < len; ret ++, p_rpos ++) {
		try_again:
            //cprintf("in dev_stdin_read loop ret=%d len=%d\n",ret,len);
		    if (p_rpos < p_wpos) {
		    	*buf ++ = stdin_buffer[p_rpos % STDIN_BUFSIZE];
		    }
		    else {
		    	wait_t __wait, *wait = &__wait;
		    	wait_current_set(wait_queue, wait, WT_KBD);
		    	local_intr_restore(intr_flag);

                //cprintf("in dev_stdin_read before schedule \n");
                //loop_wait_queue(wait_queue);
		    	schedule();
                //cprintf("in dev_stdin_read after schedule \n");

		    	local_intr_save(intr_flag);
		    	wait_current_del(wait_queue, wait);
		    	if (wait->wakeup_flags == WT_KBD) {
		    		goto try_again;
		    	}
                //cprintf("wakeup_flags: %x \n", wait->wakeup_flags);
		    	break;
		    }
		}
	}
	local_intr_restore(intr_flag);
    //cprintf("in dev_stdin_read ret:%d\n",ret);
	return ret;
}

static int
stdin_open(struct device *dev, uint32_t open_flags, uint32_t arg2) {
	if (open_flags != O_RDONLY) {
        return -E_INVAL;
    }
    return 0;
}

static int
stdin_close(struct device *dev) {
    return 0;
}

//static int
int
stdin_io(struct device *dev, struct iobuf *iob, bool write) {
    if (!write) {
        int ret;
        //读到iob->io_base中去
        if ((ret = dev_stdin_read(iob->io_base, iob->io_resid)) > 0) {
            iob->io_resid -= ret;
        }
        return ret;
    }
    return -E_INVAL;
}

static int
stdin_ioctl(struct device *dev, int op, void *data) {
    return -E_INVAL;
}

/*
 *kern_init
 *  fs_init
 *   dev_init
 *     init_device
 *        dev_init_stdin
 *
 *  设置stdin设备的状态和操作函数
 */
static void
stdin_device_init(struct device *dev) {
    dev->d_blocks = 0;
    dev->d_blocksize = 1;
    dev->d_open = stdin_open;
    dev->d_close = stdin_close;
    dev->d_io = stdin_io;
    dev->d_ioctl = stdin_ioctl;

    p_rpos = p_wpos = 0;
    wait_queue_init(wait_queue);
}

/*
 *kern_init
 *  fs_init
 *   dev_init
 *     init_device
 *       dev_init_stdin
 */
void
dev_init_stdin(void) {
    struct inode *node;
    if ((node = dev_create_inode()) == NULL) {
        panic("stdin: dev_create_node.\n");
    }
    //vop_info = node->__device_info
    //设置__device_info的状态和操作函数，真正的成为stdin设备
    //stdin_device_init(node->in_info->__device__info);
    stdin_device_init(vop_info(node, device));

    int ret;
    if ((ret = vfs_add_dev("stdin", node, 0)) != 0) {
        panic("stdin: vfs_add_dev: %e.\n", ret);
    }
}

