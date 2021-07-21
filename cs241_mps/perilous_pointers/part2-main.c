/**
* Pointers Gone Wild Lab
* CS 241 - Spring 2018
*/

#include "part2-functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * (Edit this function to print out the "Illinois" lines in
 * part2-functions.c in order.)
 */
int main() {
    // your code here
  first_step(81);

  int a = 132;
  int *value1 = &a;
  second_step(value1);

  int** value2 = (int**)malloc(sizeof(int*));
  int b = 8942;
  value2[0] = &b;
  double_step(value2);
  free(value2);
  
  char* value3 = malloc(30*sizeof(char));
  for(int i = 0; i < 30; i++)
    {
      value3[i] = 0;
    }
  value3[5] = 15;
  //int ptr = &c;
  //*(value3 + 5) = (char)c;
  strange_step(value3);
  free(value3);
  
  
  char *ptr2 = malloc(4);
  ptr2[3] = 0;
  void *value4 = ptr2;
  empty_step(value4);
  free(ptr2);

  char*ptr3 = "abcu";
  void *value5 = ptr3;
  two_step(value5, ptr3);

  char* ptr4 = "abcdef";
  three_step(ptr4, ptr4+2, ptr4 +4);

  char* value55 = malloc(4);
  value55[0] = 0;
  value55[1] = 1;
  value55[2] = 9;
  value55[3] = 17;
  step_step_step(value55, value55, value55);
  free(value55);
  
  int value6 = 5;
  char *ptr6 = malloc(1);
  ptr6[0] = value6;
  it_may_be_odd(ptr6, value6);
  free(ptr6);

  char* ptr7 = malloc(20);
  strcpy(ptr7, "NO MORE,CS241");
  tok_step(ptr7);
  free(ptr7);

  char* ptr8 = malloc(4);
  ptr8[0] = 1;
  ptr8[1] = 0;
  ptr8[2] = 0;
  ptr8[3] = 2;

  void* z = ptr8;
  the_end(z, z);
  free(ptr8);
 
  return 0;
}
