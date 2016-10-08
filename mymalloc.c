#include <stdio.h>


static char mem[20000];


void * mymalloc(size_t size, char * file, int line) {

  static short firstmalloc = 1;

  // check if this is the first allocation of memory and set up initial metadata
  if (firstmalloc) {
    *((short*)mem) = 19996;  //size of block
    *((short*)mem + 1) = 0;  //availability: 1 = allocated, 0 = free
    firstmalloc = 0;
    
  }
  
  // go through the "linked list" of blocks until we find an available block big enough, if we hit the end,
  // return NULL and report the lack of space
  void * ptr = mem;
  while (ptr < (void*)(mem + 20000)) {
    if (!*((short*)(ptr + 2)) && *((short*)ptr) >= size + 4) {
      break;
    }
    ptr += *((short*)ptr) + 4;
  }

  if (ptr >= (void*)(mem + 20000)) {
    fprintf(stderr, "ERROR: Not Enough Memory\n\tFile: %s\n\tLine: %d\n", file, line);
    return NULL;
  }

  // if a suitable block is found, divide it into an allocated and free block, then return
  // a pointer to the allocated block
  int leftover = *((short*)ptr) - size;
  *((short*)ptr) = size;
  *((short*)(ptr + 2)) = 1;
  
  *((short*)(ptr + size + 4)) = leftover;
  *((short*)(ptr + size + 6)) = 0;

  return ptr + 4;


}


void myfree(void * index, char * file, int line) {

  //check if the given index is a valid pointer
  if (index < (void*)mem || (void*)(mem + 20000) <= index) {
    fprintf(stderr, "ERROR: Can only free a valid pointer\n\tFile: %s\n\t%d\n", file, line);
    return;
  }

  // check our list of blocks for the given index
  void * ptr = mem;
  if (ptr + 4 == index) {
    if (!*((short*)(ptr + *((short*)ptr) + 6))) {
      *((short*)ptr) += *((short*)(ptr + *((short*)ptr) + 4));
    }
    *((short*)(ptr+2)) = 0;
    return;
  }
  void * prev = ptr;
  ptr += *((short*)ptr) + 4;

  while(ptr < (void*)(mem + 20000)) {
    if (ptr + 4 == index && *((short*)(ptr + 2))) {
      break; 
    }
    prev = ptr;
    ptr += *((short*)ptr) + 4;
  }

  // report if the pointer is not an allocated block
  if (ptr >= (void*)(mem + 20000)) {
    fprintf(stderr, "ERROR: Pointer is not allocated \n\tFile: %s\n\tLine: %d\n", file, line);
    return;
  }

  // if we find the block, mark it free and combine with adjacent free blocks
  if (!*((short*)(ptr + *((short*)ptr) + 6))) {
    *((short*)ptr) += *((short*)(ptr + *((short*)ptr) + 4));
  }

  *((short*)(ptr + 2)) = 0;

  if (!*((short*)(prev + 2))) {
    *((short*)prev) += *((short*)ptr) + 4;
  }

}
