
/**
 * pugnacious_profilers Lab
 * CS 241 - Fall 2018
 */

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "orderbook.h"

#define STDOUT 1

//#define SIZE 50459
#define SIZE 1000000

static OrderNode* array[SIZE];
static OrderNode* d_node;

/****Main Implementation****/

int main(int argc, char *argv[]) {
<<<<<<< HEAD
  InputType in_type = ASCII;
  StructType struct_type = LL;

  FILE *input = stdin;
  int outputFD = STDOUT;

  //init d_node
  d_node = (OrderNode*)malloc(sizeof(OrderNode));
  d_node -> id = -1;


  // Process the commandline
  proc_cmdline(argc, argv, &in_type, &struct_type, &input, &outputFD);
  //printf("finish cmd line\n");
  if (struct_type == LL) { // linked list
    //OrderNode *order_book = NULL;
    // process input
    //proc_mesgs_LL(in_type, &order_book, input);
    proc_mesgs_LL(in_type, input);
    //printf("finish msg\n");
    // print the order_book
    // print_orderbook_LL(order_book, outputFD);
    print_orderbook_LL(outputFD);
    //printf("finish print ob\n");
    close(outputFD);
    // free data
    fclose(input);

    free_LL();
    //printf("finish free\n");
  } else {
    // your code here
  }

  free(d_node);
  return 0;
=======
    InputType in_type = ASCII;
    StructType struct_type = LL;

    FILE *input = stdin;
    int outputFD = STDOUT;

    //init d_node
    d_node = (OrderNode*)malloc(sizeof(OrderNode));
    d_node -> id = -1;


    // Process the commandline
    proc_cmdline(argc, argv, &in_type, &struct_type, &input, &outputFD);
    //printf("finish cmd line\n");
    if (struct_type == LL) { // linked list
      //OrderNode *order_book = NULL;
        // process input
        //proc_mesgs_LL(in_type, &order_book, input);
	  proc_mesgs_LL(in_type, input);
	//printf("finish msg\n");
        // print the order_book
	  // print_orderbook_LL(order_book, outputFD);
	print_orderbook_LL(outputFD);
	//printf("finish print ob\n");
        close(outputFD);
        // free data
        fclose(input);

        free_LL();
	//printf("finish free\n");
    } else {
        // your code here
    }

    free(d_node);
    return 0;
>>>>>>> 120e854cdad6e4a1cf3d636b7f354f69ea5d37fb
}

/****Start Function implementations****/

/*
 * proc_scmdline - Processes the command line
 * argc - number of arguments
 * argv - arguments passed to the program command line
 * i_t - capture the input type
 * s_t - structure type: linked list
 * in - input file
 * out - output file
 */

void proc_cmdline(int argc, char *argv[], InputType *i_t, StructType *s_t,
		  FILE **in, int *out) {
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-h")) {
      // your code here
    } else if (!strcmp(argv[i], "-i")) {
      i++;
      if ((*in = fopen(argv[i], "r")) == NULL) {
	fprintf(stderr, "Unabled to open input file named %s\n",
		argv[i]);
	exit(0);
      }
    } else if (!strcmp(argv[i], "-o")) {
      i++;

      if ((*out = open(argv[i], O_WRONLY | O_CREAT | O_TRUNC, 0777)) <
	  0) {
	fprintf(stderr, "Unable to open output file named %s\n",
		argv[i]);
	exit(0);
      }
    } else {
      fprintf(stderr, "Unexpected input flag %s\n", argv[i]);
    }
  }
}

/*
 * proc_mesgs_LL - Process a file into a linked list
 * in_type - Type of input (ASCII)
 * head - head of the linked list
 * in - inputfile
 */
