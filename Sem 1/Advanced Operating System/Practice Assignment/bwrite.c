include <stdio.h>
#include <stdlib.h>
 
#define NUM_BUFFERS 8
#define HASH_SIZE   5
 
typedef struct Buffer{
    int block_num;
    int busy;
    int valid;
    int delayed_write;      // 1 = holds data not yet written to disk
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
        buffer_pool[i].busy = 0;
        buffer_pool[i].valid = 0;
        buffer_pool[i].delayed_write = 0;
        buffer_pool[i].hash_next = NULL;
        buffer_pool[i].free_prev = NULL;
        buffer_pool[i].free_next = NULL;
        insert_into_free_list(&buffer_pool[i]);
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
 
void insert_into_free_list(Buffer *buf){
    buf->free_next = NULL;
    buf->free_prev = free_list_tail;
    if(free_list_tail != NULL) free_list_tail->free_next = buf;
    else free_list_head = buf;
    free_list_tail = buf;
}
 
void remove_from_free_list(Buffer *buf){
    if(buf->free_prev != NULL) buf->free_prev->free_next = buf->free_next;
    else free_list_head = buf->free_next;
    if(buf->free_next != NULL) buf->free_next->free_prev = buf->free_prev;
    else free_list_tail = buf->free_prev;
    buf->free_prev = buf->free_next = NULL;
}
 
Buffer *getblk(int block_num){
    Buffer *buf = search_hash_queue(block_num);
    if(buf != NULL){
        if(buf->busy){
            printf("  Block %d is currently busy.\n", block_num);
            return NULL;
        }
        remove_from_free_list(buf);
        buf->busy = 1;
        printf("  getblk: buffer for block %d locked.\n", block_num);
        return buf;
    }
    if(free_list_head == NULL){
        printf("  No free buffers available.\n");
        return NULL;
    }
    buf = free_list_head;
    remove_from_free_list(buf);
    remove_from_hash_queue(buf);
    buf->block_num = block_num;
    buf->valid = 0;
    buf->delayed_write = 0;
    buf->busy = 1;
    insert_into_hash_queue(buf);
    printf("  getblk: free buffer allocated & locked for block %d.\n", block_num);
    return buf;
}
 
// Simulates a process writing new data into a buffer it holds
void modify_buffer(int block_num){
    Buffer *buf = search_hash_queue(block_num);
    if(buf == NULL || !buf->busy){
        printf("  Block %d must be locked(getblk) before it can be modified.\n",
               block_num);
        return;
    }
    buf->valid = 1;
    buf->delayed_write = 1;
    printf("  Buffer for block %d modified -> marked for delayed write.\n",
           block_num);
}
 
void write_block_to_disk(Buffer *buf){
    printf("  >> Simulated disk I/O: writing block %d to disk ...\n",
           buf->block_num);
}
 
void bwrite(int block_num){
    Buffer *buf = search_hash_queue(block_num);
    if(buf == NULL || !buf->busy){
        printf("  bwrite: block %d is not currently held(call getblk first).\n",
               block_num);
        return;
    }
 
    write_block_to_disk(buf);        // write modified buffer to disk
    buf->delayed_write = 0;          // reset delayed-write flag    
    buf->busy = 0;                   // release the buffer         
    insert_into_free_list(buf);      // update free list       
 
    printf("  bwrite: block %d written, flag cleared, buffer freed.\n",
           block_num);
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
                   b->delayed_write ? "dirty" : "clean");
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
    int choice, block_num;
 
    init_buffer_pool();
    printf("Buffer cache ready: %d buffers, %d hash buckets.\n",
           NUM_BUFFERS, HASH_SIZE);
    printf("Tip: getblk a block, modify it, then bwrite it.\n");
 
    while(1){
        printf("\n===================== bwrite() =====================\n");
        printf(" 1. Lock a buffer              (getblk)\n");
        printf(" 2. Modify locked buffer        (simulate new data)\n");
        printf(" 3. Write buffer to disk        (bwrite)\n");
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
                getblk(block_num);
                break;
            case 2:
                printf("  Block number: ");
                scanf("%d", &block_num);
                modify_buffer(block_num);
                break;
            case 3:
                printf("  Block number: ");
                scanf("%d", &block_num);
                bwrite(block_num);
                break;
            case 4: display_hash_queue(); break;
            case 5: display_free_list(); break;
            case 6: printf("Exiting.\n"); return 0;
            default: printf("Invalid choice.\n");
        }
    }
    return 0;
}