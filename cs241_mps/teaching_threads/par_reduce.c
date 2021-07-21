/**
* Teaching Threads Lab
* CS 241 - Fall 2018
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "reduce.h"
#include "reducers.h"

/* You might need a struct for each task ... */
struct data{
  pthread_t id;
  size_t len;
  int* list;
  reducer func;
  int base_case;
};
/* You should create a start routine for your threads. */
void* routine(void *arg)
{
  int* result = malloc(sizeof(int));
  //int ans = 0;
  //int *result = &ans;
  struct data *p = (struct data*)arg;
  *result = p->base_case;
  reducer func = p->func;
  for(size_t i = 0; i < p->len; i++)
    {
      *result = func(*result, p->list[i]);
    }
  return (void*)result;
}


int par_reduce(int *list, size_t list_len, reducer reduce_func, int base_case,
               size_t num_threads) {
    /* Your implementation goes here */
  //int ans = base_case;
  //int* sub_ans;
  void *ret;
  int* sub_ans;
  if(num_threads > 1)
    {
      size_t new_num_threads = num_threads - 1;
      if(num_threads - 1 > list_len)
	{
	  new_num_threads = list_len;
	}

      sub_ans = calloc(new_num_threads, sizeof(int));
      /*for(size_t i =0; i < list_len; i++)
	{
	  printf("%d, ", list[i]);
	}
	printf("\n");*/
      size_t sub_list_len;
      size_t last_sub_list_len;
      sub_list_len = list_len/(new_num_threads);
      last_sub_list_len = list_len%(new_num_threads) + sub_list_len;

      struct data *aData = calloc(new_num_threads, sizeof(struct data));
      //printf("insert sub lists in to each data strcut\n");
      //printf("sub_list_le is %zu last_sub_list_len is %zu\n", sub_list_len, last_sub_list_len);

      //insert sub lists into each data struct
      for(size_t i = 0; i < new_num_threads; i++)
	{
	  if(i < new_num_threads - 1)
	    {
	      size_t sub_list_index = 0;
	      aData[i].list = malloc(sizeof(int)*sub_list_len);
	      while(sub_list_index < sub_list_len)
		{
		  aData[i].list[sub_list_index] = list[i*sub_list_len+sub_list_index];
		  sub_list_index++;
		}
	      aData[i].len = sub_list_len;
	      aData[i].func = reduce_func;
	      aData[i].base_case = base_case;
	    }
	  else
	    {
	      size_t sub_list_index = 0;
	      aData[i].list= malloc(sizeof(int)*last_sub_list_len);
	      while(sub_list_index < last_sub_list_len)
		{
		  aData[i].list[sub_list_index] = list[i*sub_list_len+sub_list_index];
		  sub_list_index++;
		}
	      aData[i].len = last_sub_list_len;
	      aData[i].func = reduce_func;
	      aData[i].base_case = base_case;
	    }
	}

      //printf("start threads\n");
      for(size_t i =0; i < new_num_threads;i++)
	{
	  pthread_create(&aData[i].id, NULL, routine, &aData[i]);
	}
  
      for(size_t i = 0; i < new_num_threads; i++)
	{
	  pthread_join(aData[i].id, &ret);
	  sub_ans[i] = *(int*)ret;
	}
    
      struct data* lData = calloc(1, sizeof(struct data));
      lData[0].list = sub_ans;
      lData[0].len = new_num_threads;
      lData[0].func = reduce_func;
      lData[0].base_case = base_case;
      void* lret = routine(lData);

      free(lData);
      free(aData);
      return *(int*)lret;
    }
  struct data* lData = calloc(1, sizeof(struct data));
  lData[0].list = list;
  lData[0].len = list_len;
  lData[0].func = reduce_func;
  lData[0].base_case = base_case;
  void* lret = routine(lData);

  free(lData);
  return *(int*)lret;
}
