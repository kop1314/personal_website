/**
* Utilities Unleashed Lab
* CS 241 - Fall 2018
*/
#include <string.h>
#include <ctype.h>
#include "format.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int checkEV(char* str)
{
  return str[0] == '%';
  
}

char* getVar(char* str)
{
  size_t index = 0;
  size_t pos = 0;
  //printf("str is %s\n", str);
  while(*(str+index))
    {
      if(*(str+index) == '=')
	{
	  pos = index;
	  break;
	}
      index++;
    }
  //printf("pos is %zu\n", pos);
  char* var = calloc(pos + 1, sizeof(char));
  var[pos] = '\0';
  strncpy(var, str, pos);
  //printf("var is %s\n", var);
  return var;

}
 
char** split(char* str){
  char* temp = calloc(strlen(str) + 1, sizeof(char));
  temp[strlen(str)] = '\0';
  strcpy(temp, str);
  if(strchr(temp, '=') == NULL)
    {
      free(temp);
      return NULL;
    }
  free(temp);
  char* var = strtok(str, "=");
  var = strtok(NULL, "=");
  size_t index = 0;
  size_t numOfCo = 0;
  while(var[index])
    {
      if(var[index] == ',')
	{
	  numOfCo++;
	}
      index++;
    }
  char** output = (char**)calloc(numOfCo + 2, sizeof(char*));
  index = 0;
  char* token = strtok(var, ",");
  while(token)
    {
      output[index] = token;
      token = strtok(NULL, ",");
      index++;
    }
  output[numOfCo + 1] = NULL;
  return output;
}

void destroy(char** input)
{
  size_t index = 0;
  while(input[index])
    {
      //printf("free ele %zu\n", index);
      free(*(input + index));
      index++;
    }
  free(input);
}


