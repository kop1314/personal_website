/**
* Extreme Edge Cases Lab
* CS 241 - Fall 2018
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "camelCaser.h"
#include "camelCaser_tests.h"

/*
 * Testing function for various implementations of camelCaser.
 *
 * @param  camelCaser   A pointer to the target camelCaser function.
 * @param  destroy      A pointer to the function that destroys camelCaser
 * output.
 * @return              Correctness of the program (0 for wrong, 1 for correct).
 */
int test_camelCaser(char **(*camelCaser)(const char *),
                    void (*destroy)(char **)) {
    // TODO: Return 1 if the passed in function works properly; 0 if it doesn't.
  
  int check = 1;
  //printf("statr\n");
  //test
  char** ptr12 = (*camelCaser)("?&$%@");
  if(strcmp(ptr12[0], "") || strcmp(ptr12[1], "") || strcmp(ptr12[2], "") || strcmp(ptr12[3], "") || strcmp(ptr12[4], "") || ptr12[5] != NULL) {
      check = 0;
    }
  destroy(ptr12);

  //printf("hey\n");
  char** ptr13 = (*camelCaser)("12. ABC eDF!");
  if(strcmp(ptr13[0], "12") || strcmp(ptr13[1], "abcEdf") || ptr13[2] != NULL) {
      check = 0;
    }
    destroy(ptr13);
  
    char** ptr14 = (*camelCaser)("add      1a.ff.");
    if(strcmp(ptr14[0], "add1A") || strcmp(ptr14[1], "ff") || ptr14[2] != NULL)
      {

	check = 0;
      }
    destroy(ptr14);
    //printf("hi\n");
    //werid sentences
    char** ptr21 = (*camelCaser)("       . no ^ ");
    //printf("%s\n", ptr21[0]);
    //printf("%s\n", ptr21[1]);
    if(strcmp(ptr21[0], "") || strcmp(ptr21[1], "no") || ptr21[2] != NULL) {
	check = 0;
      }
    destroy(ptr21);

    char** ptr22 = (*camelCaser)("23 ab?.. a56b !");
    if(strcmp(ptr22[0], "23ab") || strcmp(ptr22[1], "") || strcmp(ptr22[2], "") || strcmp(ptr22[3], "a56b") || ptr22[4] != NULL)
      {
	check = 0;
      }
    destroy(ptr22);

    char** ptr23 = (*camelCaser)(" AB   ) A B     ");
    if(strcmp(ptr23[0], "ab")) {
      check = 0;
    }
    destroy(ptr23);

    //escape sequences
    //test
    char** ptr31 =  (*camelCaser)("\r \t \v \n \f.");
    if(strcmp(ptr31[0], "") || ptr31[1] != NULL)
      {
	check = 0;
      }
    destroy(ptr31);
    //printf("nononono\n");
     char** ptr33 = (*camelCaser)(" ABC\n tEF* EDC \b\v\tf~");
       if(strcmp(ptr33[0], "abcTef") || strcmp(ptr33[1], "edc\bF") || ptr33[2] != NULL)
       {
	 check = 0;
       }
     destroy(ptr33);
     
     //NULL, empty string, and no punct

     //test
     //printf("hi\n");
     char** ptr41 = (*camelCaser)(NULL);
     if(ptr41 != NULL)
       {
	 //printf("can us\n");
	 check = 0;
	 destroy(ptr41);
	 return check;
       }
     //destroy(ptr41);

     //printf("pass\n");
     char* a= NULL;
     char** ptr42 = camelCaser(a);
     //printf("ri\n");
     if(ptr42 != NULL)
       {
	 check = 0;
	 destroy(ptr42);
	 return check;
       }
     destroy(ptr42);
     //printf("what\n");
     //test
     char** ptr43 = (*camelCaser)(" abc   efg    ");
     if(*ptr43 != NULL)
       {
	 check = 0;
	 destroy(ptr43);
	 return check;
       }
     
     destroy(ptr43);
     //printf("here12\n");
     //test
     
     char* c = "\0.";
     char** ptr44 = camelCaser(c);
     if((!ptr44) || ((*ptr44) != 0))
       {
	 check = 0;
	 destroy(ptr44);
	 return check;
       }
     destroy(ptr44);
     
     //printf("45\n");
     //test
     char** ptr45 = (*camelCaser)("game?\0gg");
     if(strcmp(ptr45[0], "game") != 0 || ptr45[1] != NULL)
       {
	 check = 0;
       }
     destroy(ptr45);
     //     printf("heere?\n");
     //test
     char** ptr51 = (*camelCaser)("The Heisenbug is an incredible creature. Facenovel servers get their power from its indeterminism. Code smell can be ignored with INCREDIBLE use of air freshener. God objects are the new religion");

     if(strcmp(ptr51[0], "theHeisenbugIsAnIncredibleCreature") != 0 ||
	strcmp(ptr51[1], "facenovelServersGetTheirPowerFromItsIndeterminism") != 0 ||
       strcmp(ptr51[2], "codeSmellCanBeIgnoredWithIncredibleUseOfAirFreshener") != 0 || 
       ptr51[3] != NULL)
       {
	 check = 0;
	 
       }
     destroy(ptr51);

     return check;
  
}
