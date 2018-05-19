#ifndef __KERN_NET_TCPIP_H_QUEUE_H__
#define __KERN_NET_TCPIP_H_QUEUE_H__

#define EleType  void*
#define QMAXSIZE  20

typedef struct EleComb {
    EleType elem;
    int sort;
} EleComb;

#define DEFAULT_SORT   -1

typedef struct {
   //EleType data[QMAXSIZE];
	EleComb data[QMAXSIZE];
   int front;
   int rear;
   int count;//当前已经填的数量
   int size;
   int seen;
} queue;


queue* create_queue(int);
void destroy_queue(queue *q);
bool is_fullq(queue* q);
bool addq(queue* q, EleType item);
bool is_emptyq(queue *q);
EleType delq(queue* q);
EleType seeq(queue* q);
void freeq(queue* q);
int  qcount(queue* q);

#define deq delq
#define enq addq

#endif //!__KERN_NET_TCPIP_H_QUEUE_H__
