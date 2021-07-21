/**
* Malloc Lab
* CS 241 - Fall 2018
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct _meta_data
{
  size_t size;
  struct _meta_data* next;
  struct _meta_data* prev;
} meta_data;

#define META_DATA_SIZE (sizeof(meta_data))
#define SIZE_T_SIZE (sizeof(size_t))

#define ALLOC_SIZE(req_size)  (size_t)(META_DATA_SIZE + SIZE_T_SIZE + req_size)
#define GET_FLAG_POS(req_size) (META_DATA_SIZE + req_size)
#define ALIGN_SIZE(old_size) ((old_size % 4 == 0) ? old_size : ((old_size / 4 + 1) * 4))
#define SET_BIT(old_size) (old_size | 1)

static meta_data* heap_begin = NULL;
static char* heap_end = NULL;
static int initialized = 0;

static meta_data used_front;
static meta_data used_tail;
static meta_data free_front;
static meta_data free_tail;

size_t clear_bit(size_t old_size)
{
  return (old_size & (~1));
}

size_t* get_data_flag(meta_data* md_ptr)
{
  char* flag = md_ptr -> size;
  if((char*)md_ptr + META_DATA_SIZE + ((md_ptr -> size & 1)))
    {
      flag = (md_ptr -> size & ~1);
    }
  return (size_t*)flag;
}

void insert_to_free(meta_data* md_ptr)
{
  md_ptr -> next = free_front.next;
  md_ptr -> prev = &free_front;
  free_front.next -> prev = md_ptr;
  free_front.next = md_ptr;
}

void insert_to_used(meta_data* md_ptr)
{
  md_ptr -> next = used_front.next;
  md_ptr -> prev = &used_front;
  used_front.next -> prev = md_ptr;
  used_front.next = md_ptr;
}

void delete_md(meta_data* md_ptr)
{
  md_ptr -> prev -> next = md_ptr -> next;
  md_ptr -> next -> prev = md_ptr -> prev;
}

void mark_free(meta_data* md_ptr)
{
  md_ptr -> size &= ~1;
  size_t* flag = get_data_flag(md_ptr);
  *flag &= ~1;
}

void mark_used(meta_data* md_ptr){
  md_ptr -> size |= 1;
  size_t* flag = get_data_flag(md_ptr);
  *flag |= 1;
}

void* initialize(size_t _size)
{
  size_t size = ALIGN_SIZE(_size);

  //set initialized
  initialized = 1;
  void* heap_ptr = heap_begin = sbrk(ALLOC_SIZE(size));
  heap_end = (char*)heap_ptr + ALLOC_SIZE(size);
  ((meta_data*)heap_ptr) -> size = size;

  char* flag = (char*)heap_ptr + GET_FLAG_POS(size);
  *((size_t*)flag) = size;
  meta_data* md_ptr = (meta_data*)heap_ptr;

  //make md_ptr used
  mark_used(md_ptr);
  md_ptr -> prev = (meta_data*)&used_front;

  //set used front
  used_front.next = md_ptr;
  used_front.prev = NULL;
  md_ptr -> next = (meta_data*)&used_tail;
  used_front.size = 1;

  //set used tail
  used_tail.prev = md_ptr;
  used_tail.next = NULL;
  used_tail.size = 1;

  //set free
  free_front.prev = NULL;
  free_front.next = (meta_data*)&free_tail;
  free_front.size = 1;
  free_tail.prev = (meta_data*)&free_front;
  free_tail.next = NULL;
  free_tail.size = 1;

  return (void*)(md_ptr + 1);
}

meta_data* first_fit(size_t size)
{
  meta_data* md_ptr = free_front.next;
  while(md_ptr)
    {
      if (~(md_ptr -> size & 1) && md_ptr -> size >= size)
	{
	  return md_ptr;
	}
      md_ptr = md_ptr -> next;
    }
  return NULL;
}

void split(size_t m_size, meta_data* chosen)
{
  size_t remain_size = chosen -> size - ALLOC_SIZE(m_size);

  chosen -> size = SET_BIT(m_size);

  size_t* flag = get_data_flag(chosen);
  *flag = SET_BIT(m_size);

  delete_md(chosen);
  insert_to_used(chosen);
  meta_data* s_md = (meta_data*)((char*)flag + SIZE_T_SIZE);

  s_md -> size = remain_size;
  size_t* s_md_flag = get_data_flag(s_md);
  *s_md_flag = remain_size;
  insert_to_free(s_md);
}

void *my_malloc(size_t size)
{
  if (initialized == 0)
    {
      return initialize(size);
    }

  size_t m_size = ALIGN_SIZE(size);
  meta_data* chosen = first_fit(m_size);

  if (chosen == NULL)
    {
      chosen = sbrk(ALLOC_SIZE(m_size));
      heap_end = (char*)chosen + ALLOC_SIZE(m_size);
      //set lowest bit
      chosen -> size = SET_BIT(m_size);
      size_t* flag = get_data_flag(chosen);
      *flag = chosen -> size;
      insert_to_used(chosen);
      return (void*)(chosen + 1);
    }
  long remain_size = chosen -> size - ALLOC_SIZE(m_size);
  if (remain_size >= 64)
    {
      //extra space
      split(m_size, chosen);
    }
  else
    {
      mark_used(chosen);
      delete_md(chosen);
      insert_to_used(chosen);
    }
  return (void*)(chosen + 1);  
}

meta_data* get_next(meta_data* md_ptr)
{
  if (md_ptr <= heap_begin)
    {
      return NULL;
    }
  size_t size = ((size_t*)md_ptr)[-1];
  if (size & 1)
    {
      return NULL;
    }
  return (meta_data*)((char*)md_ptr - (SIZE_T_SIZE + size + META_DATA_SIZE));
}

meta_data* get_pre(meta_data* md_ptr)
{
  //size_t size = md_ptr->size;
  char* pre = (char*)md_ptr +  ALLOC_SIZE(md_ptr -> size);
  if (pre - (char*)heap_end >= 0)
    {
      return NULL;
    }
  if (((meta_data*)pre) -> size & 1)
    {
      return NULL;
    }
  return (meta_data*)pre;

}

void left_merge(meta_data* md_ptr, meta_data* next)
{
  next -> size = ALLOC_SIZE(next -> size) + md_ptr -> size;
  *get_data_flag(md_ptr) = next -> size;
  delete_md(md_ptr);
}

void right_merge(meta_data* md_ptr, meta_data* pre)
{
  md_ptr -> size = md_ptr -> size + ALLOC_SIZE(pre -> size);
  *get_data_flag(pre) = md_ptr -> size;
  delete_md(pre);
}

void merge(meta_data* md_ptr)
{
  meta_data* next = get_next(md_ptr);
  meta_data* pre = get_pre(md_ptr);
  if (next && !pre)
    {
      left_merge(md_ptr, next);
    }
  else if (!next && pre)
    {
      right_merge(md_ptr, pre);
    }
  else if (pre && next)
    {
      next -> size =
	next -> size +
	ALLOC_SIZE(md_ptr -> size) + ALLOC_SIZE(pre -> size);
      *get_data_flag(pre) = next -> size;
      delete_md(pre);
      delete_md(md_ptr);
    }
}

void my_free(void* ptr)
{
  if (ptr == NULL)
    {
      return;
    }
  meta_data* md_ptr = ((meta_data*)ptr - 1);
  delete_md(md_ptr);
  mark_free(md_ptr);
  insert_to_free(md_ptr);
  merge(md_ptr);
}

void* my_calloc(size_t num, size_t size)
{
  size_t total_size = num*size;
  void* ptr = my_malloc(total_size);
  memset(ptr, 0, total_size);
  return ptr;
}

void* my_memcpy(char* src, char* dest,size_t len)
{
  size_t index = 0;
  while(index < len)
    {
      *((char*)dest + index) = *((char*)src + index);
      index++;
    }
  my_free(src);
  return dest;
}

void* my_realloc(void *ptr, size_t size)
{
  meta_data* md_ptr = ((meta_data*)ptr) - 1;
  size_t m_size = (size % 4 == 0) ? size : ((size / 4 + 1) * 4);
  size_t old_size = clear_bit(md_ptr -> size);
  size_t remain_size = old_size - m_size;

  if (old_size >= size)
    {
      if (remain_size > old_size || old_size / 3 == size )
	{
	  size_t* flag = get_data_flag(md_ptr);
	  
	  md_ptr -> size = SET_BIT(m_size);
	  char* temp_ptr = (char*)ptr + m_size;
	  *((size_t*)temp_ptr) = md_ptr -> size;
	  
	  temp_ptr += SIZE_T_SIZE;
	  
	  meta_data* md_ptr = (meta_data*)temp_ptr;
	  md_ptr -> size = old_size - ALLOC_SIZE(m_size);
	  *flag = md_ptr -> size;
	  
	  insert_to_free(md_ptr);
	  merge(md_ptr);
	}
      return ptr;
    }
  else
    {
      char* new_ptr = my_malloc(size);
      return my_memcpy(ptr, new_ptr, old_size);
    }
}

/**
 * Allocate space for array in memory
 *
 * Allocates a block of memory for an array of num elements, each of them size
 * bytes long, and initializes all its bits to zero. The effective result is
 * the allocation of an zero-initialized memory block of (num * size) bytes.
 *
 * @param num
 *    Number of elements to be allocated.
 * @param size
 *    Size of elements.
 *
 * @return
 *    A pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory, a
 *    NULL pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
 */
