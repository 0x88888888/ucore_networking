文件系统的抽象层次
  fs [bootfs_inode]
    sfs_fs __sfs_info
        device                   *dev
        bitmap                   *freemap
        list_entry_t(inode)      inode_list
        list_entry_t(inode)      hash_list
                                 inode
                                        fs         *in_fs
                                        inode_ops  in_ops
                                        device     __device_info
                                        sfs_inode  __sfs_inode_info


  device 
       size_t d_blocks;
       size_t d_blocksize;

  sfs_inode 
       sfs_disk_inode       *din;
       list_entry_t         inode_link;     /* entry for linked-list in sfs_fs */
       list_entry_t         hash_link;      /* entry for hash linked-list in sfs_fs */


struct fs{
	union {
        struct sfs_fs __sfs_info;                   
    } fs_info;                                     // filesystem-specific data 
    enum {
        fs_type_sfs_info,
    } fs_type;                                     // filesystem type 
    int (*fs_sync)(struct fs *fs);                 // Flush all dirty buffers to disk 
    struct inode *(*fs_get_root)(struct fs *fs);   // Return root inode of filesystem.
    int (*fs_unmount)(struct fs *fs);              // Attempt unmount of filesystem.
    void (*fs_cleanup)(struct fs *fs);             // Cleanup of filesystem.???
};

/* filesystem for sfs */
struct sfs_fs {
    struct sfs_super super;                         /* on-disk superblock */
    struct device *dev;                             /* device mounted on */
    struct bitmap *freemap;                         /* blocks in use are mared 0 */
    bool super_dirty;                               /* true if super/freemap modified */
    void *sfs_buffer;                               /* buffer for non-block aligned io */
    semaphore_t fs_sem;                             /* semaphore for fs */
    semaphore_t io_sem;                             /* semaphore for io */
    semaphore_t mutex_sem;                          /* semaphore for link/unlink and rename */
    list_entry_t inode_list;                        /* inode linked-list */
    list_entry_t *hash_list;                        /* inode hash linked-list */
};



struct inode {
    //区分当前的inode的inode_ops是sfs_inode还是 device
    union {
        struct device __device_info;
        /*
          区分sfs_inode是代表 
          SFS_TYPE_FILE、
          SFS_TYPE_DIR、
          SFS_TYPE_LINK、
          要根据__sfs_inode_info.din->type来区分
          不同的type有不同的inode_ops
        */
        struct sfs_inode __sfs_inode_info;  
    } in_info;
    enum {
        inode_type_device_info = 0x1234,
        inode_type_sfs_inode_info,
    } in_type;
    int ref_count;    // 引用计数
    int open_count;   // 打开此inode对应文件的个数
    struct fs *in_fs; // 抽象的文件系统，包含反问文件系统的函数指针
    /*
     *  in_ops取值:
     *  sfs_node_dirops  // __sfs_inode_info.din->type == SFS_TYPE_DIR
     *  sfs_node_fileops // __sfs_inode_info.din->type ==  SFS_TYPE_FILE
     *  dev_node_ops     // in_type == inode_type_device_info
     *
     */
    const struct inode_ops *in_ops; // 抽象的inode操作，包含访问inode的函数指针
};


struct device {
    size_t d_blocks;
    size_t d_blocksize;
    int (*d_open)(struct device *dev, uint32_t open_flags);
    int (*d_close)(struct device *dev);
    int (*d_io)(struct device *dev, struct iobuf *iob, bool write);
    int (*d_ioctl)(struct device *dev, int op, void *data);     
};

struct sfs_inode {
    struct sfs_disk_inode *din;                      /* on-disk inode */
    uint32_t ino;                                    /* inode number */
    bool dirty;                                      /* true if inode modified */
    int reclaim_count;                               /* 引用数量 kill inode if it hits zero */
    semaphore_t sem;                                 /* semaphore for din */
    list_entry_t inode_link;                         /* entry for linked-list in sfs_fs */
    list_entry_t hash_link;                          /* entry for hash linked-list in sfs_fs */
};

