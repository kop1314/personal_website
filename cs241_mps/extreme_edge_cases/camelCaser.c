/**
* Extreme Edge Cases Lab
* CS 241 - Fall 2018
*/

#include "camelCaser.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

char **camel_caser(const char *input_str) {
  if(input_str == NULL || !strlen(input_str)){
    return NULL;
  }
  size_t len = 0;
  size_t numPunct = 0;
  for(size_t i = 0; i < strlen(input_str); i++)
    {
      if(ispunct(*(input_str + i)))
	{
	  numPunct ++;
	}
      if(isalpha(*(input_str + i) || isdigit(*(input_str + i))))
	{
	  len++;
	}
    }
  if(numPunct == 0){
    char** output = (char**)calloc(1, sizeof(char*));
    output[0] = NULL;
    return output;
  }
  //printf("I get numPunct: %zu\n", numPunct);
  char** output = (char**)calloc(numPunct * sizeof(char*) + 1, sizeof(char*));
      size_t index = 0;
      size_t fCPos = 0;
      
      //loop char**
      while(index < numPunct)
	{
	  //printf("start loop!\n");
	  //printf("fCPos: %zu\n", fCPos);;
	  int isFirstChar = 1;
	  int isFirstW = 1;
	  size_t index2 = fCPos;
	  size_t numChar = 0;
	  while(!ispunct(*(input_str + index2))&&index2 < strlen(input_str))
	    {
	      if(!ispunct(*(input_str + index2)) && !isspace(*(input_str + index2)))
		{
		  numChar++;
		}
	      index2++;
	    }
	  //printf("get numChar %zu\n", numChar);
	    char* output_s = malloc(numChar*sizeof(char) + 1);
	    output_s[numChar] = '\0';
	    if(numChar !=0){
	      size_t index3 = fCPos;
	      //size_t spacePos = 0;
	      size_t charPos = 0;
	      while(!ispunct(*(input_str + index3)) && index3 < strlen(input_str))
		{
		  //not full ascii table
		  if(isalpha(*(input_str + index3)) /*|| isdigit(*(input_str + index3))*/)
		    {
		      if(isFirstChar == 1 &&isFirstW == 1)
			{
			  //strncat(output_s, input_str + index3, 1*sizeof(char));

			  char c = *(input_str + index3);
			  *(output_s + charPos) = tolower(c);
			  
			  /*
			  *(output_s + charPos) = *(input_str + index3);
			  *(output_s + charPos) = tolower(*(output_s + charPos));
			  */
			  isFirstChar = 0;
			  isFirstW = 0;
			  charPos++;
			}
		      else if(isFirstChar == 1 && isFirstW ==0)
			{
			  //strncat(output_s, input_str + index3, 1);
			  char c = *(input_str + index3);
			  *(output_s + charPos) = toupper(c);
			  /**(output_s + charPos) = *(input_str + index3);   
		       	  *(output_s + charPos) = toupper(*(output_s + charPos));
	       		  */
			  isFirstChar = 0;
       			  charPos++;
			}
		      else
			{
			  //strncat(output_s, input_str + index3, 1);
			  char c = *(input_str + index3);
			  *(output_s + charPos) = tolower(c);
			  /**(output_s + charPos) = *(input_str + index3);
			  *(output_s + charPos) = tolower(*(output_s + charPos));
			  */
			  charPos++;
			 
			}
		      // index3 ++; 
		    }
		  //else if(isspace(*(input_str + index3)))
		  else if(isspace(*(input_str + index3)))
		  {
		    /**(output_s + charPos) = *(input_str + index3);
		       charPos++;*/
		    isFirstChar = 1;
		  }
		  else {
		    //isFirstChar = 1;
		    char c = *(input_str + index3);
		    *(output_s + charPos) = c;
		    //*(output_s + charPos) = *(input_str + index3);
	             charPos++;
		  }
		  //isFirstChar = 1;
		  index3++;    
		}
	      isFirstW = 1;
	      isFirstChar = 1;
	      fCPos = index3 + 1;
	    }
	    else{
	      //fCPos ++;
	      fCPos = index2 + 1;
	    }
	    //printf("store ptr\n");
	    //output_s[numChar] = '\0';
	    output[index] = output_s;
	    index++;
	}
  output[numPunct] = NULL;    
  return output;
}

void destroy(char **result) {
  if(!result)
    {
      return;
    }

  size_t i = 0;
  while(result[i])
    {
      free(result[i]);
      i++;
    }
  free(result);
  return;
}
