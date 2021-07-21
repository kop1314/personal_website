/**
* Vector Lab
* CS 241 - Fall 2018
*/

#include "sstring.h"

int main(int argc, char *argv[]) {
    // TODO create some tests
  sstring *str1 = cstr_to_sstring("abc");
  char* str2 = sstring_to_cstr(str1);
  printf("%s\n", str2);
  sstring_destroy(str1);
  free(str2);
  
  sstring* str3 = cstr_to_sstring("hello!!");
  sstring* str4 = cstr_to_sstring("cheo");
  int len1;
  len1 = sstring_append(str3, str4);
  char* ptr1 = sstring_to_cstr(str3);
  printf("%d %s\n", len1, ptr1);
  sstring_destroy(str3);
  sstring_destroy(str4);
  free(ptr1);
  
  sstring* str5 = cstr_to_sstring("abcde");
  sstring* str6 = cstr_to_sstring("def");
  int i = sstring_append(str5, str6);
  printf("%d\n", i);
  sstring_destroy(str5);
  sstring_destroy(str6);

  sstring* str7 = cstr_to_sstring("This is a sentence a b c d e.");
  vector* vec = sstring_split(str7, ' ');
  printf("call split\n");
  printf("size:%zu capacity %zu\n", vector_size(vec), vector_capacity(vec));
  for(size_t i = 0; i < vector_size(vec); i++) {
    printf("String: %s\n", vector_get(vec, i));
  }
  vector_destroy(vec);
  sstring_destroy(str7);

  //sstring_substitution test
  printf("Test Substitution\n");
  sstring *r = cstr_to_sstring("This is a {} day, {}!");
  int z = sstring_substitute(r, 19, "{}", "friend");
  z = sstring_substitute(r, 0, "{}", "good");
  char* str = sstring_to_cstr(r);
  printf("%d\n", z);
  printf("%s\n", str);
  sstring_destroy(r);
  free(str);
  
  sstring *r2 = cstr_to_sstring("INPUa");
  int z2 = sstring_substitute(r2, 4, "a", "output");
  char* str8 = sstring_to_cstr(r2);
  printf("%d\n", z2);
  printf("%s\n", str8);
  sstring_destroy(r2);
  free(str8);
  
  return 0;
  //      sstring *replace_me = cstr_to_sstring("This is a {} day, {}!");
}
