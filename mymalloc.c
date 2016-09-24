#include <stdio.h>
#include <stdlib.h>


static char myblock[5000];


// a linked list struct to keep track of allocated memory
struct tag {

  char * index; // pointer to the block
  size_t size;  // the size of the block
  struct tag * next;   // pointer to the next block

};

typedef struct tag tag;

// a functoin to create new tags
tag* newTag(char * index, size_t size, tag* next) {

  tag* new = malloc(sizeof(tag));
  new->index = index;
  new->size = size;
  new->next = next;

  return new;
}

static tag* tags;

char * mymalloc(size_t size, char * file, int line) {

  // check if any memory is allocated
  if (!tags) {
    tags = newTag(myblock, size, NULL); 
    return tags->index;

  }
  // check if there is empty space at the beginging of the block
  else if (tags->index - myblock >= size) {
    tags = newTag(myblock, size, tags);
    return tags->index;

  }
  
  // go through the list of tags until we hit the end or find a gap with enough space
  tag* ptr = tags;
  while (ptr->next && ptr->next->index - ptr->index - ptr->size < size) {
    ptr = ptr->next;
  }

  // if there is a large enough gap, allocate that space and insert a corresponding tag
  if (ptr->next) {
    ptr->next = newTag(ptr->index + ptr->size, size, ptr->next->next);
    return ptr->next->index;
  
  }

  // if we hit the end check to make sure there is enough memory
  if (ptr->index + ptr->size + size >= myblock + 5000) {
    fprintf(stderr, "ERROR: Not Enough Memory\n\tFile: %s\n\tLine: %d\n", file, line);
    return NULL;
  }

  // if there is, allocate the memory and append a tag to the list
  ptr->next = newTag(ptr->index + ptr->size, size, NULL);
  return ptr->next->index;

}


void myfree(char * index, char * file, int line) {

  //check if the given index is a valid pointer
  if (index < myblock || myblock + 5000 <= index) {
    fprintf(stderr, "ERROR: Can only free a valid pointer\n\tFile: %s\n\t%d\n", file, line);
    return;
  }

  // search the tag list for the given index and remove the tag if found
  tag* ptr = tags;
  if (ptr->index == index) {
    tags = ptr->next;
    return;
  }
  tag* prev = ptr;
  ptr = ptr->next;

  while(ptr->next) {
    if (ptr->index == index) {
      tag * temp = ptr;
      prev->next = ptr->next;
      free(temp);
      return;
    }
    
    prev = ptr;
    ptr = ptr->next;

  }
  
  // if we make it this far, the index was not in our list and so has either been freed already or was never allocated
  fprintf(stderr, "ERROR: Pointer was not allocated or has already been freed\n\tFile: %s\n\tLine: %d\n", file, line);

}
