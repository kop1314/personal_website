/**
* Utilities Unleashed Lab
* CS 241 - Fall 2018
*/
#include "format.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
  if(argc <2) print_time_usage();
  struct timespec start, stop;
  double accum;

  int status;
  if(clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
    //perror("clock gettime");
      exit(EXIT_FAILURE);
    }
  // printf("get start time\n");
  pid_t f = fork();

  if(f == -1) {print_fork_failed();}
  if(f == 0) {
    
    //do stuff
    execvp(*(argv + 1), argv + 1);
    print_exec_failed();
    
  }
  else {
    waitpid(f, &status, 0);
    if(clock_gettime(CLOCK_MONOTONIC, &stop) == -1) {
      //perror("clock gettime");
      exit(EXIT_FAILURE);
    }
    //printf("get stop time\n");
    long s = stop.tv_sec - start.tv_sec;
    long ns = stop.tv_nsec - start.tv_nsec;

    if(start.tv_nsec > stop.tv_nsec) { --s; ns += 1000000000;}
    accum = s + ns / 1000000000.0;
    //printf("Time is %lf\n", accum);
    display_results(argv, accum);
  }
  return 0;
}
