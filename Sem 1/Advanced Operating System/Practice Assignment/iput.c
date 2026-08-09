#include <stdio.h>
#include <stdlib.h>
 
#define NUM_INODES       8
#define INODE_HASH_SIZE  5
 
typedef struct WaitNode{
    int pid;
    struct WaitNode *next;
} WaitNode;
 
typedef struct Inode{
    int inode_num;
    int ref_count;
    int locked;
    int holder_pid;
    int modified;        // 1 = changed in memory, needs write-back
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
        inode_table[i].modified   = 0;
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

Inode *iget(int inode_num, int pid){
    Inode *inode = search_inode_cache(inode_num);
    if(inode != NULL){
        if(inode->locked && inode->holder_pid != pid){
            printf("  Inode %d is busy(held by P%d) -> P%d sleeps.\n",
                   inode_num, inode->holder_pid, pid);
            add_to_wait_list(inode, pid);
            return NULL;
        }
        if(!inode->locked){
            remove_from_free_list(inode);
        }
        inode->locked     = 1;
        inode->holder_pid = pid;
        inode->ref_count++;
        printf("  iget: inode %d locked for P%d, ref_count = %d.\n",
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
    inode->inode_num  = inode_num;
    inode->modified   = 0;
    insert_into_hash_queue(inode);
    inode->locked     = 1;
    inode->holder_pid = pid;
    inode->ref_count  = 1;
    printf("  iget: inode %d loaded from disk for P%d, ref_count = 1.\n",
           inode_num, pid);
    return inode;
}
 
void mark_inode_modified(int inode_num){
    Inode *inode = search_inode_cache(inode_num);
    if(inode == NULL || !inode->locked){
        printf("  Inode %d must be locked(iget) before it can be modified.\n",
               inode_num);
        return;
    }
    inode->modified = 1;
    printf("  Inode %d marked as modified(needs write-back).\n", inode_num);
}
 
void write_inode_to_disk(Inode *inode){
    printf("  >> Simulated disk I/O: writing inode %d to disk ...\n",
           inode->inode_num);
}
 
void iput(int inode_num){
    Inode *inode = search_inode_cache(inode_num);
    if(inode == NULL || inode->ref_count <= 0){
        printf("  iput: inode %d is not currently referenced.\n", inode_num);
        return;
    }
 
    inode->ref_count--;
    printf("-- iput(inode %d): ref_count now %d --\n", inode_num, inode->ref_count);
 
    if(inode->ref_count == 0){
        if(inode->modified){
            write_inode_to_disk(inode);
            inode->modified = 0;
        }
        inode->locked     = 0;                  // remove from busy state 
        inode->holder_pid = -1;
        wakeup_waiting_processes(inode);        // wake any waiters       
        insert_into_free_list(inode);           // release the inode      
        printf("  Inode %d released -> placed on free list.\n", inode_num);
    } else{
        printf("  Inode %d still referenced -> kept in memory(still locked by P%d).\n",
               inode_num, inode->holder_pid);
    }
}
 
void display_inode_table(void){
    int i;
    printf("\n--- In-core Inode Table ---\n");
    printf("  %-6s %-7s %-10s %-6s %-9s %-8s\n",
           "Slot", "Inode", "RefCount", "Lock", "Modified", "Holder");
    for(i = 0; i < NUM_INODES; i++){
        Inode *n = &inode_table[i];
        if(n->inode_num == -1){
            printf("  %-6d %-7s %-10s %-6s %-9s %-8s\n", i, "-", "-", "-", "-", "-");
        } else{
            char holder_buf[16];
            if(n->locked) snprintf(holder_buf, sizeof(holder_buf), "P%d", n->holder_pid);
            else snprintf(holder_buf, sizeof(holder_buf), "-");
            printf("  %-6d %-7d %-10d %-6s %-9s %-8s\n", i, n->inode_num,
                   n->ref_count, n->locked ? "busy" : "free",
                   n->modified ? "yes" : "no", holder_buf);
        }
    }
}
 
int main(void){
    int choice, inode_num, pid;
 
    init_inode_table();
    printf("In-core inode table ready: %d slots, %d hash buckets.\n",
           NUM_INODES, INODE_HASH_SIZE);
    printf("Tip: iget an inode twice from the SAME pid(ref_count -> 2),\n");
    printf("     then iput it once - it stays in memory; iput again to\n");
    printf("     see it fully released.\n");
 
    while(1){
        printf("\n===================== iput() =====================\n");
        printf(" 1. Acquire an inode  (iget - sets up demo state)\n");
        printf(" 2. Mark inode modified\n");
        printf(" 3. Release an inode  (iput)\n");
        printf(" 4. Display inode table\n");
        printf(" 5. Exit\n");
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
                printf("  Inode number: ");
                scanf("%d", &inode_num);
                mark_inode_modified(inode_num);
                break;
            case 3:
                printf("  Inode number: ");
                scanf("%d", &inode_num);
                iput(inode_num);
                break;
            case 4: display_inode_table(); break;
            case 5: printf("Exiting.\n"); return 0;
            default: printf("Invalid choice.\n");
        }
    }
    return 0;
}