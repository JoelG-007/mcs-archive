#include <stdio.h>
#include <stdlib.h>
 
#define NUM_BUFFERS 8
#define HASH_SIZE   5
 
typedef struct WaitNode{
    int pid;
    struct WaitNode *next;
} WaitNode;
 
typedef struct Buffer{
    int block_num;
    int busy;
    int valid;
    WaitNode *wait_list;
    struct Buffer *hash_next;
    struct Buffer *free_prev;
    struct Buffer *free_next;
} Buffer;
 
Buffer  buffer_pool[NUM_BUFFERS];
Buffer *hash_queue[HASH_SIZE];
Buffer *free_list_head = NULL;
Buffer *free_list_tail = NULL;
 
int hash_function(int block_num){ return block_num % HASH_SIZE; }
 
void init_buffer_pool(void){
    int i;
    for(i = 0; i < HASH_SIZE; i++) hash_queue[i] = NULL;
    free_list_head = free_list_tail = NULL;
    for(i = 0; i < NUM_BUFFERS; i++){
        buffer_pool[i].block_num = -1;
        buffer_pool[i].busy      = 0;
        buffer_pool[i].valid     = 0;
        buffer_pool[i].wait_list = NULL;
        buffer_pool[i].hash_next = NULL;
        buffer_pool[i].free_prev = NULL;
        buffer_pool[i].free_next = NULL;
        insert_into_free_list_tail(&buffer_pool[i]);
    }
}
 
Buffer *search_hash_queue(int block_num){
    Buffer *b = hash_queue[hash_function(block_num)];
    while(b != NULL){
        if(b->block_num == block_num) return b;
        b = b->hash_next;
    }
    return NULL;
}
 
void insert_into_hash_queue(Buffer *buf){
    int idx = hash_function(buf->block_num);
    buf->hash_next = hash_queue[idx];
    hash_queue[idx] = buf;
}
 
void remove_from_hash_queue(Buffer *buf){
    if(buf->block_num == -1) return;
    int idx = hash_function(buf->block_num);
    Buffer *cur = hash_queue[idx], *prev = NULL;
    while(cur != NULL){
        if(cur == buf){
            if(prev) prev->hash_next = cur->hash_next;
            else hash_queue[idx] = cur->hash_next;
            buf->hash_next = NULL;
            return;
        }
        prev = cur;
        cur = cur->hash_next;
    }
}
 
void insert_into_free_list_tail(Buffer *buf){
    buf->free_next = NULL;
    buf->free_prev = free_list_tail;
    if(free_list_tail != NULL) free_list_tail->free_next = buf;
    else free_list_head = buf;
    free_list_tail = buf;
}
 
void insert_into_free_list_head(Buffer *buf){
    buf->free_prev = NULL;
    buf->free_next = free_list_head;
    if(free_list_head != NULL) free_list_head->free_prev = buf;
    else free_list_tail = buf;
    free_list_head = buf;
}
 
void remove_from_free_list(Buffer *buf){
    if(buf->free_prev != NULL) buf->free_prev->free_next = buf->free_next;
    else free_list_head = buf->free_next;
    if(buf->free_next != NULL) buf->free_next->free_prev = buf->free_prev;
    else free_list_tail = buf->free_prev;
    buf->free_prev = buf->free_next = NULL;
}
 
void add_to_wait_list(Buffer *buf, int pid){
    WaitNode *node =(WaitNode *) malloc(sizeof(WaitNode));
    node->pid = pid;
    node->next = buf->wait_list;
    buf->wait_list = node;
}
 
