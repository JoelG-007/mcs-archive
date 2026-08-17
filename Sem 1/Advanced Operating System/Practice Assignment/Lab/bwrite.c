#include <stdio.h>
#define H 3
typedef struct Buffer{
    int block;
    int busy;
    int delayed;
    int sync;
    struct Buffer *nextHash;
    struct Buffer *nextFree;
}Buffer;

// bwrite() -> sync = 1 -> perform write -> process waits -> I/O completes -> brelse() -> busy = 0 -> add to free list
Buffer b1={10,1,0,1,NULL,NULL}; 
// bwrite() -> sync = 0 -> delayed = 1 -> start asynchronous write -> delayed = 0 -> busy = 0 -> add to free list
Buffer b2={20,1,1,0,NULL,NULL};
// bwrite() -> sync = 0 -> delayed = 0 -> buffer remains allocated
Buffer b3={30,1,0,0,NULL,NULL};

Buffer b4={40,0,0,0,NULL,NULL};
Buffer b5={50,0,0,0,NULL,NULL};

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

void removeFree(Buffer *b){
    Buffer *p=freeList,*prev=NULL;
    while(p!=NULL){
        if(p==b){
            if(prev==NULL)
            freeList=p->nextFree;
            else
            prev->nextFree=p->nextFree;b->nextFree=NULL;
            return;
        }
        prev=p;
        p=p->nextFree;
    }
}

void addFreeHead(Buffer *b){
    b->nextFree=freeList;
    freeList=b;
}

void brelse(Buffer *b){
    b->busy=0;
    addFreeHead(b);
}

void bwrite(Buffer *b){
    if(b==NULL){
        printf("Buffer not found.\n");
        return;
    }
    if(!b->busy){
        printf("Buffer is already free.\n");
        return;
    }
    printf("Initiating disk write for block %d...\n",b->block);
    if(b->sync){
        printf("I/O is synchronous.\n");
        printf("Process sleeps until I/O completes...\n");
        printf("I/O complete. Process wakes up.\n");
        b->delayed=0;
        brelse(b);
        printf("Buffer released using brelse().\n");
    }else if(b->delayed){
        printf("I/O is asynchronous.\n");
        printf("Buffer is marked for delayed write.\n");
        b->delayed=0;
        b->busy=0;
        addFreeHead(b);
        printf("Buffer placed at HEAD of free list.\n");
    }else{
        printf("I/O is asynchronous and buffer is not delayed.\n");
        printf("Buffer remains allocated.\n");
    }
}

void display(){
    int i;
    Buffer *p;
    printf("\nHash Queues:\n");
    for(i=0;i<H;i++){
        printf("H%d: ",i);
        p=hash[i];
        while(p!=NULL){
            printf("B%d(B%d,D%d,S%d) -> ",p->block,p->busy,p->delayed,p->sync);
            p=p->nextHash;
        }
        printf("NULL\n");
    }
    printf("Free List: ");
    p=freeList;
    while(p!=NULL){
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
        printf("\nBefore bwrite():");
        display();
        bwrite(searchHash(block));
        printf("\nAfter bwrite():");
        display();
    }
    return 0;
}