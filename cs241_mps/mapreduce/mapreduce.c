/**
*  Lab
* CS 241 - Fall 2018
*/

#include "utils.h"
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static int mapper_pipe[100][2];
static int reducer_pipe[2];

static void wait_for_reducer(char* str, pid_t* process, int num_process)
{
  for(int i = 0; i < num_process; i++)
    {
      int status = 0;
      pid_t pid = waitpid(process[i], &status, 0);
      if(pid != -1 && WIFEXITED(status))
	{
	  if(WEXITSTATUS(status))
	    {
	      print_nonzero_exit_status(str, WEXITSTATUS(status));
	    }
	}
      
    }
}

int main(int argc, char **argv) {
  if(argc != 6)
    {
      print_usage();
      return 1;
    }

  int num_mapper = atoi(argv[5]);
  
    // Create an input pipe for each mapper.
  for(int i = 0; i < num_mapper; i++)
    {
      pipe2(mapper_pipe[i], O_CLOEXEC);
    }
  
    // Create one input pipe for the reducer.
  pipe2(reducer_pipe, O_CLOEXEC);
    // Open the output file.
  char *out_f = argv[2];
  int out = open(out_f, O_CREAT | O_TRUNC | O_RDWR, S_IWUSR | S_IRUSR);
		 
    // Start a splitter process for each mapper.
  char* in_f = argv[1];

  pid_t splitter_process[num_mapper];

  for(int i = 0; i < num_mapper; i++)
    {
      splitter_process[i] = fork();

      if(splitter_process[i] == 0)
	{
	  char array[256];
	  sprintf(array, "%d", i);
	  close(mapper_pipe[i][0]);
	  dup2(mapper_pipe[i][1], 1);

	  execl("splitter", "splitter", in_f, argv[5], array, NULL);
	  exit(1);
	}
    }
  
    // Start all the mapper processes.
  pid_t mapper_process[num_mapper];
  for(int i = 0; i < num_mapper; i++)
    {
      mapper_process[i] = fork();

      if(mapper_process[i] == 0)
	{
	  close(mapper_pipe[i][1]);
	  dup2(mapper_pipe[i][0], 0);
	  dup2(reducer_pipe[1], 1);
	  execl(argv[3], argv[3], NULL);
	  exit(1);
	}
      
    }
  
    // Start the reducer process.
  pid_t reducer_process = fork();

  if(reducer_process == 0)
    {
      dup2(reducer_pipe[0], 0);
      dup2(out, 1);
      execl(argv[4], argv[4], NULL);
      exit(1);
    }
  close(reducer_pipe[0]);
  close(reducer_pipe[1]);

  //close mapper pipe
  for(int i = 0; i < num_mapper; i++)
    {
      close(mapper_pipe[i][0]);
      close(mapper_pipe[i][1]);
    }
  
    // Wait for the reducer to finish.
  wait_for_reducer("splitter", splitter_process, num_mapper);
  wait_for_reducer(argv[3], mapper_process, num_mapper);
  wait_for_reducer(argv[4], &reducer_process, 1);
  
    // Print nonzero subprocess exit codes.
  
    // Count the number of lines in the output file.
  print_num_lines(out_f);
  close(out);
  
    return 0;
}
