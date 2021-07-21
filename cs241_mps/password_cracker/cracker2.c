/**
* Password Cracker Lab
* CS 241 - Fall 2018
*/
/*
#include "cracker2.h"
#include "format.h"
#include "utils.h"
#include "thread_status.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <crypt.h>
#include <string.h>

#define SIZE 20480

char usernames[SIZE][SIZE];
char hash_pws[SIZE][SIZE];
char pws[SIZE][SIZE];
//char dots[SIZE][SIZE];
char c_pw[SIZE][SIZE];
long unknown_len[SIZE];
static pthread_mutex_t m;
pthread_barrier_t b1;
pthread_barrier_t b2;
//static char* c_pw;
static long id = 0;
static int num_thread = 0;
int s[SIZE];
static int total_hashes = 0;
static int cancel = 0;
//flag for done with single taks
static int done = 0;
//flag for done with read all lines
static int done_crypt = 0;
static double total_cpu_time = 0;

void* my_crypt(void* t_id)
{
  while(1)
    {
      pthread_barrier_wait(&b1);
      double start_cpu_t = getCPUTime();

      pthread_mutex_lock(&m);
      if(done_crypt)
	{
	  pthread_mutex_unlock(&m);
	  break;
	}
      pthread_mutex_unlock(&m);

      struct crypt_data cdata;
      cdata.initialized = 0;

      char* username = strdup(usernames[id]);
      char* hash_pw = strdup(hash_pws[id]);
      char* pw = strdup(pws[id]);

      long start_index;
      long num;
      int prefix = getPrefixLength(pw);
      getSubrange(unknown_len[id], num_thread, ((long)t_id) + 1, &start_index, &num);
      setStringPosition(pw + prefix, start_index);
      v2_print_thread_start(((long)t_id) + 1, username, start_index, pw);
      
      int found = 0;
      int hashes = 1;

      while(!found && !done_crypt)
	{
	  pthread_mutex_lock(&m);
	  if(done)
	    {
	      cancel = 1;
	      //pthread_mutex_lock(&m);
	      v2_print_thread_result(((long)t_id) + 1, hashes, 1);
	      //pthread_mutex_unlock(&m);
	      break;
	    }
	  pthread_mutex_unlock(&m);

	  int letter = incrementString(pw + prefix);
	  if(letter)
	    {
	      if(strcmp(hash_pw, crypt_r(pw, "xx", &cdata)) == 0)
		{
		  found = 1;
		  hashes++;
		  pthread_mutex_lock(&m);
		  strcpy(c_pw[id], pw);
		  //memcpy(c_pw[id], pw, 8);
		  //printf("password is %s\n", c_pw[id]);
		  s[id] = 1;
		  done = 1;
		  v2_print_thread_result(((long)t_id) + 1, hashes, !found);
		  pthread_mutex_unlock(&m);
		  break;
		}
	    }
	  else
	    {
	      break;
	    }
	  hashes ++;
	}

      double cpu_t = getTime() - start_cpu_t;

      pthread_mutex_lock(&m);
      total_hashes += hashes;
      total_cpu_time += cpu_t;

      if(!found &&!cancel)
	{
	  v2_print_thread_result(((long)t_id) + 1, hashes, 2);
	}
      pthread_mutex_unlock(&m);

      free(username);
      free(hash_pw);
      free(pw);
      pthread_barrier_wait(&b2);
    }
  return NULL;
}

int start(size_t thread_count) {
    // TODO your code here, make sure to use thread_count!
    // Remember to ONLY crack passwords in other threads
  pthread_barrier_init(&b1, NULL, thread_count + 1);
  pthread_barrier_init(&b2, NULL, thread_count + 1);
  pthread_mutex_init(&m, NULL);
  num_thread = thread_count;
  pthread_t threads[thread_count];
  
  //start threads
  for(size_t i = 0; i < thread_count; i++)
    {
      pthread_create(&threads[i], NULL, &my_crypt, (void*) i);
    }

  char* buffer = NULL;
  size_t capacity = 0;
  ssize_t sizeOfLine;
  long id = 0;
  while((sizeOfLine = getline(&buffer, &capacity, stdin)) != EOF)
    {
      //calculate cpu time
      double start_t = getTime();
      double start_cpu_t = getCPUTime();
      
      if(sizeOfLine > 0 && buffer[sizeOfLine - 1] == '\n')
	{
	  buffer[sizeOfLine - 1] = '\0';
	}

      char* first_space = strchr(buffer, ' ');
      char* second_space = strchr(first_space + 1, ' ');
      char* dot_loc = strchr(second_space + 1, '.');
      
      memcpy(usernames[id], buffer, first_space - buffer);
      memcpy(hash_pws[id], first_space + 1, second_space - first_space - 1);
      strcpy(pws[id], second_space + 1);
     
      unknown_len[id] = strlen(buffer) - (dot_loc - buffer);
      free(buffer);
      buffer = NULL;

      //print info
      v2_print_start_user(usernames[id]);

      pthread_barrier_wait(&b1);
      pthread_barrier_wait(&b2);

      //printf("c_pw is %s\n", c_pw[id]);
      pthread_mutex_lock(&m);
      //printf("c_pw is %s\n", c_pw[id]);
      v2_print_summary(usernames[id], c_pw[id], total_hashes, getTime() - start_t, getCPUTime() - start_cpu_t,!s[id]);
      pthread_mutex_unlock(&m);

      pthread_mutex_lock(&m);
      id++;
      cancel = 0;
      done = 0;
      total_cpu_time = 0;
      //s = 0;
      pthread_mutex_unlock(&m);
    }

  //pthread_barrier_wait(&b1);
  pthread_mutex_lock(&m);
  done_crypt = 1;
  pthread_mutex_unlock(&m);
  pthread_barrier_wait(&b1);
  free(buffer);

  for(size_t i = 0; i < thread_count; i++)
    {
      pthread_join(threads[i], NULL);
    }

  pthread_mutex_destroy(&m);
    return 0; // DO NOT change the return code since AG uses it to check if your
              // program exited normally
}
*/

