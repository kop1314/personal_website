/**
* Critical Concurrency Lab
* CS 241 - Fall 2018
*/

#include "queue.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * This queue is implemented with a linked list of queue_nodes.
 */
typedef struct queue_node {
    void *data;
    struct queue_node *next;
} queue_node;

struct queue {
    /* queue_node pointers to the head and tail of the queue */
    queue_node *head, *tail;

    /* The number of elements in the queue */
    ssize_t size;

    /**
     * The maximum number of elements the queue can hold.
     * max_size is non-positive if the queue does not have a max size.
     */
    ssize_t max_size;

    /* Mutex and Condition Variable for thread-safety */
    pthread_cond_t cv;
    pthread_mutex_t m;
};

queue *queue_create(ssize_t max_size) {
    /* Your code here */
  queue* q = malloc(sizeof(queue));
  q -> head = NULL;
  q -> tail = NULL;
  q -> size = 0;
  q -> max_size = max_size;
  pthread_mutex_init(&q -> m, NULL);
  pthread_cond_init(&q -> cv, NULL);
  return q;
}

void queue_destroy(queue *this) {
    /* Your code here */
  queue_node* cur = this->head;
  while(cur != NULL)
    {
      queue_node* temp = cur;
      cur = cur -> next;
      free(temp);
    }
  pthread_mutex_destroy(&this -> m);
  pthread_cond_destroy(&this -> cv);
  free(this);
}

void queue_push(queue *this, void *data) {
    /* Your code here */
  pthread_mutex_lock(&this -> m);
  queue_node* node = malloc(sizeof(queue_node));
  node -> data = data;
  node -> next = NULL;
  while(this->size == this->max_size && this -> max_size > 0)
    {
      pthread_cond_wait(&this ->cv, &this -> m);
    }
  if(this -> size == 0)
    {
      this->head = node;
      this->tail = node;
    }
  else
    {
      this -> tail -> next = node;
      this -> tail = node;
    }
  this -> size++;
  if(this -> size == 1)
    {
      pthread_cond_broadcast(&this -> cv);
    }
  pthread_mutex_unlock(&this->m);
}

void *queue_pull(queue *this) {
    /* Your code here */
  pthread_mutex_lock(&this -> m);
  while(this -> size == 0)
    {
      pthread_cond_wait(&this -> cv, &this -> m);
    }
  void* ret = this -> head -> data;
  queue_node* temp = this -> head;
  this -> head = this -> head -> next;
  free(temp);
  this -> size--;
  if(this -> size == 0)
    {
      this -> tail = NULL;
    }
  if(this -> size == this -> max_size - 1)
    {
      pthread_cond_broadcast(&this -> cv);
    }
  pthread_mutex_unlock(&this -> m);
    return ret;
}
