#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#define NAME_LEN            32
#define ROOT_INODE           1
#define CURRENT_DIR_INODE    3   // simulated cwd: /home/user 
 
typedef struct DirEntry{
    char name[NAME_LEN];
    int  inode_num;
    struct DirEntry *next;
} DirEntry;
 
typedef struct FSNode{
    int  inode_num;
    int  is_dir;
    DirEntry *entries;     // only meaningful when is_dir == 1
    struct FSNode *next;
} FSNode;
 
FSNode *filesystem = NULL;

FSNode *find_fsnode_by_inode(int inode_num){
    FSNode *cur = filesystem;
    while(cur != NULL){
        if(cur->inode_num == inode_num) return cur;
        cur = cur->next;
    }
    return NULL;
}
 
DirEntry *create_dir_entry(const char *name, int inode_num){
    DirEntry *e =(DirEntry *) malloc(sizeof(DirEntry));
    strncpy(e->name, name, NAME_LEN - 1);
    e->name[NAME_LEN - 1] = '\0';
    e->inode_num = inode_num;
    e->next = NULL;
    return e;
}
 
void append_dir_entry(DirEntry **head, const char *name, int inode_num){
    DirEntry *e = create_dir_entry(name, inode_num);
    if(*head == NULL){ *head = e; return; }
    DirEntry *cur = *head;
    while(cur->next != NULL) cur = cur->next;
    cur->next = e;
}
 
FSNode *create_fsnode(int inode_num, int is_dir){
    FSNode *n =(FSNode *) malloc(sizeof(FSNode));
    n->inode_num = inode_num;
    n->is_dir = is_dir;
    n->entries = NULL;
    n->next = filesystem;
    filesystem = n;
    return n;
}
 
/* Building a small, fixed, in-memory demo file system:
 *
 *   /(1)
 *   |-- home(2)
 *   |    `-- user(3)          <-- CURRENT_DIR_INODE
 *   |         `-- file1.txt(4)
 *   |-- etc(5)
 *   |    `-- config.txt(6)
 *   `-- bin(7)
 *        `-- bash(8)
 */
void build_filesystem(void){
    FSNode *root, *home, *user, *etc, *bin;
 
    filesystem = NULL;
 
    root = create_fsnode(1, 1);
    home = create_fsnode(2, 1);
    user = create_fsnode(3, 1);
    create_fsnode(4, 0);
    etc  = create_fsnode(5, 1);
    create_fsnode(6, 0);
    bin  = create_fsnode(7, 1);
    create_fsnode(8, 0);
 
    append_dir_entry(&root->entries, ".",    1);
    append_dir_entry(&root->entries, "..",   1);
    append_dir_entry(&root->entries, "home", 2);
    append_dir_entry(&root->entries, "etc",  5);
    append_dir_entry(&root->entries, "bin",  7);
 
    append_dir_entry(&home->entries, ".",    2);
    append_dir_entry(&home->entries, "..",   1);
    append_dir_entry(&home->entries, "user", 3);
 
    append_dir_entry(&user->entries, ".",         3);
    append_dir_entry(&user->entries, "..",        2);
    append_dir_entry(&user->entries, "file1.txt", 4);
 
    append_dir_entry(&etc->entries, ".",          5);
    append_dir_entry(&etc->entries, "..",         1);
    append_dir_entry(&etc->entries, "config.txt", 6);
 
    append_dir_entry(&bin->entries, ".",    7);
    append_dir_entry(&bin->entries, "..",   1);
    append_dir_entry(&bin->entries, "bash", 8);
}
 
int namei(const char *pathname){
    char path_copy[256];
    char *component;
    int current_inode;
    FSNode *dir_node;
    DirEntry *entry;
    int found;
 
    strncpy(path_copy, pathname, sizeof(path_copy) - 1);
    path_copy[sizeof(path_copy) - 1] = '\0';
 
    if(path_copy[0] == '/'){
        current_inode = ROOT_INODE;
        printf("  Absolute path -> starting at root(inode %d).\n", ROOT_INODE);
    } else{
        current_inode = CURRENT_DIR_INODE;
        printf("  Relative path -> starting at current directory(inode %d).\n",
               CURRENT_DIR_INODE);
    }
 
    component = strtok(path_copy, "/");
 
    while(component != NULL){
        printf("  Component \"%s\": searching directory inode %d ... ",
               component, current_inode);
 
        dir_node = find_fsnode_by_inode(current_inode);
        if(dir_node == NULL || !dir_node->is_dir){
            printf("\n  ERROR: inode %d is not a directory.\n", current_inode);
            return -1;
        }
 
        found = 0;
        entry = dir_node->entries;
        while(entry != NULL){
            if(strcmp(entry->name, component) == 0){
                current_inode = entry->inode_num;
                found = 1;
                break;
            }
            entry = entry->next;
        }
 
        if(!found){
            printf("\n  ERROR: \"%s\": no such file or directory.\n", component);
            return -1;
        }
 
        printf("found -> inode %d\n", current_inode);
        component = strtok(NULL, "/");
    }
 
    return current_inode;
}
 
void display_filesystem(void){
    FSNode *n = filesystem;
    printf("\n--- Simulated File System(inode : type : entries) ---\n");
    while(n != NULL){
        printf("  inode %2d [%s] : ", n->inode_num, n->is_dir ? "DIR " : "FILE");
        if(n->is_dir){
            DirEntry *e = n->entries;
            while(e != NULL){
                printf("%s(%d) ", e->name, e->inode_num);
                e = e->next;
            }
        } else{
            printf("(regular file)");
        }
        printf("\n");
        n = n->next;
    }
    printf("--------------------------------------------------------\n");
}
 
int main(void){
    char pathname[256];
    int result, choice;
 
    build_filesystem();
 
    printf("namei() SIMULATOR\n");
    printf("Root inode = %d | current working directory inode = %d(/home/user)\n",
           ROOT_INODE, CURRENT_DIR_INODE);
 
    while(1){
        printf("\n===================== namei() =====================\n");
        printf(" 1. Resolve a pathname  (namei)\n");
        printf(" 2. Display file system tree\n");
        printf(" 3. Exit\n");
        printf("=====================================================\n");
        printf("Enter choice: ");
        if(scanf("%d", &choice) != 1) break;
        while(getchar() != '\n');    /* flush rest of the input line */
 
        if(choice == 1){
            printf("Enter pathname(e.g. /home/user/file1.txt or file1.txt): ");
            if(fgets(pathname, sizeof(pathname), stdin) == NULL) break;
            pathname[strcspn(pathname, "\n")] = '\0';
 
            printf("\n-- namei(\"%s\") --\n", pathname);
            result = namei(pathname);
            if(result != -1)
                printf("RESULT: \"%s\" resolves to inode %d\n", pathname, result);
            else
                printf("RESULT: path could not be resolved.\n");
        } else if(choice == 2){
            display_filesystem();
        } else if(choice == 3){
            printf("Exiting.\n");
            break;
        } else{
            printf("Invalid choice.\n");
        }
    }
    return 0;
}