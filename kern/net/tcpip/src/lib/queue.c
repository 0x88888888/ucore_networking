#include <tcpip/h/network.h>

queue* create_queue(int c) {
    queue* q=(queue*)kmalloc(sizeof(queue));
    bzero(q, sizeof(queue));
    if(!q){
      cprintf("can't create q\n");
      return NULL;
    }
    q->front = -1;  
    q->rear = -1;  
    q->seen = -1;
    q->count = 0;
    q->size = QMAXSIZE; //c>QMAXSIZE:c,QMAXSIZE;
    return q;  
}

bool is_fullq(queue* q) {
   return (q->count == q->size);
}

bool is_emptyq(queue *q) {
    return (q->count == 0);
}

int  qcount(queue* q) {
    return q->count;
}

bool addq(queue* q, EleType item) {
    if (is_fullq(q)) {
        cprintf("can't add new element to the queue\n");
        return false;
    }
    
    /*
    if (q->count > 0) {
        EleComb* pfrom = q->data[q->rear];
        EleComb* pto =pfrom++;
        
        int i =0;
        for (i=0;i<count;i++){

            if(pfrom > &(q->data[q->size]) ) {
                pfrom = (EleComb*)q->data;//调整到第一个
            } 
            
            if(pto > &(q->data[q->size]) ) {
                pto = (EleComb*)q->data;//调整到第一个
            }  

            pto->elem = pfrom->elem;
            pto->sort = pfrom->sort;

            pto = pfrom;
            pfrom--;
        }
    } else{
    } 
    */

    q->rear++;  
    q->rear %= q->size;//QMAXSIZE;  
    q->count = q->count + 1;  
    q->data[q->rear].elem = item; 
    
    return true;
}

EleType delq(queue* q) {
    EleComb* result=NULL;
    EleType  ret;
    if(is_emptyq(q)) {
        return NULL;
    }
    q->front++;  
    q->front %= q->size; //QMAXSIZE;
    q->count = q->count - 1;  

    result = &(q->data[q->front]);

    ret=result->elem;
    result->elem = NULL;
    cprintf("delq ,elem=%x\n",ret);
    return ret;  
}

//有bug的
EleType seeq(queue* q) {
    EleType result;
    q->seen++;
    if (q->seen >= q->count) {
        q->seen = -1;
        return NULL;
    }
    result=(q->data[q->seen].elem);

    return result;
}

void freeq(queue* q) {
    EleType d;
    while((d=delq(q))) {
        freebuf(d);
        //d.elem=NULL;
        //d.sort = -1;
    }
    assert(is_emptyq(q));
}

void destroy_queue(queue *q) {
    kfree(q);
}
