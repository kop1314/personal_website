/**
* Mini Valgrind Lab
* CS 241 - Fall 2018
*/

#include <stdio.h>
#include <stdlib.h>

int main() {
    // Your tests here using malloc and free
  void *p1 = calloc(30, 1);
  void *p2 = calloc(40, 1);
  void *p3 = calloc(50, 1);
  void *p4 = realloc(p3, 0);
  void *p5 = realloc(p4, 50);
  void *p6 = realloc(NULL, 20);
  void *p7 = "abc";
  //void *p8 = realloc(p7, 60);
  
  (void)p1;
  (void)p2;
  (void)p3;
  (void)p5;
  (void)p6;
  //(void)p8;
  free(p3);
  return 0;
}
