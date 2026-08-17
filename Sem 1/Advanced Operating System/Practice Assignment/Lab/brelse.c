#include <stdio.h>
#define H 3
typedef struct Buffer{
    int block;
    int busy;
    int valid;
    int old;
    struct Buffer *nextHash;
    struct Buffer *nextFree;
}Buffer;

Buffer b1={10,1,1,0,NULL,NULL};
Buffer b2={20,1,0,0,NULL,NULL};
Buffer b3={30,1,1,1,NULL,NULL};
Buffer b4={40,0,1,0,NULL,NULL};
Buffer b5={50,0,0,1,NULL,NULL};

Buffer *hash[H]={NULL,NULL,NULL};
Buffer *freeList=NULL;

int hashFn(int block){
    return block%H;
}

void addHash(Buffer *b){
    int h=hashFn(b->block);
    b->nextHash=hash[h];
    hash[h]=b;
}

Buffer *searchHash(int block){
    Buffer *p=hash[hashFn(block)];
    while(p!=NULL){
        if(p->block==block)
        return p;
        p=p->nextHash;
    }
    return NULL;
}

void addFreeEnd(Buffer *b){
    b->nextFree=NULL;
    if(freeList==NULL){
        freeList=b;
        return;
    }
    Buffer *p=freeList;
    while(p->nextFree!=NULL)
    p=p->nextFree;
    p->nextFree=b;
}

void addFreeHead(Buffer *b){
    b->nextFree=freeList;
    freeList=b;
}

void brelse(Buffer *b){
    if(b==NULL){
        printf("Buffer not found.\n");
        return;
    }
    if(!b->busy){
        printf("Buffer is already unlocked.\n");
        return;
    }
    printf("Wakeup: processes waiting for any free buffer.\n");
    printf("Wakeup: processes waiting for this buffer.\n");
    printf("Processor execution level raised; interrupts blocked.\n");
    
    // Valid + not old -> END of free list
    // Invalid OR old -> BEGINNING of free list
    if(b->valid&&!b->old){
        printf("Buffer contents are valid and buffer is not old.\n");
        addFreeEnd(b);
        printf("Buffer added to END of free list.\n");
    }else{
        printf("Buffer is invalid or old.\n");
        addFreeHead(b);
        printf("Buffer added to BEGINNING of free list.\n");
    }
    printf("Processor execution level lowered; interrupts enabled.\n");
    b->busy=0;
    printf("Buffer unlocked.\n");
}

void display(){
    int i;
    Buffer *p;
    printf("\nHash Queues:\n");
    for(i=0;i<H;i++){
        printf("H%d: ",i);
        p=hash[i];
        while(p!=NULL){
            printf("B%d(V%d,O%d,B%d) -> ",p->block,p->valid,p->old,p->busy);
            p=p->nextHash;
        }
        printf("NULL\n");
    }
    printf("Free List: ");
    p=freeList;while(p!=NULL){
        printf("B%d -> ",p->block);
        p=p->nextFree;
    }
    printf("NULL\n");
}

int main(){
    int block;
    addHash(&b1);
    addHash(&b2);
    addHash(&b3);
    addHash(&b4);
    addHash(&b5);
    freeList=&b4;
    b4.nextFree=&b5;
    while(1){
        printf("\nEnter block number (0 to exit): ");
        scanf("%d",&block);
        if(block==0)
        break;
        printf("\nBefore brelse():");
        display();
        brelse(searchHash(block));
        printf("\nAfter brelse():");
        display();
    }
    return 0;
}
/*
             brelse(buffer)
                    |
                    v
          Is buffer NULL?
             /          \
           YES           NO
           |              |
       "Not found"     Is busy?
                          /   \
                        NO     YES
                        |       |
                   "Already    Check
                   unlocked"   valid/old
                                |
                    +-----------+-----------+
                    |                       |
             valid=1, old=0         invalid OR old=1
                    |                       |
                    v                       v
               ADD TO END              ADD TO HEAD
                    |                       |
                    +-----------+-----------+
                                |
                                v
                         busy = 0
                         Buffer unlocked
*/