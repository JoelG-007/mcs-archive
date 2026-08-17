#include <stdio.h>
#define H 3

typedef struct Inode{
    int ino;
    int busy;
    int ref;
    int link;
    int accessed;
    int changed;
    int type;
    struct Inode *nextHash;
    struct Inode *nextFree;
}Inode;

Inode i1={10,1,1,2,0,0,1,NULL,NULL};
Inode i2={20,1,1,1,1,1,1,NULL,NULL};
Inode i3={30,1,1,0,0,1,1,NULL,NULL};
Inode i4={40,1,2,2,0,0,1,NULL,NULL};
Inode i5={50,0,0,1,0,0,1,NULL,NULL};

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

Inode *searchHash(int ino){
    Inode *p=hash[hashFn(ino)];
    while(p!=NULL){
        if(p->ino==ino)
        return p;
        p=p->nextHash;
    }
    return NULL;
}

void addFree(Inode *p){
    p->nextFree=NULL;
    if(freeList==NULL){
        freeList=p;
        return;
    }
    Inode *q=freeList;
    while(q->nextFree!=NULL)
    q=q->nextFree;
    q->nextFree=p;
}

/*
iput() decreases the inode's reference count. 
If references still remain, the inode stays in memory. 
If the reference count becomes zero, the inode may be freed/updated and is then placed on the free list.
*/
void iput(Inode *p){
    if(p==NULL){
        printf("Inode not found.\n");
        return;
    }
    if(!p->busy){
        printf("Inode is already unlocked.\n");
        return;
    }
    printf("Inode %d locked.\n",p->ino);
    p->ref--;
    printf("Reference count decreased to %d.\n",p->ref);
    if(p->ref>0){
        printf("Reference count is not zero. Inode remains in memory.\n");p->busy=0;
        return;
    }
    if(p->link==0){
        printf("Link count is zero.\n");
        printf("Freeing disk blocks for inode %d...\n",p->ino);
        p->type=0;
        printf("Inode freed.\n");
    }
    if(p->accessed||p->changed){
        printf("Updating inode %d on disk...\n",p->ino);
        p->accessed=0;
        p->changed=0;
    }
    p->busy=0;
    addFree(p);
    printf("Inode placed at end of free list.\n");
}

void display(){
    int i;
    Inode *p;
    printf("\nHash Queues:\n");
    for(i=0;i<H;i++){
        printf("H%d: ",i);
        p=hash[i];
        while(p!=NULL){
            printf("I%d(R%d,L%d,B%d,T%d) -> ",p->ino,p->ref,p->link,p->busy,p->type);
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
    addHash(&i3);addHash(&i4);
    addHash(&i5);
    freeList=&i5;
    while(1){
        printf("\nEnter inode number (0 to exit): ");
        scanf("%d",&ino);
        if(ino==0)
        break;
        printf("\nBefore iput():");
        display();
        iput(searchHash(ino));
        printf("\nAfter iput():");
        display();
    }
    return 0;
}
/*
        iput(inode)
            |
            v
        Is inode NULL?
            |
            No
            |
        Is inode busy?
           / \
         YES  NO
          |    |
          |   "Already unlocked"
          |
        Decrease ref
          |
          v
        Is ref > 0?
         /       \
        YES       NO
         |         |
        busy=0    Check link
                   |
               link == 0?
                /      \
              YES       NO
               |         |
           Free blocks   |
               |         |
               +---------+
                   |
            Check accessed/changed
                   |
              Update if needed
                   |
                busy = 0
                   |
              addFree(p)
                   |
            End of free list
*/