void *calloc(size_t num, size_t size) {
    // implement calloc!
  return my_calloc(num, size);
}

/**
 * Allocate memory block
 *
 * Allocates a block of size bytes of memory, returning a pointer to the
 * beginning of the block.  The content of the newly allocated block of
 * memory is not initialized, remaining with indeterminate values.
 *
 * @param size
 *    Size of the memory block, in bytes.
 *
 * @return
 *    On success, a pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a null pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
 */
void *malloc(size_t size) {
    // implement malloc!
  return my_malloc(size);
}

/**
 * Deallocate space in memory
 *
 * A block of memory previously allocated using a call to malloc(),
 * calloc() or realloc() is deallocated, making it available again for
 * further allocations.
 *
 * Notice that this function leaves the value of ptr unchanged, hence
 * it still points to the same (now invalid) location, and not to the
 * null pointer.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(),
 *    calloc() or realloc() to be deallocated.  If a null pointer is
 *    passed as argument, no action occurs.
 */
void free(void *ptr) {
    // implement free!
  my_free(ptr);
  return;
}

/**
 * Reallocate memory block
 *
 * The size of the memory block pointed to by the ptr parameter is changed
 * to the size bytes, expanding or reducing the amount of memory available
 * in the block.
 *
 * The function may move the memory block to a new location, in which case
 * the new location is returned. The content of the memory block is preserved
 * up to the lesser of the new and old sizes, even if the block is moved. If
 * the new size is larger, the value of the newly allocated portion is
 * indeterminate.
 *
 * In case that ptr is NULL, the function behaves exactly as malloc, assigning
 * a new block of size bytes and returning a pointer to the beginning of it.
 *
 * In case that the size is 0, the memory previously allocated in ptr is
 * deallocated as if a call to free was made, and a NULL pointer is returned.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(), calloc()
 *    or realloc() to be reallocated.
 *
 *    If this is NULL, a new block is allocated and a pointer to it is
 *    returned by the function.
 *
 * @param size
 *    New size for the memory block, in bytes.
 *
 *    If it is 0 and ptr points to an existing block of memory, the memory
 *    block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return
 *    A pointer to the reallocated memory block, which may be either the
 *    same as the ptr argument or a new location.
 *
 *    The type of this pointer is void*, which can be cast to the desired
 *    type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a NULL pointer is returned, and the memory block pointed to by
 *    argument ptr is left unchanged.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
 */
void *realloc(void *ptr, size_t size) {
    // implement realloc!
  return my_realloc(ptr, size);
}
