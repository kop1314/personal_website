/**
* Parallel Make Lab
* CS 241 - Fall 2018
*/

#include "format.h"
#include "graph.h"
#include "parmake.h"
#include "parser.h"
#include "queue.h"
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define ZERO 0
#define SATISFY 2
#define VISITED 1
#define FAILED 3
#define AVAILABLE 4
#define CYCLE 5
#define INQ 6;

graph* g = NULL;
queue* q = NULL;
vector* goal_rules = NULL;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv;
static size_t num_task = 0;
//static size_t max_thread = 0;
//static size_t c_threads = 0;
static size_t q_size = 0;

static int cycle_exit(graph* graph, char* target, char* parent)
{
  //printf("check cycle\n");
  int result = 0;
  rule_t* cur_rule = (rule_t*)graph_get_vertex_value(graph, target);
  int cur_state = cur_rule -> state;
  cur_rule -> state = VISITED;
  vector* adj_list = graph_neighbors(graph, target);

  VECTOR_FOR_EACH(adj_list, child, {
      //printf("check  child %s of %s\n", (char*)child, target);
      if(parent != NULL && strcmp((char*)child, parent) == 0)
	{
	  result = 1;
	  break;
	}
      if(parent == NULL || strcmp((char*)child, parent) != 0)
	{
	  //printf("parent is not child\n");
	  rule_t*child_rule = (rule_t*)graph_get_vertex_value(graph, child);
	  if(child_rule -> state == VISITED)
	    {
	      //printf("%s is visisted\n", (char*)child);
	      result = 1;
	      break;
	    }
	  else
	    {
	      //printf("%s is not visited\n", (char*)child);
	      if(cycle_exit(graph, child, target))
		{
		  //printf("%s cycle exist in child subgraph\n", (char*)child);
		  result = 1;
		  break;
		}
	    }
	}
      
    });
    cur_rule -> state = cur_state;
    vector_destroy(adj_list);
  return result;
}

static int is_goal_rule(char* target)
{
  int ret = 0;
  VECTOR_FOR_EACH(goal_rules, rule, {
      if(strcmp(rule, target) == 0)
	{
	  ret = 1;
	  break;
	}
    });
  return ret;
}


/*
static int parent_is_goal_rule(char* target)
{
  vector* parents = graph_antineighbors(g, target);
  int ret = 0;
  //0 parent size
  if(!vector_size(parents) || (vector_size(parents) == 1 && strcmp((char*)vector_get(parents, 0), "") == 0))
    {
      vector_destroy(parents);
      return ret;
    }
  VECTOR_FOR_EACH(parents, parent, {
      if(is_goal_rule(parent))
	{
	  ret = 1;
	  break;
	}
      else
	{
	  if(parent_is_goal_rule(parent))
	    {
	      ret = 1;
	      break;
	    }
	}
    });
  vector_destroy(parents);
  return ret;
}
 
static void mark_satisfy(char* target)
{
  //printf("mark satisfy target is %s\n", target);
  rule_t* rule = (rule_t*)graph_get_vertex_value(g, target);

  if(rule -> state == SATISFY)
    {
      //printf("bye\n");
      return;
    }
  
  int rule_is_goal = is_goal_rule(target);
  if(rule_is_goal)
    {
      if(cycle_exit(g, target, NULL))
	{
	  
	  return;
	}
      int cycle_exit_in_child = 0;
      vector* children = graph_neighbors(g, target);
      VECTOR_FOR_EACH(children, child, {
	  if(cycle_exit(g, child, target))
	    {
	      cycle_exit_in_child = 1;
	      break;
	    }
	});

      vector_destroy(children);
      if(cycle_exit_in_child)
	{
	  return;
	}
    }
  else
    {
      //printf("target it is not goal rule\n");
      if(!parent_is_goal_rule(target))
	{
	  return;
	}
      else
	{
	  vector* parents = graph_antineighbors(g, target);
	  if(!vector_size(parents))
	    {
	      vector_destroy(parents);
	      return;
	    }
	  int failed_one = 0;
	  int failed_two = 0;
	  VECTOR_FOR_EACH(parents, parent, {
	      
	      if(cycle_exit(g, parent, NULL))
		{
		  failed_one = 1;
		  break;
		}
	      vector* children = graph_neighbors(g, parent);
	      VECTOR_FOR_EACH(children, child, {
		  if(cycle_exit(g, child, parent))
		    {
		      failed_two = 1;
		      vector_destroy(children);
		      break;
		    }
		});
	      
	      vector_destroy(children);
	      if(failed_one || failed_two)
		{
		  break;
		}
	    });
	  vector_destroy(parents);
	  if(failed_one || failed_two)
	    {
	      return;
	    }
      }
    }
  rule -> state = SATISFY;
}
*/

