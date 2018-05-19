#ifndef __LIB_PROTOBUF_H__
#define __LIB_PROTOBUF_H__

typedef struct protobuf{
      uint32_t len;
      uint32_t size;
      void* data;
} protobuf;

typedef struct protobufs{
	protobuf buf;
	protobuf *next;
}

#endif // !__LIB_PROTOBUF_H__