int main(int argc, char *argv[]) {
  if(argc < 3){
    print_env_usage();
  }
  
  //printf("start compare --\n");
  if(strcmp(argv[1], "--")==0)
    {
      char*cmd = argv[2];
      int status1;
      pid_t f = fork();
      if(f == -1) {print_fork_failed();}
      if(f == 0)
	{
	  //printf("execute cmd\n");
	  execvp(cmd, argv + 2);
	  print_exec_failed();
	}
      else
	{
	  waitpid(f, &status1, 0);
	  //printf("I am done\n");
	  return 0;
	}
     
    }
  //printf("-- not at pos 1\n");
  //check "--"
  size_t indicatorIndex = 0;
  for(size_t i = 0; i< (size_t)argc; i++)
    {
      if(strcmp(argv[i], "--") == 0)
	{
	  indicatorIndex = i;
	}
    }
  //printf("indicatorIndex is %zu\n", indicatorIndex);
  if(indicatorIndex < 1)
    {
      print_env_usage();
    }

  //get command
  char* cmd = argv[indicatorIndex + 1];
  
  int N;
  if(strcmp(argv[1], "-n")==0  && strlen(argv[2]) == 1 && isdigit(*argv[2]))	
    {
      //printf("It is normal usage\n");
      N = atoi(argv[2]);
      //printf("N is %d\n", N);
      size_t indexArg = 3;
      if(indicatorIndex <= indexArg)
	{
	  print_env_usage();
	}
     
      //check N match num of values
      while(indexArg < indicatorIndex)
	{
	  size_t index2 = 0;
	  size_t numOfCo = 0;
	  while(index2 < strlen(argv[indexArg]))
	    {
	      if(*(argv[indexArg] + index2) ==  ',')
		{
		  numOfCo++;
		}

	      index2++;
	    }
	 
	  if((size_t)N!=(numOfCo + 1) && numOfCo != 0)
	    {
	      print_env_usage();
	    }
	  indexArg++;
	}
      //printf("N = %d  match # of values!\n", N);
      for(size_t j = 0; j < (size_t) N; j++)
	{
	  //printf("pos of -- is %zu\n", indicatorIndex);
	  for(size_t i = 3; i < indicatorIndex; i++)
	    {
	      char temp[strlen(argv[i])];
	      strcpy(temp, argv[i]);
	      char** values = split(temp);
	      if(values == NULL)
		{
		 
		  print_env_usage();
		}
	      //one value
	      if(values[1] == NULL)
		{
		  //has %
		  //printf("checkEV\n");
		  if(checkEV(values[0]))
		    {
		      //printf("has not j is %zu, i is %zu\n", j, i);
		      //exist
		      if(getenv(values[0] + 1) != NULL)
			{
			  char* var = getVar(argv[i]);
			  //success
			  if(setenv(var, getenv(values[0] + 1), 1) == 0)
			    {
			      free(var);
			    }
			  //failed
			  else
			    {
			      free(var);
			      free(values);
			      print_environment_change_failed();
			    }
			}
		      else{

			char* var = getVar(argv[i]);
			if(setenv(var, "", 1) == 0)
			  {
			    free(var);
			  }
			else
			  {
			    free(var);
			    free(values);
			    print_environment_change_failed();

			  }
		      }
		    }
		  else
		    {
		      //printf("has no not j is %zu, i is %zu\n", j, i);
		      char* var = getVar(argv[i]);
		      if(setenv(var, values[0], 1) ==0)
			{
			  free(var);
			}
		      else
			{
			  free(var);
			  free(values);
			  print_environment_change_failed();
			}
		    }
		}
	      else
		{
		  //printf("more values j is %zu, i is %zu", j, i);
		  char* var = getVar(argv[i]);
		  //printf("argv[0] is %s\n", argv[i]);
		  //printf("var is %s values is %s\n", var, values[j]);
		  if(setenv(var, values[j], 1) == 0)
		    {
		      //printf("more values j is %zu, i is %zu\n", j, i);
		      free(var);
		    }
		  else
		    {
		      free(var);
		      free(values);
		      //printf("failed more values j is %zu, i is %zu\n", j, i);
		      print_environment_change_failed();
		    }
					
		}
	      
	      //printf("destroy values\n");
	      free(values);
	   }
	  
	  //call cmd
	  
	  int status1;
	  pid_t f = fork();
	  if(f == -1) {print_fork_failed();}
	  if(f == 0)
	    {
	      //printf("execute cmd\n");
	      execvp(cmd, argv + indicatorIndex + 1);
	      print_exec_failed();
	    }
	  else
	    {
	      waitpid(f, &status1, 0);
	      //printf("I am done with one cmd\n");
	    } 
	}
    }
  else
    {
      for(size_t i = 1; i < indicatorIndex; i++)
	{
	  char temp[strlen(argv[i])];
	  strcpy(temp, argv[i]);
	  char** values = split(temp);
	  if(values == NULL)
	    {
	      print_env_usage();
	    }
	  if(values[1] != NULL)
	    {
	      free(values);
	      print_env_usage();
	    }
	  else
	    {
	      if(checkEV(values[0]))
		    {
		      //printf("has not j is %zu, i is %zu\n", j, i);
		      //exist
		      if(getenv(values[0] + 1) != NULL)
			{
			  char* var = getVar(argv[i]);
			  //success
			  if(setenv(var, getenv(values[0] + 1), 1) == 0)
			    {
			      free(var);
			    }
			  //failed
			  else
			    {
			      free(var);
			      free(values);
			      print_environment_change_failed();
			    }
			}
		      else{
		       	char* var = getVar(argv[i]);
			if(setenv(var, "", 1) == 0)
			  {
			    free(var);
			  }
			else
			  {
			    free(var);
			    free(values);
			    print_environment_change_failed();

			  }
		      }
		    }
		  else
		    {
		      //printf("has no not j is %zu, i is %zu\n", j, i);
		      char* var = getVar(argv[i]);
		      if(setenv(var, values[0], 1) ==0)
			{
			  free(var);
			}
		      else
			{
			  free(var);
			  free(values);
			  print_environment_change_failed();
			}
		    }
	    }
	  free(values);
	  
	}
       
	  //call cmd
	  
	  int status1;
	  pid_t f = fork();
	  if(f == -1) {print_fork_failed();}
	  if(f == 0)
	    {
	      //printf("execute cmd\n");
	      execvp(cmd, argv + indicatorIndex + 1);
	      print_exec_failed();
	    }
	  else
	    {
	      waitpid(f, &status1, 0);
	      //printf("I am done with one cmd\n");
	    }
      
    }
  return 0;
}
