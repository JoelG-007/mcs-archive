#include <stdio.h>
#define H 3
typedef struct Buffer{
    int block;
    int busy;
    int delayed;
    struct Buffer *nextHash;
    struct Buffer *nextFree;
}Buffer;

Buffer b1={10,0,0,NULL,NULL};   //S1
Buffer b2={20,1,0,NULL,NULL};   //S5
Buffer b3={30,0,1,NULL,NULL};   //S3
Buffer b4={40,0,0,NULL,NULL};
Buffer b5={50,0,0,NULL,NULL};
                                // Requested - 60, not in hash, use free buffer S2
                                // Free List Empty S4

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
void removeHash(Buffer *b){
    int h=hashFn(b->block);
    Buffer *p=hash[h],*prev=NULL;
    while(p!=NULL){
        if(p==b){
            if(prev==NULL)
            hash[h]=p->nextHash;
            else
            prev->nextHash=p->nextHash;
            b->nextHash=NULL;
            return;
        }
        prev=p;
        p=p->nextHash;
    }
}
void addFree(Buffer *b){
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

void removeFree(Buffer *b){
    Buffer *p=freeList,*prev=NULL;
    while(p!=NULL){
        if(p==b){
            if(prev==NULL)
            freeList=p->nextFree;
            else
            prev->nextFree=p->nextFree;
            b->nextFree=NULL;
            return;
        }
        prev=p;
        p=p->nextFree;
    }
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

Buffer *getblk(int block){
    Buffer *p;
    while(1){
        p=searchHash(block);
        if(p!=NULL){
            if(p->busy){
                printf("Scenario 5: Buffer is busy.\n");
                printf("Process sleeps...\n");
                printf("Buffer becomes free. Process wakes up.\n");
                p->busy=0;
                addFree(p);
                continue;
            }
            printf("Scenario 1: Block found and buffer is free.\n");p->busy=1;
            removeFree(p);
            return p;
        }
        if(freeList==NULL){
            printf("Scenario 4: Free list is empty.\n");
            printf("Process sleeps...\n");
            printf("No buffer became free in simulation.\n");
            return NULL;
        }
        p=freeList;
        removeFree(p);
        if(p->delayed){
            printf("Scenario 3: Delayed-write buffer found.\n");
            printf("Asynchronous write to disk...\n");
            p->delayed=0;
            addFree(p);
            continue;
        }
        printf("Scenario 2: Free buffer allocated.\n");
        removeHash(p);
        p->block=block;
        p->busy=1;
        addHash(p);
        return p;
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
            printf("B%d(B%d,D%d) -> ",p->block,p->busy,p->delayed);
            p=p->nextHash;
        }
        printf("NULL\n");
    }
    printf("Free List: ");
    p=freeList;
    while(p!=NULL){
        printf("B%d -> ",p->block);p=p->nextFree;
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
    freeList=&b3;
    b3.nextFree=&b4;
    b4.nextFree=&b5;
    while(1){
        printf("\nEnter block number (0 to exit): ");
        scanf("%d",&block);
        if(block==0)
        break;
        printf("\nBefore getblk():");
        display();
        if(getblk(block)!=NULL)
        printf("\nBuffer allocated successfully.\n");
        printf("\nAfter getblk():");
        display();
    }
    return 0;
}