struct sfs_disk_inode {
	uint32_t size;                                  /* 如果inode表示常规文件，则size表示文件的大小 */
    uint16_t type;                                  /* inode代表的是文件(SFS_TYPE_FILE)、目录(SFS_TYPE_DIR)、连接文件(SFS_TYPE_LINK) */
    uint16_t nlinks;                                /* 此inode的硬连接数 */
    /*对于普通文件，索引值指向的 block 中保存的是文件中的数据。
     *而对于目录，
     *索引值指向的数据保存的是目录下所有的文件名以及对应的索引节点所在的索引块(sfs_disk_entry)
     */
    uint32_t blocks;                                /* inode的数据块的个数 */
    uint32_t direct[SFS_NDIRECT];                   /* inode的直接数据块索引值(有SFS_NDIRECT) */
    uint32_t indirect;                              /* inode的一级间接数据块索引值 */
};


struct inode_ops {
    unsigned long vop_magic;
    int (*vop_open)(struct inode *node, uint32_t open_flags);
    int (*vop_close)(struct inode *node);
    int (*vop_read)(struct inode *node, struct iobuf *iob);
    int (*vop_write)(struct inode *node, struct iobuf *iob);
    int (*vop_fstat)(struct inode *node, struct stat *stat);
    int (*vop_fsync)(struct inode *node);
    int (*vop_namefile)(struct inode *node, struct iobuf *iob);
    int (*vop_getdirentry)(struct inode *node, struct iobuf *iob);
    int (*vop_reclaim)(struct inode *node);
    int (*vop_gettype)(struct inode *node, uint32_t *type_store);
    int (*vop_tryseek)(struct inode *node, off_t pos);
    int (*vop_truncate)(struct inode *node, off_t len);
    int (*vop_create)(struct inode *node, const char *name, bool excl, struct inode **node_store);
    int (*vop_lookup)(struct inode *node, char *path, struct inode **node_store);
    int (*vop_ioctl)(struct inode *node, int op, void *data);
};



static const struct inode_ops dev_node_ops = {
    .vop_magic                      = VOP_MAGIC,
    .vop_open                       = dev_open,
    .vop_close                      = dev_close,
    .vop_read                       = dev_read,
    .vop_write                      = dev_write,
    .vop_fstat                      = dev_fstat,
    .vop_ioctl                      = dev_ioctl,
    .vop_gettype                    = dev_gettype,
    .vop_tryseek                    = dev_tryseek,
    .vop_lookup                     = dev_lookup,
};


static const struct inode_ops sfs_node_dirops = {
    .vop_magic                      = VOP_MAGIC,
    .vop_open                       = sfs_opendir,
    .vop_close                      = sfs_close,
    .vop_fstat                      = sfs_fstat,
    .vop_fsync                      = sfs_fsync,
    .vop_namefile                   = sfs_namefile,
    .vop_getdirentry                = sfs_getdirentry,
    .vop_reclaim                    = sfs_reclaim,
    .vop_gettype                    = sfs_gettype,
    .vop_lookup                     = sfs_lookup,
    .vop_lookup_parent              = sfs_lookup_parent,
};

static const struct inode_ops sfs_node_fileops = {
    .vop_magic                      = VOP_MAGIC,
    .vop_open                       = sfs_openfile,
    .vop_close                      = sfs_close,
    .vop_read                       = sfs_read,
    .vop_write                      = sfs_write,
    .vop_fstat                      = sfs_fstat,
    .vop_fsync                      = sfs_fsync,
    .vop_reclaim                    = sfs_reclaim,
    .vop_gettype                    = sfs_gettype,
    .vop_tryseek                    = sfs_tryseek,
    .vop_truncate                   = sfs_truncfile,
    .vop_lookup_parent              = NULL_VOP_NOTDIR,
};




vfs_dev_t 这个结构只在文件vfs/vfsdev.c中使用
devname<-->devnode<--->fs 都是一一对应的关系
typedef struct {
    const char *devname;
    struct inode *devnode;
    struct fs *fs;
    bool mountable;
    list_entry_t vdev_link;
} vfs_dev_t;

