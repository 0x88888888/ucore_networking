#ifndef __KERN_FS_IOBUF_H__
#define __KERN_FS_IOBUF_H__

#include <defs.h>

/*
 * iobuf is a buffer Rd/Wr status record
 */
struct iobuf {
	void *io_start;    // buf的起始位置
    void *io_base;     // the base addr of buffer (used for Rd/Wr)
    off_t io_offset;   // current Rd/Wr position in buffer, will have been incremented
    size_t io_len;     // the length of buffer (used for Rd/Wr)
    size_t io_resid;   // 空闲空间的大小 current resident length need to Rd/Wr, will have been decremented by the amount transferred.
};

#define iobuf_used(iob)                        ((size_t)((iob)->io_len - (iob)->io_resid))

struct iobuf *iobuf_init(struct iobuf *iob, void *base, size_t len, off_t offset);

int iobuf_move(struct iobuf *iob, void *data, size_t len, bool m2b, size_t *copiedp);
int iobuf_move_zeros(struct iobuf *iob, size_t len, size_t *copiedp);
void iobuf_skip(struct iobuf *iob, int32_t n);
bool iobuf_at_start(struct iobuf* iob);

#endif /* !__KERN_FS_IOBUF_H__ */