void wakeup_waiting_processes(Buffer *buf){
    WaitNode *cur = buf->wait_list;
    if(cur == NULL){
        printf("   (no process was waiting on this buffer)\n");
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
    buf->wait_list = NULL;
}
 
Buffer *getblk(int block_num, int pid){
    Buffer *buf = search_hash_queue(block_num);
    if(buf != NULL){
        if(buf->busy){
            printf("  Block %d is BUSY -> process P%d sleeps(queued).\n",
                   block_num, pid);
            add_to_wait_list(buf, pid);
            return NULL;
        }
        remove_from_free_list(buf);
        buf->busy = 1;
        printf("  Cache HIT for block %d -> locked for P%d.\n", block_num, pid);
        return buf;
    }
    if(free_list_head == NULL){
        printf("  No free buffers -> process P%d sleeps.\n", pid);
        return NULL;
    }
    buf = free_list_head;
    remove_from_free_list(buf);
    remove_from_hash_queue(buf);
    buf->block_num = block_num;
    buf->valid = 0;              // newly allocated -> not yet loaded
    buf->busy = 1;
    insert_into_hash_queue(buf);
    printf("  Cache MISS for block %d -> allocated for P%d.\n", block_num, pid);
    return buf;
}

void mark_buffer_validity(int block_num, int valid){
    Buffer *buf = search_hash_queue(block_num);
    if(buf == NULL || !buf->busy){
        printf("  Block %d is not currently held.\n", block_num);
        return;
    }
    buf->valid = valid;
    printf("  Block %d marked %s.\n", block_num, valid ? "VALID" : "INVALID");
}

void brelse(int block_num){
    Buffer *buf = search_hash_queue(block_num);
    if(buf == NULL || !buf->busy){
        printf("  brelse: block %d is not currently busy.\n", block_num);
        return;
    }
 
    printf("-- brelse(block %d) --\n", block_num);
 
    wakeup_waiting_processes(buf);     // wake any sleepers on THIS buffer
    buf->busy = 0;                     // update buffer status            
 
    if(buf->valid)
        insert_into_free_list_tail(buf);
    else
        insert_into_free_list_head(buf);
 
    printf("  Buffer for block %d freed -> placed at %s of free list.\n",
           block_num, buf->valid ? "tail" : "head");
}
 
void display_hash_queue(void){
    int i;
    printf("\n--- Hash Queue(bucket = block_num %% %d) ---\n", HASH_SIZE);
    for(i = 0; i < HASH_SIZE; i++){
        Buffer *b = hash_queue[i];
        printf("  [%d] ", i);
        if(b == NULL) printf("empty");
        while(b != NULL){
            printf("-> blk%d(%s,%s) ", b->block_num,
                   b->busy ? "busy" : "free",
                   b->valid ? "valid" : "invalid");
            b = b->hash_next;
        }
        printf("\n");
    }
}
 
void display_free_list(void){
    Buffer *b = free_list_head;
    printf("\n--- Free List(head -> tail) ---\n  ");
    if(b == NULL){ printf("EMPTY\n"); return; }
    while(b != NULL){
        if(b->block_num == -1) printf("empty ");
        else printf("blk%d ", b->block_num);
        if(b->free_next) printf("-> ");
        b = b->free_next;
    }
    printf("\n");
}
 
int main(void){
    int choice, block_num, pid, valid;
 
    init_buffer_pool();
    printf("Buffer cache ready: %d buffers, %d hash buckets.\n",
           NUM_BUFFERS, HASH_SIZE);
    printf("Tip: getblk the same block with two different pids(without\n");
    printf("releasing in between) to see the busy/sleep/wakeup sequence.\n");
 
    while(1){
        printf("\n===================== brelse() =====================\n");
        printf(" 1. Request a block         (getblk)\n");
        printf(" 2. Mark held buffer valid/invalid(1/0)\n");
        printf(" 3. Release a block         (brelse)\n");
        printf(" 4. Display hash queue\n");
        printf(" 5. Display free list\n");
        printf(" 6. Exit\n");
        printf("======================================================\n");
        printf("Enter choice: ");
        if(scanf("%d", &choice) != 1) break;
 
        switch(choice){
            case 1:
                printf("  Block number: ");
                scanf("%d", &block_num);
                printf("  Requesting pid: ");
                scanf("%d", &pid);
                getblk(block_num, pid);
                break;
            case 2:
                printf("  Block number: ");
                scanf("%d", &block_num);
                printf("  Valid?(1=yes, 0=no): ");
                scanf("%d", &valid);
                mark_buffer_validity(block_num, valid);
                break;
            case 3:
                printf("  Block number to release: ");
                scanf("%d", &block_num);
                brelse(block_num);
                break;
            case 4: display_hash_queue(); break;
            case 5: display_free_list(); break;
            case 6: printf("Exiting.\n"); return 0;
            default: printf("Invalid choice.\n");
        }
    }
    return 0;
}