/**
 * Password Cracker Lab
 * CS 241 - Fall 2018
 */

#include "cracker2.h"
#include "format.h"
#include "utils.h"
#include "thread_status.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <crypt.h>
#include <string.h>

#define SIZE 10480

char usernames[SIZE][SIZE];
char hash_pws[SIZE][SIZE];
char pws[SIZE][SIZE];
char c_pw[SIZE][SIZE];
long unknown_len[SIZE];
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t b1;
pthread_barrier_t b2;
//static char* c_pw;
static long id = 0;
static int num_thread = 0;
static int s = 0;
static int total_hashes = 0;
static int cancel = 0;
static int done = 0;
static int done_crypt = 0;
static double total_cpu_time = 0;

void* my_crypt(void* t_id)
{
  while(1)
    {
      pthread_barrier_wait(&b1);
      double start_cpu_t = getCPUTime();
      pthread_mutex_lock(&m);
      if(done_crypt)
	{
	  pthread_mutex_unlock(&m);
	  break;
	}
      pthread_mutex_unlock(&m);

      struct crypt_data cdata;
      cdata.initialized = 0;

      char* username = strdup(usernames[id]);
      char* hash_pw = strdup(hash_pws[id]);
      char* pw = strdup(pws[id]);

      long start_index = 0;
      long count = 0;
      int prefix = getPrefixLength(pw);
      getSubrange(unknown_len[id], num_thread, ((long)t_id) + 1, &start_index, &count);
      setStringPosition(pw + prefix, start_index);
      v2_print_thread_start(((long)t_id) + 1, username, start_index, pw);

      int found = 0;
      int hashes = 1;

      while(!found && !done_crypt)
	{
	  pthread_mutex_lock(&m);
	  if(done)
	    {
	      cancel = 1;
	      v2_print_thread_result(((long)t_id) + 1, hashes, 1);
	      pthread_mutex_unlock(&m);
	      break;
	    }
	  pthread_mutex_unlock(&m);

	  int letter = incrementString(pw + prefix);
	  if(letter)
	    {
	      if(strcmp(hash_pw, crypt_r(pw, "xx", &cdata)) == 0)
		{
		  found = 1;
		  hashes++;
		  pthread_mutex_lock(&m);
		  strcpy(c_pw[id], pw);
		  s = 1;
		  done = 1;
		  v2_print_thread_result(((long)t_id) + 1, hashes, !found);
		  pthread_mutex_unlock(&m);
		  break;
		}
	    }
	  else
	    {
	      break;
	    }
	  hashes ++;
	}

      double cpu_t = getTime() - start_cpu_t;

      pthread_mutex_lock(&m);
      total_hashes += hashes;
      total_cpu_time += cpu_t;

      if(!found &&!cancel)
	{
	  v2_print_thread_result(((long)t_id) + 1, hashes, 2);
	}
      pthread_mutex_unlock(&m);

      free(username);
      free(hash_pw);
      free(pw);
      pthread_barrier_wait(&b2);
    }
  return NULL;
}

int start(size_t thread_count) {
  // TODO your code here, make sure to use thread_count!
  // Remember to ONLY crack passwords in other threads

  pthread_t threads[thread_count];
  pthread_barrier_init(&b1, NULL, thread_count + 1);
  pthread_barrier_init(&b2, NULL, thread_count + 1);
  num_thread = thread_count;

  //start threads
  for(size_t count = 0; count < thread_count; count++)
    {
      pthread_create(&threads[count], NULL, &my_crypt, (void*) count);
    }

  char* buffer = NULL;
  size_t capacity = 0;
  ssize_t sizeOfLine;
  long id = 0;
  while((sizeOfLine = getline(&buffer, &capacity, stdin)) != -1)
    {
      double start_t = getTime();
      double start_cpu_t = getCPUTime();

      if(sizeOfLine > 0 && buffer[sizeOfLine - 1] == '\n')
	{
	  buffer[sizeOfLine - 1] = 0;

	}
      
      char* first_space = strchr(buffer, ' ');
      char* second_space = strchr(first_space + 1, ' ');
      char* dot_loc = strchr(second_space + 1, '.');

      memcpy(usernames[id], buffer, first_space - buffer);
      memcpy(hash_pws[id], first_space + 1, second_space - first_space - 1);
      strcpy(pws[id], second_space + 1);
      
      unknown_len[id] = strlen(buffer) - (dot_loc - buffer);
      free(buffer);
      buffer = NULL;

      //print info
      v2_print_start_user(usernames[id]);

      pthread_barrier_wait(&b1);
      pthread_barrier_wait(&b2);

      pthread_mutex_lock(&m);
      v2_print_summary(usernames[id], c_pw[id], total_hashes, getTime() - start_t, getCPUTime() - start_cpu_t, !s);
      pthread_mutex_unlock(&m);

      pthread_mutex_lock(&m);
      id++;
      cancel = 0;
      done = 0;
      total_cpu_time = 0;
      s = 0;
      pthread_mutex_unlock(&m);
    }
  pthread_mutex_lock(&m);
  done_crypt = 1;
  pthread_mutex_unlock(&m);
  pthread_barrier_wait(&b1);
  free(buffer);

  for(size_t count = 0; count < thread_count; count++)
    {
      pthread_join(threads[count], NULL);
    }

  pthread_mutex_destroy(&m);
  return 0; // DO NOT change the return code since AG uses it to check if your
  // program exited normally
}
