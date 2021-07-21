/**
* Mini Valgrind Lab
* CS 241 - Fall 2018
*/

#include "mini_valgrind.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
meta_data *head = NULL;
size_t total_memory_requested = 0;
size_t total_memory_freed = 0;
size_t invalid_addresses = 0;

void *mini_malloc(size_t request_size, const char *filename,
                  void *instruction) {
    // your code here
  if(request_size == 0)
    {
      return NULL;
     }
  void* me = malloc(request_size + sizeof(meta_data));
  if(me != NULL)
    {
      meta_data* md = (meta_data*)me;
      md->request_size = request_size;
      md->filename = filename;
      md->instruction = instruction;
      md->next = NULL;
      if(head == NULL)
	{
	  head = md;
	}
      else
	{
	  meta_data* temp= head;
	  head = md;
	  md->next = temp;
	  
	}
      //void* ptr = md + 1;
      total_memory_requested += request_size;
      //fprintf(stderr, "rs is %zu\n", request_size);
      //fprintf(stderr, "addr of md is %p\n", &md);
	//fprintf(stderr, "addr of ptr is %p\n", &ptr);
      return (me + sizeof(meta_data));
    }
    return NULL;
}

void *mini_calloc(size_t num_elements, size_t element_size,
                  const char *filename, void *instruction) {
    // your code here
  assert(num_elements > 0);
  assert(element_size > 0);
  size_t req_byte = num_elements * element_size;
  void* ptr = mini_malloc(req_byte, filename, instruction);
  if(ptr != NULL)
    {
      for(size_t i =0; i < req_byte; i++)
	{
	  *(char*)(ptr + i) = 0;
	}
      return ptr;
    }
    return NULL;
}

void *mini_realloc(void *payload, size_t request_size, const char *filename,
                   void *instruction) {
    // your code here
  if(request_size == 0)
    {
      mini_free(payload);
      return NULL;
    }

  meta_data* temp;
   meta_data* current = head;
   meta_data* p = NULL;
   size_t oldSize;
   int flag = 0;
   if(payload == NULL)
    {
      return  mini_malloc(request_size, filename, instruction);
      
    }
   else
     {
       temp = (meta_data*)(((void*)payload)-sizeof(meta_data));
       while(current != NULL){
	 if((meta_data*)(payload-sizeof(meta_data)) == current){
	   oldSize = current->request_size;
	   flag = 1;
	   break;
	 }
	 p = current;
	 current = current->next;
       }
       if(!flag){
	 invalid_addresses++;
	 return NULL;
       }
       if(p) p->next = current->next;
       else head = current->next;

       temp = (meta_data*)realloc(current,request_size + sizeof(meta_data));
       if(temp == NULL) return NULL;
       if(request_size > temp->request_size) total_memory_requested += (request_size - oldSize);
       if(request_size < temp->request_size) total_memory_freed += (oldSize - request_size);
       temp->request_size = request_size;
       temp->filename = filename;
       temp->instruction = instruction;

     }
   temp ->next = head;
   head = temp;
   void * ret =(void *)((char*)temp+sizeof(meta_data));
   return ret;
     
}

void mini_free(void *payload) {
    // your code here
  if(payload == NULL)
    {
      return;
    }
  if(head == NULL)
    {
      invalid_addresses++;
      return;
    }
  //void* md = ((void*)payload) + sizeof(meta_data);
  //fprintf(stderr, "md addr is %p\n", md);
  //fprintf(stderr, "payload addr is %p\n", payload);
  meta_data* current = head;
  if(((void*)current) + sizeof(meta_data) == payload)
    {
      head = head->next;
      total_memory_freed += current->request_size;
      free(current);
      return;
    }
  meta_data* temp_node = NULL;
  //fprintf(stderr, "after first ele\n");
  while(current->next != NULL)
    {
      //fprintf(stderr, "in the loop\n");
      if(payload == ((void*)current->next) + sizeof(meta_data))
	{
	  temp_node = current->next;
	  current->next = temp_node->next;
	  //fprintf(stderr, "tn size is %zu\n", temp_node->request_size);
	  total_memory_freed += temp_node->request_size;
	  free(temp_node);
	  //free(payload);
	  return;
	}
      current = current->next;
    }
  invalid_addresses++;
  return;
  
}
