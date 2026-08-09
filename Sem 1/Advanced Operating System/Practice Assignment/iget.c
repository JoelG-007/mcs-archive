#include <stdio.h>
#include <stdlib.h>
 
#define NUM_INODES       8
#define INODE_HASH_SIZE  5
 
typedef struct WaitNode{
    int pid;
    struct WaitNode *next;
} WaitNode;
 
typedef struct Inode{
    int inode_num;            // -1 = slot unused                        
    int ref_count;
    int locked;               // 1 = busy/locked, 0 = free               
    int holder_pid;           // pid currently holding the lock(-1=none)
    int valid;                // 1 = data has been read from disk       
    WaitNode *wait_list;
    struct Inode *hash_next;
    struct Inode *free_prev;
    struct Inode *free_next;
} Inode;
 
Inode  inode_table[NUM_INODES];
Inode *inode_hash_queue[INODE_HASH_SIZE];
Inode *inode_free_head = NULL;
Inode *inode_free_tail = NULL;

 
int hash_function(int inode_num){ return inode_num % INODE_HASH_SIZE; }
 
void init_inode_table(void){
    int i;
    for(i = 0; i < INODE_HASH_SIZE; i++) inode_hash_queue[i] = NULL;
    inode_free_head = inode_free_tail = NULL;
    for(i = 0; i < NUM_INODES; i++){
        inode_table[i].inode_num  = -1;
        inode_table[i].ref_count  = 0;
        inode_table[i].locked     = 0;
        inode_table[i].holder_pid = -1;
        inode_table[i].valid      = 0;
        inode_table[i].wait_list  = NULL;
        inode_table[i].hash_next  = NULL;
        inode_table[i].free_prev  = NULL;
        inode_table[i].free_next  = NULL;
        insert_into_free_list(&inode_table[i]);
    }
}
 
Inode *search_inode_cache(int inode_num){
    Inode *n = inode_hash_queue[hash_function(inode_num)];
    while(n != NULL){
        if(n->inode_num == inode_num) return n;
        n = n->hash_next;
    }
    return NULL;
}
 
void insert_into_hash_queue(Inode *inode){
    int idx = hash_function(inode->inode_num);
    inode->hash_next = inode_hash_queue[idx];
    inode_hash_queue[idx] = inode;
}
 
void remove_from_hash_queue(Inode *inode){
    if(inode->inode_num == -1) return;
    int idx = hash_function(inode->inode_num);
    Inode *cur = inode_hash_queue[idx], *prev = NULL;
    while(cur != NULL){
        if(cur == inode){
            if(prev) prev->hash_next = cur->hash_next;
            else inode_hash_queue[idx] = cur->hash_next;
            inode->hash_next = NULL;
            return;
        }
        prev = cur;
        cur = cur->hash_next;
    }
}
 
void insert_into_free_list(Inode *inode){
    inode->free_next = NULL;
    inode->free_prev = inode_free_tail;
    if(inode_free_tail != NULL) inode_free_tail->free_next = inode;
    else inode_free_head = inode;
    inode_free_tail = inode;
}
 
void remove_from_free_list(Inode *inode){
    if(inode->free_prev != NULL) inode->free_prev->free_next = inode->free_next;
    else inode_free_head = inode->free_next;
    if(inode->free_next != NULL) inode->free_next->free_prev = inode->free_prev;
    else inode_free_tail = inode->free_prev;
    inode->free_prev = inode->free_next = NULL;
}
 
void add_to_wait_list(Inode *inode, int pid){
    WaitNode *node =(WaitNode *) malloc(sizeof(WaitNode));
    node->pid = pid;
    node->next = inode->wait_list;
    inode->wait_list = node;
}
 
void wakeup_waiting_processes(Inode *inode){
    WaitNode *cur = inode->wait_list;
    if(cur == NULL){
        printf("   (no process was waiting on this inode)\n");
        return;
    }
    printf("    Waking up: ");
    while(cur != NULL){
        WaitNode *tmp = cur;
        printf("P%d ", cur->pid);
        cur = cur->next;
        free(tmp);
    }
    printf("\n");
    inode->wait_list = NULL;
}
 
void read_inode_from_disk(Inode *inode){
    printf("  >> Simulated disk I/O: reading inode %d ...\n", inode->inode_num);
    inode->valid = 1;
}
 
