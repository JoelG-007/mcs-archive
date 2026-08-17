#include <stdio.h>
#define H 3
typedef struct Buffer{
    int block;
    int busy;
    int valid;
    int delayed;
    struct Buffer *nextHash;
    struct Buffer *nextFree;
}Buffer;

Buffer b1={10,0,1,0,NULL,NULL};
Buffer b2={20,0,0,0,NULL,NULL};
Buffer b3={30,0,1,1,NULL,NULL};
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

void removeHash(Buffer *b){
    int h=hashFn(b->block);Buffer *p=hash[h],*prev=NULL;
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
    b->nextFree=freeList;
    freeList=b;
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
    Buffer *p;while(1){
        p=searchHash(block);
        if(p!=NULL){
            if(p->busy){
                printf("Buffer is busy. Process sleeps...\n");
                printf("Buffer becomes free. Process wakes up.\n");
                p->busy=0;
                addFree(p);
                continue;
            }
            p->busy=1;
            removeFree(p);
            return p;
        }
        if(freeList==NULL){
            printf("Free list is empty.\n");
            return NULL;
        }
        p=freeList;
        removeFree(p);
        if(p->delayed){
            printf("Delayed-write buffer found.\n");
            printf("Asynchronous write to disk...\n");
            p->delayed=0;
            addFree(p);
            continue;
        }
        removeHash(p);
        p->block=block;
        p->busy=1;
        p->valid=0;
        addHash(p);
        return p;
    }
}

void brelse(Buffer *p){
    if(p==NULL)
    return;
    p->busy=0;
    addFree(p);
}

Buffer *bread(int block){
    Buffer *p=getblk(block);
    if(p==NULL)return NULL;
    if(p->valid){
        printf("Block %d already valid. Cache HIT.\n",block);
    }else{
        printf("Initiating disk read for block %d...\n",block);
        printf("Process sleeps until disk read completes...\n");
        p->valid=1;
        printf("Disk read complete. Process wakes up.\n");
    }
    return p;
}

Buffer *breada(int first,int second){
    Buffer *firstBuffer=NULL,*secondBuffer=NULL;
    int firstInCache=searchHash(first)!=NULL;
    if(!firstInCache){
        firstBuffer=getblk(first);
        if(firstBuffer==NULL)
        return NULL;
        if(!firstBuffer->valid)
        printf("Initiating disk read for first block %d...\n",first);
    }
    if(searchHash(second)==NULL){
        secondBuffer=getblk(second);
        if(secondBuffer!=NULL){
            if(secondBuffer->valid){
                printf("Second block %d is valid. Releasing buffer.\n",second);
                brelse(secondBuffer);
            }else{
                printf("Initiating asynchronous disk read for second block %d...\n",second);
                secondBuffer->valid=1;
                printf("Read-ahead block %d loaded asynchronously.\n",second);
            }
        }
    }
    if(firstInCache){
        firstBuffer=bread(first);
        return firstBuffer;
    }
    printf("Process sleeps until first block %d contains valid data...\n",first);
    firstBuffer->valid=1;
    printf("First block %d read complete. Process wakes up.\n",first);
    return firstBuffer;
}

void display(){int i;
    Buffer *p;
    printf("\nHash Queues:\n");
    for(i=0;i<H;i++){
        printf("H%d: ",i);
        p=hash[i];
        while(p!=NULL){
            printf("B%d(V%d,B%d,D%d) -> ",p->block,p->valid,p->busy,p->delayed);
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
    int first,second;
    addHash(&b1);
    addHash(&b2);
    addHash(&b3);
    addHash(&b4);
    addHash(&b5);
    freeList=&b3;
    b3.nextFree=&b4;
    b4.nextFree=&b5;
    while(1){
        printf("\nEnter first block (0 to exit): ");
        scanf("%d",&first);
        if(first==0)
            break;
        printf("Enter second block: ");
        scanf("%d",&second);
        printf("\nBefore breada():");
        display();
        if(breada(first,second)!=NULL)
            printf("\nFirst buffer returned successfully.\n");
        printf("\nAfter breada():");
        display();
    }return 0;
}