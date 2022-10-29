// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

struct {
  struct spinlock lock;
  struct spinlock bucket_lock[13];
  struct buf buf[NBUF];

  // Linked list of all buffers, through prev/next.
  // Sorted by how recently the buffer was used.
  // head.next is most recent, head.prev is least.
  struct buf* buf_list[13];
} bcache;

void
binit(void)
{
  struct buf *b;

  for (int i = 0; i < 13; ++i) {
      initlock(&bcache.bucket_lock[i], "bcache.bucket");
  }
  initlock(&bcache.lock, "bcache");

  for(b = bcache.buf; b < bcache.buf+NBUF; b++){
    initsleeplock(&b->lock, "buffer");
    b->time_stamp = 0;
    struct buf* tail = bcache.buf_list[b->blockno % 13];
    while (tail && tail->next) {
        tail = tail->next;
    }
    if (tail) {
        tail->next = b;
    }
    else {
        bcache.buf_list[b->blockno % 13] = b;
    }
  }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b;

  uint num = blockno % 13;
  acquire(&bcache.bucket_lock[num]);

  // Is the block already cached?
  for(b = bcache.buf_list[num]; b && b->next; b = b->next){
     if(b->dev == dev && b->blockno == blockno){
         b->refcnt++;
         b->time_stamp = ticks;
         release(&bcache.bucket_lock[num]);
         acquiresleep(&b->lock);
         return b;
     }
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer.
  //acquire(&bcache.lock);
  struct buf *lru = 0;
  for(b = bcache.buf_list[num]; b && b->next; b = b->next){
      if(b->refcnt == 0) {
          if (!lru) {
              lru = b;
              continue;
          }
          if (b->time_stamp < lru->time_stamp) {
              lru = b;
          }
      }
  }

  if (!lru) {
      struct buf* tail = b;
      for (uint num1 = 0; num1 < 13; num1++) {
          if (num == num1) {
              continue;
          }
          acquire(&bcache.bucket_lock[num1]);
          struct buf* last = 0;
          for(b = bcache.buf_list[num1]; b && b->next; b = b->next){
              if(b->refcnt == 0) {
                  lru = b;
                  if (last) {
                      last->next = b->next;
                  }
                  else {
                      bcache.buf_list[num1] = b->next;
                  }
                  lru->next = 0;
                  if (tail) {
                      tail->next = lru;
                  }
                  else {
                      bcache.buf_list[num] = lru;
                  }
                  break;
              }
              last = b;
          }
          release(&bcache.bucket_lock[num1]);
          if (lru) {
              break;
          }
      }
  }

  if (!lru) {
      panic("bget: no buffers");
  }

  lru->dev = dev;
  lru->blockno = blockno;
  lru->valid = 0;
  lru->refcnt = 1;
  lru->time_stamp = ticks;
  //release(&bcache.lock);
  release(&bcache.bucket_lock[num]);
  acquiresleep(&lru->lock);

  return lru;
}

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
void
brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  acquire(&bcache.bucket_lock[b->blockno % 13]);
  b->refcnt--;

  if (b->refcnt == 0) {
    // no one is waiting for it.
    //b->time_stamp = ticks;

  }

  release(&bcache.bucket_lock[b->blockno % 13]);
}

void
bpin(struct buf *b) {
  acquire(&bcache.bucket_lock[b->blockno % 13]);
  b->refcnt++;
  b->time_stamp = ticks;
  release(&bcache.bucket_lock[b->blockno % 13]);
}

void
bunpin(struct buf *b) {
  acquire(&bcache.bucket_lock[b->blockno % 13]);
  b->refcnt--;
  release(&bcache.bucket_lock[b->blockno % 13]);
}