static int run_cmds(char* target)
{
  //printf("run cmds\n");
  rule_t* rule = (rule_t*)graph_get_vertex_value(g, target);
  
  for(size_t i = 0; i < vector_size(rule -> commands); i++)
    {
      //fail
      //printf("i is %zu\n", i);
      if(system(vector_get(rule -> commands, i)) != 0)
	{
	  //printf("fail loop!\n");
	  return 0;
	}
    }
  
  pthread_mutex_lock(&m);
  if(rule -> state != FAILED)
    {
      rule -> state = SATISFY;
      pthread_mutex_unlock(&m);
      //printf("good cmds\n");
      return 1;
    }
  pthread_mutex_unlock(&m);
  //printf("fail\n");
  return 0;
}


static int children_satisfy(char* target)
{
  //rule_t* rule = (rule_t*)graph_get_vertex_value(g, target);
  int satisfy = 1;
  vector* children = graph_neighbors(g, target);
  VECTOR_FOR_EACH(children, child, {
      rule_t* child_rule = (rule_t*)graph_get_vertex_value(g, child);
      if(child_rule -> state != SATISFY)
	{
	  satisfy = 0;
	  break;
	}
    });
  vector_destroy(children);
  return satisfy;
}

static int new_modify(char* target)
{
  int ret = 0;
  struct stat cur_stat;
  if(stat(target, &cur_stat) == -1)
    {
      return 0;
    }

  vector* children = graph_neighbors(g, target);
  for(size_t i = 0; i < vector_size(children); i++)
    {
      struct stat child_stat;
      stat((char*)vector_get(children, i), &child_stat);
      if(difftime(cur_stat.st_mtime, child_stat.st_mtime) < 0)
	{
	  ret = 1;
	  break;
	}
      //if(new_modify((char*)vector_get(children, i)))
      //{
      //  ret = 1;
      //  break;
      //}
    }
  vector_destroy(children);
  return ret;
}

 static int children_not_in_disk(char* target)
 {
   //rule_t* rule = (rule_t*)graph_vertext_value(g, target);
  vector* children = graph_neighbors(g, target);
  int ret = 1;
  VECTOR_FOR_EACH(children, child, {
      if(access(child, F_OK) == -1)
	{
	  ret = 0;
	  break;
	}
    });
  vector_destroy(children);
  return ret;
 }

static void push_children(char* target)
{
  vector* children = graph_neighbors(g, target);
  if(!vector_size(children))
    {
      rule_t* rule = (rule_t*)graph_get_vertex_value(g, target);
      if(rule -> state == ZERO || rule -> state == AVAILABLE)
	{
	  rule -> state = INQ;
	  queue_push(q, target);
	  q_size++;
	}
    }
  else
    {
      VECTOR_FOR_EACH(children, child, {
	  rule_t* rule = (rule_t*)graph_get_vertex_value(g, child);
	  if(rule -> state == ZERO)
	    {
	      rule -> state = AVAILABLE;
	      push_children(child);
	    }
	});
    }
  vector_destroy(children);
}

static void insert_parent_rule(char* target)
{
  vector* parents = graph_antineighbors(g, target);

  //goal rules
  if(!vector_size(parents))
    {
      vector_destroy(parents);
      return;
    }

  VECTOR_FOR_EACH(parents, parent, {
	rule_t* rule = (rule_t*)graph_get_vertex_value(g, parent);
	if(rule -> state == AVAILABLE)
	  {
	    if(children_satisfy(parent))
	      {
		rule -> state = INQ;
		queue_push(q, parent);
	      }
	  }

	if(rule -> state == FAILED)
	  {
	    queue_push(q, NULL);
	  }
	
      });
    vector_destroy(parents);
}

static void fail_parents(char* target)
{
  vector* parents = graph_antineighbors(g, target);
  if(!vector_size(parents))
    {
      if(is_goal_rule(target) && num_task == 0)
	{
	  queue_push(q, NULL);
	}
      if(is_goal_rule(target) && num_task > 0)
	{
	  num_task--;
	}
    }
  else
    {
      VECTOR_FOR_EACH(parents, parent, {
	  rule_t* rule = (rule_t*)graph_get_vertex_value(g, parent);
	  rule -> state = FAILED;
	  fail_parents(parent);
	});
    }
  vector_destroy(parents);
}
  
//multi thread single run
static void* single_run(void* ptr)
{
  //printf("start run\n");
  (void)ptr;
  //vector* goal_rules = (vector*) ptr;
  while(1)
    {
      if(!q_size)
	{
	  break;
	}

      char* target = (char*) queue_pull(q);

      //done work
      if(!target)
	{
	  queue_push(q, NULL);
	  break;
	}

      pthread_mutex_lock(&m);
      
      rule_t* rule = (rule_t*)graph_get_vertex_value(g, target);
      //printf("target is %s\n", target);
      if(children_not_in_disk(target) && (access(target, F_OK) != -1) && !new_modify(target))
      {
	rule -> state = SATISFY;
	insert_parent_rule(target);
	//printf("push parents\n");
	pthread_mutex_unlock(&m);
	continue;
      }
      pthread_mutex_unlock(&m);


       int success = run_cmds(target);
      pthread_mutex_lock(&m);

      
      //printf("run cmds\n");
      if(success)
	{
	  //printf("success\n");
	      if(is_goal_rule(target))
		{
		  if(num_task)
		    {
		      num_task--;
		      
		    }
		  if(!num_task)
		    {
		      queue_push(q, NULL);
		    }
		}

	      if(!is_goal_rule(target))
		{
		  pthread_mutex_lock(&m2);
		  insert_parent_rule(target);
		  pthread_mutex_unlock(&m2);
		}
	}
      else
	{
	  //printf("failed\n");
	  rule -> state = FAILED;
	  if(!is_goal_rule(target))
	    {
	      pthread_mutex_lock(&m2);
	      fail_parents(target);
	      pthread_mutex_unlock(&m2);
	    }
	  if(is_goal_rule(target))
	    {
	      if(num_task)
		{
		  num_task--;
		  
		}
	      if(!num_task)
		{
		  queue_push(q, NULL);
		}
	    }
	    
	}
      pthread_mutex_unlock(&m);

    }
  //printf("done\n");
  return NULL;
}

