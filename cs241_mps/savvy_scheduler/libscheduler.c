/**
* Savvy_scheduler Lab
* CS 241 - Fall 2018
*/
//cowork with chenhao3
#include "libpriqueue/libpriqueue.h"
#include "libscheduler.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "print_functions.h"
typedef struct _job_info {
    int id;
  int priority;
    /* Add whatever other bookkeeping you need into this struct. */

  double arrival_time;
  //int requeue_time;
  double run_time;
  double start_time;
  double end_time;
  double remain_time;
} job_info;

priqueue_t pqueue;
scheme_t pqueue_scheme;
comparer_t comparision_func;
static int job_num = 0;
static double total_waiting_time = 0;
static double total_turnaround_time = 0;
static double total_response_time = 0;


void scheduler_start_up(scheme_t s) {
    switch (s) {
    case FCFS:
        comparision_func = comparer_fcfs;
        break;
    case PRI:
        comparision_func = comparer_pri;
        break;
    case PPRI:
        comparision_func = comparer_ppri;
        break;
    case PSRTF:
        comparision_func = comparer_psrtf;
        break;
    case RR:
        comparision_func = comparer_rr;
        break;
    case SJF:
        comparision_func = comparer_sjf;
        break;
    default:
        printf("Did not recognize scheme\n");
        exit(1);
    }
    priqueue_init(&pqueue, comparision_func);
    pqueue_scheme = s;
    // Put any set up code you may need here
}

static int break_tie(const void *a, const void *b) {
    return comparer_fcfs(a, b);
}

int comparer_fcfs(const void *a, const void *b) {
  job_info *a_job_info = (job_info*)(((job*)a)->metadata);
  job_info *b_job_info = (job_info*)(((job*)b)->metadata);
  if(a_job_info -> arrival_time < b_job_info -> arrival_time)
    {
      return -1;
    }
  if(a_job_info -> arrival_time > b_job_info -> arrival_time)
    {
      return 1;
    }
  return 0;
}

int comparer_ppri(const void *a, const void *b) {
    // Complete as is
    return comparer_pri(a, b);
}

int comparer_pri(const void *a, const void *b) {
  job_info *a_job_info = (job_info*)(((job*)a)->metadata);
  job_info *b_job_info = (job_info*)(((job*)b)->metadata);
  if(a_job_info -> priority < b_job_info -> priority)
    {
      return -1;
    }
  if(a_job_info -> priority > b_job_info -> priority)
    {
      return 1;
    }
  return break_tie(a, b);;
}

int comparer_psrtf(const void *a, const void *b) {
  job_info *a_job_info = (job_info*)(((job*)a)->metadata);
  job_info *b_job_info = (job_info*)(((job*)b)->metadata);
  if(a_job_info -> remain_time < b_job_info -> remain_time)
    {
      return -1;
    }
  if(a_job_info -> remain_time > b_job_info -> remain_time)
    {
      return 1;
    }
  return break_tie(a, b);
}

int comparer_rr(const void *a, const void *b) {
  return comparer_pri(a, b);
}

int comparer_sjf(const void *a, const void *b) {
  job_info *a_job_info = (job_info*)(((job*)a)->metadata);
  job_info *b_job_info = (job_info*)(((job*)b)->metadata);
  if(a_job_info -> run_time < b_job_info -> run_time)
    {
      return -1;
    }
  if(a_job_info -> run_time > b_job_info -> run_time)
    {
      return 1;
    }
  return break_tie(a, b);
}

// Do not allocate stack space or initialize ctx. These will be overwritten by
// gtgo
void scheduler_new_job(job *newjob, int job_number, double time,
                       scheduler_info *sched_data) {
    // TODO complete me!
  job_info *new_job_info = malloc(sizeof(job_info));
  new_job_info -> id = job_number;
  new_job_info -> priority = sched_data -> priority;
  new_job_info -> arrival_time = time;
  new_job_info -> run_time = sched_data -> running_time;
  new_job_info -> start_time = -1;
  new_job_info -> end_time = -1;
  new_job_info -> remain_time = sched_data -> running_time;
  newjob -> metadata = new_job_info;

  priqueue_offer(&pqueue, newjob);
}

job *scheduler_quantum_expired(job *job_evicted, double time) {
    // TODO complete me!
  if(!priqueue_size(&pqueue))
    {
      return NULL;
    }
  if(!job_evicted)
    {
      job* next_job = priqueue_peek(&pqueue);
      job_info *next_job_info = (job_info*)(next_job -> metadata);
      if(!next_job_info -> remain_time)
	{
	  priqueue_poll(&pqueue);
	  return priqueue_peek(&pqueue);
	}
      next_job_info -> start_time = time;
      return next_job;
    }

  job_info* e_job_info = (job_info*)job_evicted -> metadata;
  if(e_job_info -> start_time == -1)
    {
      e_job_info -> start_time = time;
    }
  if(pqueue_scheme != PPRI && pqueue_scheme != PRI && pqueue_scheme != PSRTF && pqueue_scheme != RR)
    {
      return job_evicted;
    }
  else
    {
      e_job_info -> remain_time -= time - e_job_info -> start_time;
      job* next = priqueue_poll(&pqueue);
      job_info* n_info = (job_info*)next -> metadata;
      
      if(pqueue_scheme == RR){
	n_info->priority += 100;
      }
      
      
      if(n_info -> remain_time > 0)
	{
	  priqueue_offer(&pqueue, next);
	}
      return priqueue_peek(&pqueue);
    }
    return NULL;
}

void scheduler_job_finished(job *job_done, double time) {
    // TODO complete me!
  job_info *job_done_info = (job_info*)(job_done -> metadata);
  job_done_info -> end_time = time;
  job_done_info -> remain_time = 0;

  job_num++;
  double waiting_time = job_done_info -> end_time - job_done_info -> arrival_time - job_done_info -> run_time;
  double response_time = job_done_info -> start_time - job_done_info -> arrival_time;
  double turnaround_time = job_done_info -> end_time - job_done_info -> arrival_time;
  
  total_waiting_time += waiting_time;
  total_response_time += response_time;
  total_turnaround_time += turnaround_time;

  free(job_done_info);

}

static void print_stats() {
    fprintf(stderr, "turnaround     %f\n", scheduler_average_turnaround_time());
    fprintf(stderr, "total_waiting  %f\n", scheduler_average_waiting_time());
    fprintf(stderr, "total_response %f\n", scheduler_average_response_time());
}

double scheduler_average_waiting_time() {
    // TODO complete me!
    return total_waiting_time / job_num;
}

double scheduler_average_turnaround_time() {
    // TODO complete me!
    return total_turnaround_time / job_num;
}

double scheduler_average_response_time() {
    // TODO complete me!
    return total_response_time / job_num;
}

void scheduler_show_queue() {
    // Implement this if you need it!
}

void scheduler_clean_up() {
    priqueue_destroy(&pqueue);
    print_stats();
}
