#include <defs.h>
#include <string.h>
#include <vfs.h>
#include <inode.h>
#include <error.h>
#include <assert.h>

/*
 * get_device - Common code to pull the device name, if any, off the front of a
 *              path and choose the inode to begin the name lookup relative to.
 *
 * node_store 存储结果，也就是相应的device inode
 * 
 * path 有 disk0:
 *         sh
           stdin:
           stdout:

           输入 ./hello
                 get_device("./hello",....)
                    get_device("/./hello",...)
 */

static int
get_device(char *path, char **subpath, struct inode **node_store) {
	int i, slash = -1, colon = -1;
	for (i = 0; path[i] != '\0'; i ++) {
		if (path[i] == ':') { colon = i; break; }
		if (path[i] == '/') { slash = i; break; }
	} 

	if (colon < 0 && slash != 0) {
		//普通的文件路径 hello 或者 ./hello
		/* *
		 * No colon before a slash, so no device name specified, and the slash isn't leading
		 * or is also absent, so this is a relative path or just a bare filename. Start from
		 * the current directory, and use the whole thing as the subpath.
		 * */
		*subpath = path;
		//返回当前路径的inode到node_store
		return vfs_get_curdir(node_store);
	}
	if (colon > 0) {
		/*
         * disk0:
         * stdin:
         * stdout:
		 */
		/* device:path - get root of device's filesystem */
		path[colon] = '\0';//把冒号:变成\0

		/* device:/path - skip slash, treat as device:path */
		//去掉 冒号后的 /
		while (path[++ colon] == '/');
        
        //subpath为去掉devname和/后的path
		*subpath = path + colon;
		//page 只剩下 devname部分了,获取设备对应的root inode
		return vfs_get_root(path, node_store);
	}

    /* *
     * we have either /path or :path
     * /path is a path relative to the root of the "boot filesystem"
     * :path is a path relative to the root of the current filesystem
     * 
     * /path 这种路径格式，是相对应与 "boot filesystem"的路径
     * :path 这种路径格式，是相对应与 当前文件系统的路径
     * */
	int ret;
	if (*path == '/') {
		//路径以 '/' 开头
		//获取bootfs的inode
		if (( ret = vfs_get_bootfs(node_store)) != 0) {
			//失败
			return ret;
		}
	}
	else {
		assert(*path == ':');
		//路径以 ':' 开头
		struct inode *node;
		//获取当前目录的inode
		if ((ret = vfs_get_curdir(&node)) != 0) {
			//失败
			return ret;
		}
		/* The current directory may not be a device, so it must have a fs. */
		assert(node->in_fs != NULL);
		//然后根据当前目录的inode得到 所在设备的root inode，就是 SFS_BLKN_ROOT 的inode
		// (node->in_fs)->in_ops->fs_get_root(node->in_fs);
		*node_store = fsop_get_root(node->in_fs);
		vop_ref_dec(node);
	}

	/* ///... or :/... */
	while (*(++ path) == '/');
	*subpath = path;
    return 0;
}

/*
 * vfs_lookup - get the inode according to the path filename
 *
 * path = disk0:
 * path = sh
 * path = stdin:
 * path = stdout:
 *
 * sys_open->
 *   sysfile_open
 *      file_open
 *          vfs_open
 *               vfs_lookup
 */
 int
 vfs_lookup(char *path, struct inode **node_store) {
 	int ret;
 	struct inode *node;
 	if ((ret = get_device(path, &path, &node)) != 0) {
 		return ret;
 	}

 	/* 
 	 * get_device的参数path如果是"stdin:","stdout:","disk0"
 	 * path在get_device之后会是\0
 	 *
 	 * 如果path是 "hello" 则出来之后 path 依旧是 "hello"
     */
 	if (*path != '\0') {
 		ret = vop_lookup(node, path, node_store);
 		vop_ref_dec(node);
 		return ret;
 	}
 	*node_store = node;
 	return 0;
 }

 /*
 * vfs_lookup_parent - Name-to-vnode translation.
 *  (In BSD, both of these are subsumed by namei().)
 *
 * sysfile_open
 *    file_open
 *       vfs_open
 *          vfs_lookup_parent
 */
int
vfs_lookup_parent(char *path, struct inode **node_store, char **endp) {
	int ret;
	struct inode *node;
	if ((ret = get_device(path, &path, &node)) != 0) {
		return ret;
	}
    ret = (*path != '\0') ? vop_lookup_parent(node, path, node_store, endp) : -E_INVAL;
	//*endp = path;
	//*node_store = node;
	return ret;
}