void proc_mesgs_LL(InputType in_type,  FILE *in) {
<<<<<<< HEAD
  char msg_type; // char to hold the operation type
  char side;     // sell Vs Buy
  OrderNode in_data;

  if (in_type == ASCII) {
    while (fscanf(in, "%c", &msg_type) != EOF) { // While not EOF
      // process data
      switch (msg_type) {
      case 'A':
	fscanf(in, "%ld %c %s %ld %lf\n", &(in_data.id), &side, in_data.symbol,
	       &(in_data.quantity), &(in_data.price));
	(side == 'B') ? (in_data.side = BUY) : (in_data.side = SELL);
	//fprintf(stderr, "ADD!\n");
	add_order_LL(&in_data);

	break;
      case 'X':
	fscanf(in, "%ld %s\n", &(in_data.id), in_data.symbol);
	//fprintf(stderr, "Cancel!\n");
	cancel_order_LL(&in_data);

	break;
      case 'T': // Same as Case 'T'
      case 'C':
	fscanf(in, "%ld %s %ld\n", &(in_data.id), in_data.symbol, &(in_data.quantity));
	//fprintf(stderr, "Reduce!\n");
	reduce_quantity_LL(&in_data);

	break;
      case 'R':
	fscanf(in, "%ld %s %ld %lf\n", &(in_data.id), in_data.symbol,
	       &(in_data.quantity), &(in_data.price));
	//fprintf(stderr, "Change order!\n");
	change_order_LL(&in_data);

	break;
      default: // If we reach here, then we got wrong input
	// fprintf(stderr, "Unknown message type, %c, ignoring
	// message\n", msg_type);
	//fprintf(stderr, "invalid\n");
	break;
      }
=======
    char msg_type; // char to hold the operation type
    char side;     // sell Vs Buy
    OrderNode in_data;

    if (in_type == ASCII) {
        while (fscanf(in, "%c", &msg_type) != EOF) { // While not EOF
            // process data
            switch (msg_type) {
            case 'A':
	      fscanf(in, "%ld %c %s %ld %lf\n", &(in_data.id), &side, in_data.symbol, 
		     &(in_data.quantity), &(in_data.price));
	      (side == 'B') ? (in_data.side = BUY) : (in_data.side = SELL);
		//fprintf(stderr, "ADD!\n");
                add_order_LL(&in_data);
		
                break;
            case 'X':
	      fscanf(in, "%ld %s\n", &(in_data.id), in_data.symbol);
		//fprintf(stderr, "Cancel!\n");
	      cancel_order_LL(&in_data);
		
                break;
            case 'T': // Same as Case 'T'
            case 'C':
	      fscanf(in, "%ld %s %ld\n", &(in_data.id), in_data.symbol, &(in_data.quantity));
		//fprintf(stderr, "Reduce!\n");
	      reduce_quantity_LL(&in_data);
		
                break;
            case 'R':
	      fscanf(in, "%ld %s %ld %lf\n", &(in_data.id), in_data.symbol, 
		     &(in_data.quantity), &(in_data.price));
		//fprintf(stderr, "Change order!\n");
	      change_order_LL(&in_data);
		
                break;
            default: // If we reach here, then we got wrong input
                // fprintf(stderr, "Unknown message type, %c, ignoring
                // message\n", msg_type);
	      //fprintf(stderr, "invalid\n");
                break;
            }
        }
>>>>>>> 120e854cdad6e4a1cf3d636b7f354f69ea5d37fb
    }
  }
}

/*
//BST insert
OrderNode* add_order(OrderNode *head, OrderNode *data) {
  
  if (head == NULL) { // Empty list
        head = (OrderNode *)malloc(sizeof(OrderNode));
        head->id = data->id;
        head->side = data->side;
        head->quantity = data->quantity;
        head->price = data->price;
        head->next = NULL;
        head->prev = NULL;
	//printf("add null head\n");
	return head;
    }
  if(data -> id < head -> id)
    {
      head -> prev = add_order(head -> prev, data);
      //printf("add left\n");
    }
  else if(data -> id > head -> id)
    {
      head -> next = add_order(head -> next, data);
      //printf("add right\n");
    }
  return head;
}
*/

int hashCode(int id)
{
  return id % SIZE;
}
<<<<<<< HEAD

OrderNode* search(OrderNode *data)
{
  int hashIndex = hashCode(data -> id);

  while(array[hashIndex] != NULL)
    {
      if(array[hashIndex] -> id == data -> id)
	{
	  return array[hashIndex];
	}
      ++hashIndex;
      hashIndex %= SIZE;
    }
  return NULL;
}

//hash table insert
/*
void insert(OrderNode *data)
{
  OrderNode* d = (OrderNode *)malloc(sizeof(OrderNode));
  d->id = data->id;
  d->side = data->side;
  d->quantity = data->quantity;
  d->price = data->price;
  int hashIndex = hashCode(data -> id);
  while(array[hashIndex] != NULL && array[hashIndex] -> id != -1)
    {
      ++hashIndex;
      hashIndex %= SIZE;
    }
  array[hashIndex] = d;
  return;
}
void delete(OrderNode *data)
{
  int hashIndex = hashCode(data -> id);
  while(array[hashIndex] != NULL)
    {
      if(array[hashIndex] -> id == data -> id)
      {
        free(array[hashIndex]);
	  array[hashIndex] = d_node;
	    break;
	      //return t;
	      }
=======

OrderNode* search(OrderNode *data)
{
  int hashIndex = hashCode(data -> id);
  
  while(array[hashIndex] != NULL)
    {
      if(array[hashIndex] -> id == data -> id)
	{
	  return array[hashIndex];
	}
      ++hashIndex;
      hashIndex %= SIZE;
    }
  return NULL;
}

//hash table insert
/*
void insert(OrderNode *data)
{
  OrderNode* d = (OrderNode *)malloc(sizeof(OrderNode));
  d->id = data->id;
  d->side = data->side;
  d->quantity = data->quantity;
  d->price = data->price;

  int hashIndex = hashCode(data -> id);

  while(array[hashIndex] != NULL && array[hashIndex] -> id != -1)
    {
      ++hashIndex;
      hashIndex %= SIZE;
    }
  array[hashIndex] = d;
  return;
}

void delete(OrderNode *data)
{
  int hashIndex = hashCode(data -> id);

  while(array[hashIndex] != NULL)
    {
      if(array[hashIndex] -> id == data -> id)
	{
	  free(array[hashIndex]);
	  array[hashIndex] = d_node;
	  break;
	  //return t;
	}

>>>>>>> 120e854cdad6e4a1cf3d636b7f354f69ea5d37fb
      ++hashIndex;
      hashIndex %= SIZE;
    }
  return;
}
*/
/****Start Linked List implementation****/

/*
 * add_order_LL - add order to linked list
 * head - head of the linked list
 * data - node data
 */

void add_order_LL(OrderNode *data) {
  /*
  if (*head == NULL) { // Empty list
  *head = (OrderNode *)malloc(sizeof(OrderNode));
        (*head)->id = data->id;
        (*head)->side = data->side;
        (*head)->quantity = data->quantity;
        (*head)->price = data->price;
        (*head)->next = NULL;
        (*head)->prev = NULL;
    } else { // Non Empty list
        OrderNode *tmp;
        tmp = (OrderNode *)malloc(sizeof(OrderNode));
        tmp->id = data->id;
        tmp->side = data->side;
        tmp->quantity = data->quantity;
        tmp->price = data->price;
        tmp->next = *head;
        tmp->prev = NULL;
        (*head)->prev = tmp;
        *head = tmp;
    }
  */
  //insert(data);
<<<<<<< HEAD
  OrderNode* d = (OrderNode *)malloc(sizeof(OrderNode));
=======
OrderNode* d = (OrderNode *)malloc(sizeof(OrderNode));
>>>>>>> 120e854cdad6e4a1cf3d636b7f354f69ea5d37fb
  d->id = data->id;
  d->side = data->side;
  d->quantity = data->quantity;
  d->price = data->price;

  int hashIndex = hashCode(data -> id);

  while(array[hashIndex] != NULL && array[hashIndex] -> id != -1)
    {
      ++hashIndex;
      hashIndex %= SIZE;
    }
  array[hashIndex] = d;
  return;
  //*head = add_order(*head, data);
  //head = &t;
}

/*
OrderNode* search(OrderNode *head, OrderNode *data)
{
  if(head == NULL || head -> id == data-> id)
    return head;
  if(head -> id < data -> id)
    return search(head->next, data);
  return search(head -> prev, data);
}
OrderNode* find_min(OrderNode* node)
{
  OrderNode* cur = node;
  while(cur -> prev != NULL)
    cur = cur -> prev;
  return cur;
}
OrderNode* delete_order(OrderNode* head, OrderNode *data)
{
  if(head == NULL)
    return head;
  if(data -> id < head -> id)
    head -> prev = delete_order(head -> prev, data);
  else if (data -> id > head -> id)
    head -> next = delete_order(head -> next, data);
  else
    {
      if (head -> prev == NULL)
      {
        OrderNode* t = head -> next;
	  free(head);
	    return t;
	    }
      else if(head -> next == NULL)
      {
        OrderNode* t = head -> prev;
	  free(head);
	    return t;
	    }
      OrderNode* t = find_min(head -> next);
      //copy data
      head -> id = t -> id;
      head -> side = t -> side;
      head -> quantity = t -> quantity;
      head -> price = t -> price;
      //head -> next = NULL;
      //head -> prev = NULL;
      
      head -> next = delete_order(head -> next, t);
    }
  return head;
}
*/

/*
 * cancel_order_LL - cancel order from linked list
 * head - head of the linked list
 * data - node data
 */
void cancel_order_LL(OrderNode *data) {
  /*
  OrderNode *cur = *head;
    // find the location of the node to cancel
    while (cur != NULL && (cur->id != data->id))
        cur = cur->next;
    // delete node after finding its location
    if (cur != NULL) {
        if (cur == *head) { // curr is head, we should assign a new head
	*head = cur->next;
            (*head)->prev = NULL;
        } else { // cur isn't head node
            cur->prev->next = cur->next;
	    if(cur->next != NULL)
	    cur->next->prev = cur->prev;
        }
        // free the memory location
        free(cur);
    
    }
  */
<<<<<<< HEAD
  int hashIndex = hashCode(data -> id);
=======
    int hashIndex = hashCode(data -> id);
>>>>>>> 120e854cdad6e4a1cf3d636b7f354f69ea5d37fb

  while(array[hashIndex] != NULL)
    {
      if(array[hashIndex] -> id == data -> id)
	{
	  free(array[hashIndex]);
	  array[hashIndex] = d_node;
	  break;
	  //return t;
	}
<<<<<<< HEAD
=======

      ++hashIndex;
      hashIndex %= SIZE;
    }
  return;
  //delete(data);
  // *head = 
  //*head = delete_order(*head, data);
>>>>>>> 120e854cdad6e4a1cf3d636b7f354f69ea5d37fb

      ++hashIndex;
      hashIndex %= SIZE;
    }
  return;
  //delete(data);
  // *head =
  //*head = delete_order(*head, data);

  //////NOT DONE!!!!11!!!1
}

