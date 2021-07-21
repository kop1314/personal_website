/**
* Vector Lab
* CS 241 - Fall 2018
*/

#include "vector.h"
#include <stdio.h>
#include <assert.h>
int main(/*int argc, char *argv[]*/) {
    // Write your test cases here
  //vector *fv = vector_create(NULL, NULL, NULL);
  //testVectorResize();
   printf("loading vector\n");
  vector *v = unsigned_int_vector_create();
  if( vector_empty(v)) printf("it is empty\n");
  //printf("set 10");
  //vector_set(v, 0,) 
  printf("resize vector\n");
  vector_resize(v, 10);
  printf("size is %zu capacity is %zu\n", vector_size(v), vector_capacity(v));
  //assert(vector_size(v) == 2);
  if(vector_empty(v))printf("it is empty\n");
  printf("destroy\n");
  vector_destroy(v);

  //test insert
  printf("test insert\n");
  vector *vIn = int_vector_create();
  for (int i = 0; i < 10; i++) {
    vector_insert(vIn, vector_size(vIn), &i);
    assert((int)vector_size(vIn) == i+1);
  }

  for(int i = 0; i < 10; i++) {
    assert(i == *(int*) vector_get(vIn, i));
  }
  int ii = 10;
  vector_insert(vIn, 3, &ii);
  assert(ii == *(int *) vector_get(vIn, 3));
  vector_destroy(vIn);
  
  //test pop push
  printf("test pop push\n");
  vector *pp = int_vector_create();

  int i = 5;
  vector_push_back(pp, &i);
  vector_pop_back(pp);
  assert(vector_size(pp)==0);

  vector_push_back(pp, &i);
  vector_push_back(pp, &i);
  vector_pop_back(pp);
  vector_push_back(pp, &i);
  assert(vector_size(pp) ==2);
  assert(*(int *)vector_get(pp, 0) == i);
  assert(*(int *)vector_get(pp, 0) == i);
  vector_destroy(pp);
  
  //print out vector
  printf("print out vector\n");
  vector* pov = string_vector_create();
  char *one = "hi";
  vector_push_back(pov, one);
  one = "wow";
  vector_push_back(pov, one);
  one = "hi hi";
  vector_push_back(pov, one);
  for(size_t i = 0; i < vector_size(pov); i++) {
    printf("string: %s\n", vector_get(pov, i));
  }
  vector_erase(pov, 2);
  vector_erase(pov, 1);
  vector_set(pov, 0, "Hello");
  vector_push_back(pov, "World");
  for(size_t i = 0; i < vector_size(pov); i++) {
    printf("%s ", vector_get(pov, i));
  }

  printf("\n");
  printf("vector capacity: %d size: %d\n", (int) vector_capacity(pov), (int)vector_size(pov));
  vector_destroy(pov);

  printf("Test resize 11 then 20\n");
  vector* re = int_vector_create();
  vector_resize(re, 11);
  printf("Vector size is %zu capacity is %zu\n", vector_size(re), vector_capacity(re));

  for(size_t i = 0; i < vector_size(re); i++) {
    printf("%d ", *(int*)vector_get(re, i));
  }

  vector_resize(re, 20);
  printf("Vector size is %zu capacity is %zu\n", vector_size(re), vector_capacity(re));

  for(size_t i = 0; i < vector_size(re); i++) {
    printf("%d ", *(int*)vector_get(re, i));
  }
  
  vector_destroy(re);

  //printf("Test Erase\n");
  
  return 0;
}
/*void testVectorResize(){
  printf("loading vector\n");
  vector *v = unsigned_int_vector_create();
  printf("resize vector\n");
  vector_resize(v, 2);
  assert(vector_size(v) == 2);
  vector_destroy(v);
  }*/