int parmake(char *makefile, size_t num_threads, char **targets) {
    // good luck!
  //create graph
  //(void)num_threads;
  g = parser_parse_makefile(makefile, targets);
  q = queue_create(-1);
  goal_rules = graph_neighbors(g, "");
  pthread_cond_init(&cv, NULL);

  VECTOR_FOR_EACH(goal_rules, goal_rule, {
      //printf("rule is %s\n", (char*)rule);
      if(cycle_exit(g, goal_rule, NULL))
	{
	  rule_t* rule = (rule_t *)graph_get_vertex_value(g, goal_rule);
	  rule -> state = CYCLE;
	  print_cycle_failure(goal_rule);
	}
      else
	{
	  rule_t* rule = (rule_t *)graph_get_vertex_value(g, goal_rule);
	  rule -> state = AVAILABLE;
	  num_task++;
	}
    });
  
  vector* vertices = graph_vertices(g);

  VECTOR_FOR_EACH(vertices, vertice, {
      //not empty rule
      rule_t* rule = (rule_t *)graph_get_vertex_value(g, vertice);
      if(rule -> state != CYCLE)
	{
	  
	  rule -> state = ZERO;
	}
    });
  vector_destroy(vertices);

  push_children("");
  
  pthread_t threads[num_threads];

  for(size_t i = 0; i < num_threads; i++)
    {
      pthread_create(&threads[i], NULL, single_run, (void*)goal_rules);
    }

  //printf("wait!\n");
  for(size_t i = 0; i < num_threads; i++)
    {
      void* result;
      pthread_join(threads[i], &result);
    }

  //printf("done\n");
  //destroy
  vector_destroy(goal_rules);
  graph_destroy(g);
  queue_destroy(q);
  pthread_mutex_destroy(&m);
  pthread_mutex_destroy(&m2);
  pthread_cond_destroy(&cv);
  return 0;
}

/*
static void single_run(char* target)
{
  //printf("start exe\n");
  rule_t* rule = (rule_t*)graph_get_vertex_value(g, target);

  if(rule -> state == SATISFY)
    {
      return;
    }
  
  vector* children = graph_neighbors(g, target);
  //no dependency
  if(!vector_size(children))
    {
      //not a file on disk
      vector_destroy(children);
      
      if(access(target, F_OK) == -1)
	{
	  int success = run_cmds(target);
	  if(!success)
	    {
	      rule -> state = FAILED;
	      return;
	    }
	  else
	    {
	      mark_satisfy(target);
	      return;
	    }
	}
    }
  else
    {
      vector_destroy(children);
      
      //rule depend on rule that is not a file on disk
      
      if(!children_satisfy(target))
	{
	  //printf("child not satisfy\n");
	  vector* children = graph_neighbors(g, target);
	  VECTOR_FOR_EACH(children, child, {
	      //printf("%s is run\n", (char*)child);
	      single_run(child);
	    });
	  vector_destroy(children);
	}

      if(child_failed(target))
	{
	  //printf("child failed\n");
	  rule -> state = FAILED;
	  //vector_destroy(children);
	  return;
	}
      
      //rule not on disk
      if(access(target, F_OK) == -1)
	{
	  //printf("current target not on disk\n");
	  int success = run_cmds(target);
	  if(!success)
	    {
	      rule -> state = FAILED;
	      return;
	    }
	  else
	    {
	      mark_satisfy(target);
	      return;
	    }
	}

      //children not on disk
      if(children_not_in_disk(target))
	{
	  //printf("children not on disk\n");
	  int success = run_cmds(target);
	  if(!success)
	    {
	      rule -> state = FAILED;
	      return;
	    }
	  else
	    {
	      mark_satisfy(target);
	      return;
	    }
	}

      
      if((access(target, F_OK) != -1) && new_modify(target))
	{
	  //printf("child is modified\n");
	  int success = run_cmds(target);
	  if(!success)
	    {
	      rule -> state = FAILED;
	      return;
	    }
	  else
	    {
	      mark_satisfy(target);
	      return;
	    }
	}
      //printf("final mark s\n");
      mark_satisfy(target);
    }
}
*/