/*
 * reduce_quantity_LL - reduce quantity in a linked list
 * head - head of the linked list
 * data - node data
 */
void reduce_quantity_LL(OrderNode *data) {
  /*
  OrderNode *cur = head;
    // find the location of the node to be updated
    while (cur != NULL && (cur->id != data->id))
        cur = cur->next;
    // reduce quantity
    if (cur != NULL)
        cur->quantity = cur->quantity - data->quantity;
  */
<<<<<<< HEAD

  OrderNode* cur = search(data);
  // reduce quantity
  if (cur != NULL)
    cur->quantity = cur->quantity - data->quantity;

=======
  
  OrderNode* cur = search(data);
  // reduce quantity
    if (cur != NULL)
        cur->quantity = cur->quantity - data->quantity;
  
>>>>>>> 120e854cdad6e4a1cf3d636b7f354f69ea5d37fb
}

/*
 * change_order_LL - change the date of an order
 * head - head of the linked list
 * data - node data
 */
void change_order_LL(OrderNode *data) {
  /*  
  OrderNode *cur = head;
    // find the location of the node to cancel
    while (cur != NULL && (cur->id != data->id))
        cur = cur->next;
    // change order
    if (cur != NULL) {
        cur->quantity = data->quantity;
        cur->price = data->price;
    }
  */
<<<<<<< HEAD

  OrderNode* cur = search(data);
  // change order
  if (cur != NULL) {
    cur->quantity = data->quantity;
    cur->price = data->price;
  }

=======
    
  OrderNode* cur = search(data);
    // change order
    if (cur != NULL) {
        cur->quantity = data->quantity;
        cur->price = data->price;
    }
      
>>>>>>> 120e854cdad6e4a1cf3d636b7f354f69ea5d37fb
}

