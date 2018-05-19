#include <defs.h>
#include <stdio.h>
#include <string.h>
#include <vfs.h>
#include <dev.h>
#include <inode.h>
#include <sem.h>
#include <list.h>
//#include <kmalloc.h>
#include <slab.h>
#include <unistd.h>
#include <error.h>
#include <assert.h>

// device info entry in vdev_list
// 链表vdev_list中的对象
// vfs_dev_t关联着 devname,inode,fs
typedef struct {
    const char *devname;
    struct inode *devnode;
    struct fs *fs;
    bool mountable;
    list_entry_t vdev_link;
} vfs_dev_t;

#define le2vdev(le, member)                         \
    to_struct((le), vfs_dev_t, member)

static list_entry_t vdev_list;     // device info list in vfs layer
static semaphore_t vdev_list_sem;  // 互斥访问vdev_list

static void
lock_vdev_list(void) {
	down(&vdev_list_sem);
}

static void
unlock_vdev_list(void) {
	up(&vdev_list_sem);
}

void
vfs_devlist_init(void) {
	list_init(&vdev_list);
	sem_init(&vdev_list_sem, 1);
}

// vfs_cleanup - finally clean (or sync) fs
void
vfs_cleanup(void) {
	if (!list_empty(&vdev_list)) {
		lock_vdev_list();
		{
			list_entry_t *list = &vdev_list, *le = list;
			while ((le = list_next(le)) != list) {
				vfs_dev_t *vdev = le2vdev(le, vdev_link);
				if (vdev->fs != NULL) {
					fsop_cleanup(vdev->fs);
				}
			}
		}
		unlock_vdev_list();
	}
}

/*
 * vfs_get_root - Given a device name (stdin, stdout, etc.), hand
 *                back an appropriate inode.
 * devname = disk0
 * devname = stdin
 * devname = stdout
 * 以上的这三个设备对象都能够找到，
 * 因为在kern_init->fs_init->dev_init，已经被初始化了
 */
int
vfs_get_root(const char *devname, struct inode **node_store) {
	assert(devname != NULL);
	int ret = -E_NO_DEV;
	if (!list_empty(&vdev_list)) {
		lock_vdev_list();
		{
			list_entry_t *list = &vdev_list, *le = list;
			while ((le = list_next(le)) != list) {
				//转成设备对象
                vfs_dev_t *vdev = le2vdev(le, vdev_link);

                if (strcmp(devname, vdev->devname) == 0) {
                    struct inode *found = NULL;
                    if (vdev->fs != NULL) {
                        //获取root block 的inode
                        found = fsop_get_root(vdev->fs);
                    }
                    else if (!vdev->mountable) {
                        vop_ref_inc(vdev->devnode);
                        found = vdev->devnode;
                    }
                    if (found != NULL) {
                        //返回成功结果0
                        ret = 0, *node_store = found;
                    }
                    else {
                        ret = -E_NA_DEV;
                    }
                    break;
                }
			}
		}
		unlock_vdev_list();
	}
	return ret;
}

/*
 * vfs_get_devname - Given a filesystem, hand back the name of the device it's mounted on.
 */
const char *
vfs_get_devname(struct fs *fs) {
    assert(fs != NULL);
    list_entry_t *list = &vdev_list, *le = list;
    while ((le = list_next(le)) != list) {
        vfs_dev_t *vdev = le2vdev(le, vdev_link);
        if (vdev->fs == fs) {
            return vdev->devname;
        }
    }
    return NULL;
}

/*
 * check_devname_confilct - Is there alreadily device which has the same name?
 */
static bool
check_devname_conflict(const char *devname) {
    list_entry_t *list = &vdev_list, *le = list;
    while ((le = list_next(le)) != list) {
        vfs_dev_t *vdev = le2vdev(le, vdev_link);
        if (strcmp(vdev->devname, devname) == 0) {
            return 0;
        }
    }
    return 1;
}


/*
* vfs_do_add - Add a new device to the VFS layer's device table.
*
* If "mountable" is set, the device will be treated as one that expects
* to have a filesystem mounted on it, and a raw device will be created
* for direct access.
* 添加设备到vdev_list中去
*/
static int
vfs_do_add(const char *devname, struct inode *devnode, struct fs *fs, bool mountable) {
    assert(devname != NULL);
    assert((devnode == NULL && !mountable) || (devnode != NULL && check_inode_type(devnode, device)));
    if (strlen(devname) > FS_MAX_DNAME_LEN) {
        return -E_TOO_BIG;
    }

    int ret = -E_NO_MEM;
    char *s_devname;
    if ((s_devname = strdup(devname)) == NULL) {
        return ret;
    }

    vfs_dev_t *vdev;
    if ((vdev = kmalloc(sizeof(vfs_dev_t))) == NULL) {
        goto failed_cleanup_name;
    }

    ret = -E_EXISTS;
    lock_vdev_list();
    //确保没有没有名称冲突,一个设备名称只能有一个device
    if (!check_devname_conflict(s_devname)) {
        unlock_vdev_list();
        goto failed_cleanup_vdev;
    }
    vdev->devname = s_devname;
    vdev->devnode = devnode;
    vdev->mountable = mountable;
    vdev->fs = fs;

    list_add(&vdev_list, &(vdev->vdev_link));
    unlock_vdev_list();
    return 0;

failed_cleanup_vdev:
    kfree(vdev);
failed_cleanup_name:
    kfree(s_devname);
    return ret;
}

