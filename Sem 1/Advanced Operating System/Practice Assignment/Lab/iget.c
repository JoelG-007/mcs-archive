#include <stdio.h>
#define H 3
#define N 5

typedef struct Inode{
    int ino;
    int fs;
    int busy;
    int ref;
    struct Inode *nextHash;
    struct Inode *nextFree;
}Inode;

Inode i1={10,1,0,0,NULL,NULL};
Inode i2={20,1,1,1,NULL,NULL};
Inode i3={30,1,0,0,NULL,NULL};
Inode i4={40,1,0,0,NULL,NULL};
Inode i5={50,1,0,0,NULL,NULL};

Inode *hash[H]={NULL,NULL,NULL};
Inode *freeList=NULL;

int hashFn(int ino){
    return ino%H;
}

void addHash(Inode *p){
    int h=hashFn(p->ino);
    p->nextHash=hash[h];
    hash[h]=p;
}

void removeHash(Inode *p){
    int h=hashFn(p->ino);
    Inode *q=hash[h],*prev=NULL;
    while(q!=NULL){
        if(q==p){
            if(prev==NULL)
            hash[h]=q->nextHash;
            else
            prev->nextHash=q->nextHash;
            p->nextHash=NULL;
            return;
        }
        prev=q;
        q=q->nextHash;
    }
}

void addFree(Inode *p){
    p->nextFree=freeList;
    freeList=p;
}

void removeFree(Inode *p){
    Inode *q=freeList,*prev=NULL;
    while(q!=NULL){
        if(q==p){
            if(prev==NULL)
            freeList=q->nextFree;
            else
            prev->nextFree=q->nextFree;
            p->nextFree=NULL;
            return;
        }
        prev=q;
        q=q->nextFree;
    }
}

Inode *searchHash(int ino){
    Inode *p=hash[hashFn(ino)];
    while(p!=NULL){
        if(p->ino==ino)
        return p;
        p=p->nextHash;
    }
    return NULL;
}

/*
Given an inode number, iget() first checks whether that inode is already in the hash table. 
If it is, it reuses it; if it isn't, it takes an inode from the free list, 
replaces its contents with the requested inode, and puts it into the hash table.
*/
Inode *iget(int ino){
    Inode *p;
    while(1){
        p=searchHash(ino);
        if(p!=NULL){
            if(p->busy){
                printf("Status: Inode %d is locked.\n",ino);
                printf("Status: Process sleeps...\n");
                printf("Status: Inode unlocked. Process wakes up.\n");
                p->busy=0;
                addFree(p);
                continue;
            }
            printf("Status: Inode found in cache.\n");
            removeFree(p);p->ref++;
            p->busy=1;
            return p;
        }
        if(freeList==NULL){
            printf("Status: No free inodes available.\n");
            return NULL;
        }
        p=freeList;
        removeFree(p);
        printf("Status: Inode %d not in cache.\n",ino);
        printf("Status: Allocating free in-core inode.\n");
        removeHash(p);
        p->ino=ino;
        p->fs=1;
        p->busy=1;
        p->ref=1;
        addHash(p);
        printf("Status: Reading inode %d from disk...\n",ino);
        printf("Status: Inode initialized with reference count 1.\n");
        return p;
    }
}

void display(){
    int i;
    Inode *p;
    printf("\nHash Queues:\n");
    for(i=0;i<H;i++){
        printf("H%d: ",i);
        p=hash[i];
        while(p!=NULL){
            printf("I%d(R%d,B%d) -> ",p->ino,p->ref,p->busy);
            p=p->nextHash;
        }
        printf("NULL\n");
    }
    printf("Free List: ");
    p=freeList;
    while(p!=NULL){
        printf("I%d -> ",p->ino);
        p=p->nextFree;
    }
    printf("NULL\n");
}

int main(){
    int ino;
    addHash(&i1);
    addHash(&i2);
    addHash(&i3);
    addHash(&i4);
    addHash(&i5);
    freeList=&i1;
    i1.nextFree=&i3;
    i3.nextFree=&i4;
    i4.nextFree=&i5;
    while(1){
        printf("\nEnter inode number (0 to exit): ");
        scanf("%d",&ino);
        if(ino==0)
        break;
        printf("\nBefore iget():");
        display();
        if(iget(ino)!=NULL)
        printf("\nInode %d locked and allocated successfully.\n",ino);
        printf("\nAfter iget():");
        display();
    }
    return 0;
}
/*
              iget(ino)
                  |
                  v
        Search inode in hash table
                  |
          +-------+-------+
          |               |
        Found          Not found
          |               |
          v               v
       Is busy?       Is free list empty?
       /     \           /       \
     YES      NO       YES        NO
      |        |        |          |
    Sleep   reuse    return NULL   Take free inode
      |        |                   |
   Wake up     |              Remove from hash
      |        |                   |
      +--------+              Change inode number
               |                   |
               |              Add to hash table
               |                   |
               +-------------------+
*/