/*
 * print_orderbook_LL - Print the order book
 * head - head of the linked list
 * out - outputfile
 */
void print_orderbook_LL(int out) {
  /*
  OrderNode *cur = head;
    char line[50];
    while (cur != NULL) { // loop on the list starting from head
        sprintf(line, "%ld %c %ld %lf\n", cur->id,
                (cur->side == BUY) ? 'B' : 'S', cur->quantity, cur->price);
        write(out, line, strlen(line));
        cur = cur->next;
    }
  */
  /*
  //printf("start print ob\n");
  OrderNode *cur = head;
  if(cur != NULL)
    {
      //printf("head is not NULL\n");
      //in order print
      print_orderbook_LL(cur -> next, out);
      char line[50];
      sprintf(line, "%ld %c %ld %lf\n", cur->id,
                (cur->side == BUY) ? 'B' : 'S', cur->quantity, cur->price);
      write(out, line, strlen(line));
      //printf("print node\n");
      print_orderbook_LL(cur -> prev, out);
    }
<<<<<<< HEAD
  */
=======
    */
>>>>>>> 120e854cdad6e4a1cf3d636b7f354f69ea5d37fb
  for(int i = 0; i < SIZE; i++)
    {
      if(array[i] != NULL && array[i] -> id != -1)
	{
	  OrderNode* cur = array[i];
	  char line[50];
<<<<<<< HEAD
	  sprintf(line, "%ld %c %ld %lf\n", cur->id,
		  (cur->side == BUY) ? 'B' : 'S', cur->quantity, cur->price);
	  write(out, line, strlen(line));
=======
      sprintf(line, "%ld %c %ld %lf\n", cur->id,
                (cur->side == BUY) ? 'B' : 'S', cur->quantity, cur->price);
      write(out, line, strlen(line));
>>>>>>> 120e854cdad6e4a1cf3d636b7f354f69ea5d37fb

	}
    }
}

/*
void free_tree(OrderNode *head)
{
  if(head == NULL)
    return;
  free_tree(head -> prev);
  free_tree(head -> next);
  free(head);
}
*/

/*
 * free_LL - free the entire list
 * head - head of the linked list
 */
void free_LL() {
  /*
  OrderNode *last, *cur = *head;
    while (cur != NULL) { // loop on the list starting from head
        last = cur;
        cur = cur->next;
        // free the node
        free(last);
    }
  */
<<<<<<< HEAD
  //free_tree(*head);
=======
    //free_tree(*head);
>>>>>>> 120e854cdad6e4a1cf3d636b7f354f69ea5d37fb
  for(int i = 0; i < SIZE; i++)
    {
      if(array[i] != NULL && array[i] -> id != -1)
	free(array[i]);
    }
}

// End Linked List Implementation
