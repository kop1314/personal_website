/**
* Vector Lab
* CS 241 - Fall 2018
*/

#include "sstring.h"
#include "vector.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <string.h>

struct sstring {
  char* str;
};

sstring *cstr_to_sstring(char *input) {
    // your code goes here
  assert(input);
  sstring* sstr = malloc(sizeof(sstring));
  sstr -> str = malloc(strlen(input) + 1);
  strcpy(sstr -> str, input);
  sstr -> str[strlen(input)] = '\0';
    return sstr;
}

char *sstring_to_cstr(sstring *input) {
    // your code goes here
  assert(input);
  char* ptr = malloc(strlen(input->str) + 1);
  strcpy(ptr, input -> str);
  
  return ptr;
}

int sstring_append(sstring *this, sstring *addition) {
    // your code goes here
  //char* ns = malloc(strlen(this->str) + strlen(addition->str) + 1);
  //ns[0] = '\0';
  //strcat(ns, this->str);
  //strcat(ns, addition->str);
  assert(this);
  assert(addition);
  size_t thisLen = strlen(this->str);
  this -> str = realloc(this -> str, thisLen + strlen(addition->str) + 1);
  strcpy(this -> str + thisLen, addition -> str);
  this -> str[thisLen + strlen(addition->str)] = '\0';
  return strlen(this->str);
}

vector *sstring_split(sstring *this, char delimiter) {
    // your code goes here
  //printf("call split?????\n");
  assert(this);
  vector* v = string_vector_create();
  if(strchr(this->str, delimiter) == NULL)
    {
      //printf("return null\n");
      vector_push_back(v, this->str);
      return v;
    }
  if(strlen(this->str) == 0) {
    //printf("return empty string");
    vector_push_back(v, this-> str);
    return v;
  }
  //size_t index = 0;
  //printf("start split\n");
  char *token = strtok(this -> str, &delimiter);
  //printf("%s\n", token);
  while(token){
    //if(vector_size(v) == vector_capacity(v)){
    //vector_resize(v, vector_capacity(v) + 1);
      
    //}
    //printf("token is %s\n", strdup(token));
    vector_push_back(v, token);
    //index++;
    token = strtok(NULL, &delimiter);
  }
    return v;
}

int sstring_substitute(sstring *this, size_t offset, char *target,
                       char *substitution) {
    // your code goes here
  assert(this);
  assert(target);
  assert(substitution);
  int r = -1;
  int done = 0;
  size_t index = 0;
  while(index <= (strlen(this->str) - strlen(target)))
    {
      if(index >= offset)
	{
	  if(memcmp(this->str + index, target, strlen(target)) == 0)
	    {
	       if(done ==0){
	       printf("I compare strs when index is %zu\n", index);
	       size_t len =strlen(this ->str ) + strlen(substitution) - strlen(target);
	       char* tmp = malloc(strlen(this -> str) - index);
	       tmp[strlen(this ->str) - index - 1] = '\0';
	       this -> str = realloc(this -> str, strlen(this ->str) + strlen(substitution) - strlen(target)+ 1);
	      
	       strcpy(tmp, this -> str + index + strlen(target));
	       strcpy(this-> str + index, substitution);
	       strcpy(this -> str + index + strlen(substitution), tmp);
	       this -> str[len] = '\0';
	      
	       done = 1;
	       free(tmp);
	      }
	      r = 1;
	    }
	}
      index ++;
    }
    return r;
}

char *sstring_slice(sstring *this, int start, int end) {
    // your code goes here
  assert(this);
  if(this->str == NULL){
    return NULL;
  }
  if(strlen(this->str) == 0)
    {
      return this->str;
    }
  char* str = calloc(end - start + 1, sizeof(char));
  memcpy(str, this->str + start, end - start);
  str[end - start] = '\0';
    return str;
}

void sstring_destroy(sstring *this) {
    // your code goes here
  assert(this);
  free(this->str);
  free(this);
}
