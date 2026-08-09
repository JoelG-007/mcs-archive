# Practice Assignment - Buffer Cache & Inode Algorithm Simulations

C simulations of eight classic UNIX kernel algorithms (buffer cache and
inode management), based on the descriptions in Maurice Bach's
*The Design of the UNIX Operating System*.

## Files

| File            | Algorithm | What it does |
|-----------------|-----------|---------------|
| `getblk.c`   | getblk()  | Buffer allocation: hash queue lookup, free list allocation, busy-buffer sleep/wakeup |
| `bread.c`    | bread()   | Calls getblk(), reports cache hit/miss, simulates a disk read on miss |
| `breada.c`   | breada()  | Read-ahead: fetches the requested block and asynchronously prefetches the next one |
| `bwrite.c`   | bwrite()  | Writes a modified buffer to disk, clears the delayed-write flag, releases the buffer |
| `brelse.c`   | brelse()  | Releases a busy buffer, wakes waiting processes, reinserts into the free list (head or tail, by validity) |
| `namei.c`    | namei()   | Resolves an absolute or relative pathname to an inode number by walking a simulated directory tree |
| `iget.c`     | iget()    | In-core inode lookup/allocation, with busy-inode sleep and reference counting |
| `iput.c`     | iput()    | Releases an in-core inode, writing it back to disk and freeing it once ref_count reaches 0 |

## Data structures

Every program is built on linked lists:

- **Hash queue** - an array of singly linked lists (separate chaining),
  used to locate a buffer/inode by number.
- **Free list** - a doubly linked list threading through the same
  buffer/inode nodes, holding everything not currently in use.
- **Wait list** - a singly linked list of process ids attached to a
  busy buffer/inode, simulating the sleep/wakeup queue (Q1, Q5, Q7, Q8).
- **Directory entries / file-system nodes** (Q6) - one linked list of
  `FSNode`s (one per inode) plus one linked list of `DirEntry`s per
  directory.


