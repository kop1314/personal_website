/**
* advanced_c Lab
* CS 241 - Spring 2018
*/

#include "dictionary.h"
#include <Block.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void _dictionary_destructor(void **e) {
    // Your code here
  dictionary_t d = (dictionary_t)(*e);
  d("destroy");
  Block_release(d);
}

dictionary_t create_dictionary(void) {
    // Your code here
  __block dict_value** dData = NULL;
  __block size_t size = 0;

  return /*(^dictionary_t)*/ Block_copy(^(char* str, ...) {
      //printf("in bolck copy\n");
      if(strcmp(str, "insert") == 0)
	{
	  va_list args;
	  va_start(args, str);
	  //printf("insert str is %s\n", str);
	  //va_arg(args, dict_value*);
	  while(str != NULL)
	    {
	      //printf("str is %s\n", str);
	      if(size == 0)
		{
		  dData = (dict_value**)calloc(1, sizeof(dict_value*));
		  dict_value* temp = va_arg(args, dict_value*);
		  dict_value* n = malloc(sizeof(dict_value*));
		  n -> key = strdup(temp -> key);
		  n -> value_len = temp -> value_len;
		  n -> value = malloc(temp -> value_len);
		  memcpy(n -> value, temp -> value, temp -> value_len);
		  dData[size] = n;
		  size++;
		}
	      else
		{
		  //printf("realloc new one\n");
		  dData = (dict_value**)realloc(dData, (size+1) * sizeof(dict_value*));
		  dict_value* temp = va_arg(args, dict_value*);
		  dict_value* n = malloc(sizeof(dict_value*));
		  n -> key = strdup(temp -> key);
		  n -> value_len = temp -> value_len;
		  n -> value = malloc(temp -> value_len);
		  memcpy(n -> value, temp -> value, temp -> value_len);
		  dData[size] = n;
		  size++;
		}
	      str = va_arg(args, char*);
	      //++str;
	    }
	  //printf("return baby\n");
	  va_end(args);
	  return dict_NULL;
	}
      if(strcmp(str, "lookup") == 0)
	{
	  //return 
	  if(size ==0)
	    {
	      // printf("it is null\n");
	      return dict_NULL;
	    }
	  va_list args;
	  va_start(args, str);
	  //str++;
	  char* a = va_arg(args, char*);
	  //printf("key is %s\n", a);
	  for(size_t i = 0; i < size; i++)
	    {
	      if(strcmp(dData[i]->key, a) == 0)
		{
		  return *dData[i];
		}
	      
	    }
	  //printf("pass loop\n");
	  va_end(args);
	  return dict_NULL;
	}
      if(strcmp(str, "destroy") == 0)
	{
	  //^(void) {
	   if(size > 0)
	     {
	       for(size_t i = 0; i < size; i++)
		 {
		   free(dData[i] -> key);
		   free(dData[i] -> value);
		   free(dData[i]);
		 }
	       free(dData);
	       size = 0;
	     }
	   //return;
	   // }
      // return;
	}
      return dict_NULL;
   });  
}
