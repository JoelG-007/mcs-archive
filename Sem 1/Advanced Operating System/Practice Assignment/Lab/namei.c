#include <stdio.h>
#include <string.h>
#define MAX_INODES 5
#define MAX_ENTRIES 5
#define MAX_PATH 100

typedef struct Entry{
    char name[20];
    int inode;
}Entry;

typedef struct Inode{
    int ino;int isDir;
    int ref;
    int busy;
    Entry entries[MAX_ENTRIES];
    int count;
}Inode;

Inode inodes[MAX_INODES];
int inodeCount=0;
int currentInode=3;

Inode *iget(int ino){
    int i;
    for(i=0;i<inodeCount;i++){
        if(inodes[i].ino==ino){
            if(inodes[i].busy){
                printf("Inode %d is busy. Process sleeps...\n",ino);
                printf("Inode becomes free. Process wakes up.\n");
                inodes[i].busy=0;
            }
            inodes[i].busy=1;
            inodes[i].ref++;
            return &inodes[i];
        }
    }
    return NULL;
}

void iput(Inode *p){
    if(p!=NULL){
        if(p->ref>0)
        p->ref--;
        p->busy=0;
    }
}

Inode *findEntry(Inode *dir,char *name){
    int i;
    for(i=0;i<dir->count;i++){
        if(strcmp(dir->entries[i].name,name)==0)
        return &inodes[dir->entries[i].inode-1];
    }
    return NULL;
}

Inode *namei(char *path){
    char copy[MAX_PATH];
    char *token;
    Inode *working,*next;
    int absolute;
    if(strlen(path)==0){
        printf("Empty path.\n");
        return NULL;
    }
    absolute=(path[0]=='/');
    strcpy(copy,path);
    if(absolute)
    working=iget(1);
    else
    working=iget(currentInode);
    if(working==NULL){
        printf("Starting inode not found.\n");
        return NULL;
    }
    token=strtok(copy,"/");
    if(token==NULL)
    return working;
    while(token!=NULL){
        printf("Searching component: %s\n",token);
        if(strcmp(token,".")==0){
            token=strtok(NULL,"/");
            continue;
        }
        if(!working->isDir){
            printf("Inode %d is not a directory.\n",working->ino);
            iput(working);
            return NULL;
        }
        if(working->ino==1 && strcmp(token,"..")==0){
            token=strtok(NULL,"/");
            continue;
        }
        next=findEntry(working,token);
        if(next==NULL){
            printf("Component '%s' not found.\n",token);
            iput(working);
            return NULL;
        }
        printf("Component '%s' found. Inode = %d\n",token,next->ino);
        iput(working);
        working=iget(next->ino);
        if(working==NULL){
            printf("Inode not found.\n");
            return NULL;
        }
        token=strtok(NULL,"/");
    }
    return working;
}

void addEntry(int dir,int position,char *name,int ino){
    strcpy(inodes[dir-1].entries[position].name,name);
    inodes[dir-1].entries[position].inode=ino;
    inodes[dir-1].count=position+1;
}

void display(){
    int i;
    printf("\nInode Table:\n");
    for(i=0;i<inodeCount;i++)
    printf("I%d(D%d,R%d,B%d)\n",inodes[i].ino,inodes[i].isDir,inodes[i].ref,inodes[i].busy);
}

int main(){
    char path[MAX_PATH];Inode *p;
    int i;
    for(i=0;i<MAX_INODES;i++)
    inodes[i].ino=i+1;
    inodes[0].isDir=1;
    inodes[1].isDir=1;
    inodes[2].isDir=1;
    inodes[3].isDir=0;
    inodes[4].isDir=0;
    inodeCount=5;
    addEntry(1,0,"home",2);
    addEntry(1,1,"etc",5);
    addEntry(2,0,"user",3);
    addEntry(2,1,"docs",4);
    addEntry(2,2,"..",1);
    addEntry(3,0,"file.txt",4);
    addEntry(3,1,"..",2);
    while(1){
        printf("\nEnter path (0 to exit): ");
        fgets(path,sizeof(path),stdin);
        path[strcspn(path,"\n")]=0;
        if(strcmp(path,"0")==0)
        break;
        printf("\nBefore namei():");
        display();
        p=namei(path);
        if(p!=NULL){
            printf("Requested file/directory inode: %d\n",p->ino);
            iput(p);
        }
        printf("\nAfter namei():");
        display();
    }return 0;
}

/*
Sample - /home/user/file.txt

                /home/user/file.txt
                         |
                         v
                    Absolute path
                         |
                         v
                        I1 (/)
                         |
                     search home
                         |
                         v
                         I2
                      /home
                         |
                     search user
                         |
                         v
                         I3
                   /home/user
                         |
                  search file.txt
                         |
                         v
                         I4
                /home/user/file.txt
                         |
                         v
                     return I4
*/