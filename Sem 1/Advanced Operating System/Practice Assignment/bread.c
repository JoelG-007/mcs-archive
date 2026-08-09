#include <stdio.h>
#include <stdlib.h>
 
#define NUM_BUFFERS 8
#define HASH_SIZE   5
 
typedef struct Buffer{
    int block_num;
    int busy;
    int valid;
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
    buf->busy = 1;
    insert_into_hash_queue(buf);
    return buf;
}

void release_buffer(int block_num){
    Buffer *buf = search_hash_queue(block_num);
    if(buf == NULL || !buf->busy){
        printf("  Block %d is not currently busy.\n", block_num);
        return;
    }
    buf->busy = 0;
    insert_into_free_list(buf);
    printf("  Block %d released.\n", block_num);
}
 
void read_block_from_disk(Buffer *buf){
    printf("  >> Simulated disk I/O: reading block %d ...\n", buf->block_num);
    buf->valid = 1;
}
 
Buffer *bread(int block_num){
    Buffer *buf = getblk(block_num);
    if(buf == NULL) return NULL;
 
    if(buf->valid){
        printf("  [CACHE HIT]  block %d already valid - no disk I/O needed.\n",
               block_num);
    } else{
        printf("  [CACHE MISS] block %d not valid.\n", block_num);
        read_block_from_disk(buf);
    }
    return buf;
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
    int choice, block_num;
 
    init_buffer_pool();
    printf("Buffer cache ready: %d buffers, %d hash buckets.\n",
           NUM_BUFFERS, HASH_SIZE);
    printf("Tip: bread() a block(MISS), release it, then bread() it again\n");
    printf("     to see the HIT - bread() keeps the buffer locked, so it\n");
    printf("     must be released in between.\n");
 
    while(1){
        printf("\n================ bread() SIMULATOR ================\n");
        printf(" 1. Read a block  (bread)\n");
        printf(" 2. Release a block\n");
        printf(" 3. Display hash queue\n");
        printf(" 4. Display free list\n");
        printf(" 5. Exit\n");
        printf("=====================================================\n");
        printf("Enter choice: ");
        if(scanf("%d", &choice) != 1) break;
 
        switch(choice){
            case 1:
                printf("  Block number: ");
                scanf("%d", &block_num);
                bread(block_num);
                break;
            case 2:
                printf("  Block number to release: ");
                scanf("%d", &block_num);
                release_buffer(block_num);
                break;
            case 3: display_hash_queue(); break;
            case 4: display_free_list(); break;
            case 5: printf("Exiting.\n"); return 0;
            default: printf("Invalid choice.\n");
        }
    }
    return 0;
}