/*
 * vfs_add_fs - Add a new fs,  by name. See  vfs_do_add information for the description of
 *              mountable.
 */
int
vfs_add_fs(const char *devname, struct fs *fs) {
    return vfs_do_add(devname, NULL, fs, 0);
}

/*
 * vfs_add_dev - Add a new device, by name. See  vfs_do_add information for the description of
 *               mountable.
 */
int
vfs_add_dev(const char *devname, struct inode *devnode, bool mountable) {
    return vfs_do_add(devname, devnode, NULL, mountable);
}

/*
 * find_mount - Look for a mountable device named DEVNAME.
 *              Should already hold vdev_list lock.
 * 查找名称为 devname的mountable的设备
 */
static int
find_mount(const char *devname, vfs_dev_t **vdev_store) {
    assert(devname != NULL);
    list_entry_t *list = &vdev_list, *le = list;
    while ((le = list_next(le)) != list) {
        vfs_dev_t *vdev = le2vdev(le, vdev_link);
        if (vdev->mountable && strcmp(vdev->devname, devname) == 0) {
            *vdev_store = vdev;
            return 0;
        }
    }
    return -E_NO_DEV;
}

/*
 * vfs_mount - Mount a filesystem. Once we've found the device, call MOUNTFUNC to
 *             set up the filesystem and hand back a struct fs.
 *
 * The DATA argument is passed through unchanged to MOUNTFUNC.
 */
int
vfs_mount(const char *devname, int (*mountfunc)(struct device *dev, struct fs **fs_store)) {
    int ret;
    lock_vdev_list();
    vfs_dev_t *vdev;
    if ((ret = find_mount(devname, &vdev)) != 0) {
        //失败
        goto out;
    }

    //
    if (vdev->fs != NULL) {
        ret = -E_BUSY;
        goto out;
    }
    assert(vdev->devname != NULL && vdev->mountable);
    
    //dev=vdev->devnode->in_info.__device_info
    struct device *dev = vop_info(vdev->devnode, device);
    if ((ret = mountfunc(dev, &(vdev->fs))) == 0) {
        //成功
        assert(vdev->fs != NULL);
        cprintf("vfs: mount %s.\n", vdev->devname);
    }

out:
    unlock_vdev_list();
    return ret;
}

/*
 * vfs_unmount - Unmount a filesystem/device by name.
 *               First calls FSOP_SYNC on the filesystem; then calls FSOP_UNMOUNT.
 */
int
vfs_unmount(const char *devname) {
    int ret;
    lock_vdev_list();
    vfs_dev_t *vdev;
    if ((ret = find_mount(devname, &vdev)) != 0) {
        goto out;
    }
    if (vdev->fs == NULL) {
        ret = -E_INVAL;
        goto out;
    }
    assert(vdev->devname != NULL && vdev->mountable);

    if ((ret = fsop_sync(vdev->fs)) != 0) {
        goto out;
    }
    if ((ret = fsop_unmount(vdev->fs)) == 0) {
        vdev->fs = NULL;
        cprintf("vfs: unmount %s.\n", vdev->devname);
    }

out:
    unlock_vdev_list();
    return ret;
}

/*
 * vfs_unmount_all - Global unmount function.
 */
int
vfs_unmount_all(void) {
    if (!list_empty(&vdev_list)) {
        lock_vdev_list();
        {
            list_entry_t *list = &vdev_list, *le = list;
            while ((le = list_next(le)) != list) {
                vfs_dev_t *vdev = le2vdev(le, vdev_link);
                if (vdev->mountable && vdev->fs != NULL) {
                    int ret;
                    if ((ret = fsop_sync(vdev->fs)) != 0) {
                        cprintf("vfs: warning: sync failed for %s: %e.\n", vdev->devname, ret);
                        continue ;
                    }
                    if ((ret = fsop_unmount(vdev->fs)) != 0) {
                        cprintf("vfs: warning: unmount failed for %s: %e.\n", vdev->devname, ret);
                        continue ;
                    }
                    vdev->fs = NULL;
                    cprintf("vfs: unmount %s.\n", vdev->devname);
                }
            }
        }
        unlock_vdev_list();
    }
    return 0;
}