Inode *iget(int inode_num, int pid){
    Inode *inode = search_inode_cache(inode_num);
 
    if(inode != NULL){
        if(inode->locked && inode->holder_pid != pid){
            printf("  Inode %d is BUSY(held by P%d) -> process P%d sleeps.\n",
                   inode_num, inode->holder_pid, pid);
            add_to_wait_list(inode, pid);
            return NULL;
        }
        if(!inode->locked){
            remove_from_free_list(inode);   /* it was cached & free */
        }
        inode->locked     = 1;
        inode->holder_pid = pid;
        inode->ref_count++;
        printf("  Inode %d locked for P%d -> ref_count now %d.\n",
               inode_num, pid, inode->ref_count);
        return inode;
    }
 
    if(inode_free_head == NULL){
        printf("  No free in-core inodes available.\n");
        return NULL;
    }
 
    inode = inode_free_head;
    remove_from_free_list(inode);
    remove_from_hash_queue(inode);
    inode->inode_num = inode_num;
    read_inode_from_disk(inode);
    insert_into_hash_queue(inode);
    inode->locked     = 1;
    inode->holder_pid = pid;
    inode->ref_count  = 1;
 
    printf("  Inode %d not cached -> loaded from disk, ref_count = 1(P%d).\n",
           inode_num, pid);
    return inode;
}
 
void release_inode(int inode_num){
    Inode *inode = search_inode_cache(inode_num);
    if(inode == NULL || !inode->locked){
        printf("  Inode %d is not currently locked.\n", inode_num);
        return;
    }
    inode->ref_count--;
    if(inode->ref_count <= 0){
        inode->ref_count  = 0;
        inode->locked     = 0;
        inode->holder_pid = -1;
        wakeup_waiting_processes(inode);
        insert_into_free_list(inode);
        printf("  Inode %d released -> ref_count 0, placed on free list.\n",
               inode_num);
    } else{
        printf("  Inode %d ref_count now %d -> stays locked(held by P%d).\n",
               inode_num, inode->ref_count, inode->holder_pid);
    }
}
 
void display_inode_table(void){
    int i;
    printf("\n--- In-core Inode Table ---\n");
    printf("  %-6s %-7s %-10s %-6s %-6s %-8s\n",
           "Slot", "Inode", "RefCount", "Lock", "Valid", "Holder");
    for(i = 0; i < NUM_INODES; i++){
        Inode *n = &inode_table[i];
        if(n->inode_num == -1){
            printf("  %-6d %-7s %-10s %-6s %-6s %-8s\n", i, "-", "-", "-", "-", "-");
        } else{
            char holder_buf[16];
            if(n->locked) snprintf(holder_buf, sizeof(holder_buf), "P%d", n->holder_pid);
            else snprintf(holder_buf, sizeof(holder_buf), "-");
            printf("  %-6d %-7d %-10d %-6s %-6s %-8s\n", i, n->inode_num,
                   n->ref_count, n->locked ? "busy" : "free",
                   n->valid ? "yes" : "no", holder_buf);
        }
    }
}
 
int main(void){
    int choice, inode_num, pid;
 
    init_inode_table();
    printf("In-core inode table ready: %d slots, %d hash buckets.\n",
           NUM_INODES, INODE_HASH_SIZE);
    printf("Tip: iget the same inode from two different pids(no release\n");
    printf("     in between) to see the busy/sleep sequence; iget it twice\n");
    printf("     from the SAME pid to see ref_count go above 1.\n");
 
    while(1){
        printf("\n==================== iget() ======================\n");
        printf(" 1. Request an inode  (iget)\n");
        printf(" 2. Release an inode\n");
        printf(" 3. Display inode table\n");
        printf(" 4. Exit\n");
        printf("====================================================\n");
        printf("Enter choice: ");
        if(scanf("%d", &choice) != 1) break;
 
        switch(choice){
            case 1:
                printf("  Inode number: ");
                scanf("%d", &inode_num);
                printf("  Requesting pid: ");
                scanf("%d", &pid);
                iget(inode_num, pid);
                break;
            case 2:
                printf("  Inode number to release: ");
                scanf("%d", &inode_num);
                release_inode(inode_num);
                break;
            case 3: display_inode_table(); break;
            case 4: printf("Exiting.\n"); return 0;
            default: printf("Invalid choice.\n");
        }
    }
    return 0;
}