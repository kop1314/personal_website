/**
* Networking Lab
* CS 241 - Fall 2018
*/

#include "common.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

ssize_t read_all_from_socket(int socket, char *buffer, size_t count) {
  // Your Code Here
  ssize_t len = 0;
  while(len < (ssize_t)count)
    {
      ssize_t n = read(socket, buffer + len, count - len);
      //fprintf(stderr, "n each line is %zu\n", n);
      
      if(n == 0)
	{
	  // socket disconnected
	  return 0;
	}
      if(n < 0)
	{
	  if(errno == EINTR)
	    {
	      continue;
	    }
	  if(errno == EAGAIN)
	    {
	      return len;
	    }
	  if(errno != EINTR)
	    {
	      return -1;
	    }
	}
      if((size_t)n < (count - len))
	{
	  return n;
	}
      len += n;
    }
  return len;
}

ssize_t write_all_to_socket(int socket, const char *buffer, size_t count) {
  // Your Code Here
  ssize_t len = 0;
  while(len < (ssize_t)count)
    {
      ssize_t n = write(socket, buffer + len, count - len);
      if(n == 0)
	{
	  // socket disconnected
	  return 0;
	}
      if(n < 0)
	{
	  if(errno == EINTR)
	    {
	      continue;
	    }
	  else
	    {
	      return -1;
	    }
	}
      len += n;
    }
  return len;
}
