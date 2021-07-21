/**
* Password Cracker Lab
* CS 241 - Fall 2018
*/


#include "cracker1.h"
#include "format.h"
#include "utils.h"
#include "queue.h"
#include "thread_status.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <crypt.h>
#include <string.h>

#define SIZE 20480

queue* q;
static char usernames[SIZE][SIZE];
static char hash_pws[SIZE][SIZE];
static char pws[SIZE][SIZE];
static pthread_mutex_t m;
static int num_s = 0;
static int num_f = 0;


void* my_crypt(void* t_id)
{
  while(true)
    {
      struct crypt_data cdata;
      cdata.initialized = 0;

      pthread_mutex_lock(&m);
      long id = (long)queue_pull(q);
      queue_push(q, (char*)-1);
      pthread_mutex_unlock(&m);

      if(id == -1)
	{
	  //end of q
	  pthread_mutex_lock(&m);
	  queue_push(q, (char*)-1);
	  pthread_mutex_unlock(&m);
	  break;
	}

      char* username = strdup(usernames[id]);
      char* hash_pw = strdup(hash_pws[id]);
      char* pw = strdup(pws[id]);

      v1_print_thread_start(((long)t_id) + 1, username);
      int prefix = getPrefixLength(pw);
      setStringPosition(pw + prefix, 0);

      int hashes = 1;
      int s = 0;
      //crypt password
      while(true)
	{
	  int letter = incrementString(pw + prefix);
	  if(letter)
	    {
	      if(strcmp(crypt_r(pw, "xx", &cdata), hash_pw) == 0)
		{
		  s = 1;
		  hashes++;
		  pthread_mutex_lock(&m);
		  num_s++;
		  pthread_mutex_unlock(&m);
		  v1_print_thread_result(((long)t_id) + 1, username, pw, hashes, getThreadCPUTime(), !s);
		  break;
		}
	    }
	  else
	    {
	      break;
	    }
	  hashes++;
	}
      if(!s)
	{
	  pthread_mutex_lock(&m);
	  num_f++;
	  pthread_mutex_unlock(&m);
	  v1_print_thread_result(((long)t_id) + 1, username, pw, hashes, getThreadCPUTime(), !s);
	}
      free(username);
      free(hash_pw);
      free(pw);
    }


  return NULL;
}


int start(size_t thread_count) {
    // TODO your code here, make sure to use thread_count!
    // Remember to ONLY crack passwords in other threads
  pthread_t threads[thread_count];
  //lines = malloc(sizeof(line*) * SIZE);
  q = queue_create(-1);
  pthread_mutex_init(&m, NULL);

  /*      
  for(size_t i = 0; i < thread_count; i++)
    {
      pthread_create(&threads[i], NULL, &my_crypt, (void*) i);
    }
  */

  //printf("start read file\n");
  char* buffer = NULL;
  size_t capacity = 0;
  ssize_t sizeOfLine;
  long id = 0;
  //int l= 0;
  while((sizeOfLine = getline(&buffer, &capacity, stdin)) != -1)
    {
      if(sizeOfLine > 0 && buffer[sizeOfLine - 1] == '\n')
	{
	  buffer[sizeOfLine - 1] = '\0';
	}
      //printf("get a line %d\n", l + 1);
      //l++;
      char* first_space = strstr(buffer, " ");
      char* second_space = strstr(first_space + 1, " ");
      //char* dot_loc = strstr(second_space + 1, ".");

      //printf("store un\n");
      memcpy(usernames[id], buffer, first_space - buffer);
      //printf("store hash_pw\n");
      memcpy(hash_pws[id], first_space + 1, second_space - first_space - 1);
      //printf("store pw\n");
      strcpy(pws[id], second_space + 1);
      //printf("id is %ld, len is %zu\n", id, strlen(pws[id]));
      //printf("store loc_index\n");
      /*
      if(!dot_loc)
	{
	  memcpy(dots[id], dot_loc, buffer + strlen(buffer) - dot_loc);
	}
      else
	{
	  dots[id] = NULL;
	}
      */
      //printf("push q\n");
      queue_push(q, (void*) id);
      free(buffer);
      buffer = NULL;
      id++;
    }
  free(buffer);
  //printf("get all lines\n");

  queue_push(q, (char*)-1);
  
  for(size_t i = 0; i < thread_count; i++)
    {
      pthread_create(&threads[i], NULL, &my_crypt, (void*) i);
    }
  
  //printf("wait for thread\n");
  
  for(size_t i = 0; i < thread_count; i++)
    {
      pthread_join(threads[i], NULL);
    }
  v1_print_summary(num_s, num_f);
  pthread_mutex_destroy(&m);
  queue_destroy(q);
  


  return 0; // DO NOT change the return code since AG uses it to check if your
              // program exited normally
}
