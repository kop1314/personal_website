/**
* Networking Lab
* CS 241 - Fall 2018
*/

#include "common.h"
#include "format.h"
#include "vector.h"


#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>

#define BUFFER_SIZE (1024)
//#define GB (4 * 1024 * 1024 * 1024)
static int socket_id;

char **parse_args(int argc, char **argv);
verb check_args(char **args);


int static connection(char* host, char* port)
{
  int s;
  socket_id = socket(AF_INET, SOCK_STREAM, 0);
  struct addrinfo hints, *result;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  s = getaddrinfo(host, port, &hints, &result);
  if(s != 0)
    {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
      freeaddrinfo(result);
      exit(1);
      
    }
  if(connect(socket_id, result -> ai_addr, result -> ai_addrlen) == -1)
    {
      perror("connect");
      freeaddrinfo(result);
      exit(1);
    }
  
  freeaddrinfo(result);
  return socket_id;
}

static void c_get(char* remote, char* local)
{
  char* buffer = calloc(BUFFER_SIZE + 1, sizeof(char));

  strcpy(buffer, "GET ");
  strcat(buffer, remote);
  strcat(buffer, "\n");

  write(socket_id, buffer, strlen(buffer));
  shutdown(socket_id, SHUT_WR);

  memset(buffer, 0, BUFFER_SIZE);
  if(!read(socket_id, buffer, 3))
    {
      return;
    }
  //printf("%saa", buffer);
  //read(socket_id, buffer, 20);
  //printf("%s\n", buffer);
  
  if(strcmp(buffer, "OK\n") == 0)
    {
      FILE* fp;
      size_t read_byte = read(socket_id, buffer, 8);
      if(!read_byte)
	{
	  return;
	}
      /*
      else if(read_byte == 8)
	{
	  //printf("what\n");
	  print_too_little_data();
	  return;
	}
      */
      else
	{
	  fp = fopen(local, "w+");
	  if(!fp)
	    {
	      fprintf(stderr, "GET: failed to create local file\n");
	      free(buffer);
	      return;
	    }
	  
	  //write(fd, buffer + 8, 2);
	}
      vector* v = string_vector_create();
      size_t fd_size = *(size_t *)buffer;
      size_t read_size = 0;
      while(read_size < fd_size)
	{
	  //char* line = calloc(BUFFER_SIZE, sizeof(char));
	  char line[BUFFER_SIZE + 1];
	  memset(line, 0, BUFFER_SIZE + 1);
	  int sub_read_size = read(socket_id, line, BUFFER_SIZE);
	  if(sub_read_size == -1)
	    {
	      fprintf(stderr, "Read line failed\n");
	      //free(line);
	      free(buffer);
	      fclose(fp);
	      vector_destroy(v);
	      return;
	    }

	  read_size += sub_read_size;
	  if(!sub_read_size)
	    {
	      //free(line);
	      fprintf(stderr, "GET: line length is 0\n");
	      //free(line);
	      break;
	    }
	  else
	    {
	      vector_push_back(v, line);
	      //if(sub_read_size < BUFFER_SIZE)
	      //{
	      //  break;
	      //}
	    }
	 
	}
      //printf("hi\n");
      //read_size --;
      //fprintf(stderr, "file size: %zu\n", fd_size);
      //fprintf(stderr, "read byte: %zu\n", read_size); 
      if(read_size > fd_size)
	{
	  print_received_too_much_data();
	  free(buffer);
	  fclose(fp);
	  vector_destroy(v);
	  return;
	}
      else if(read_size < fd_size)
	{
	  print_too_little_data();
	  free(buffer);
	  fclose(fp);
	  vector_destroy(v);
	  return;
	}
      else
	{
	  printf("OK\n");
	  printf("file size is %zu\n", fd_size);
	  printf("read %zu bytes\n", read_size);
	  VECTOR_FOR_EACH(v, s_v, {

	      fprintf(fp, "%s", (char*)s_v);
	    });

	  free(buffer);
	  fclose(fp);
	  vector_destroy(v);
	  return;
	}
    }
  else if(strcmp(buffer, "ERR") == 0)
    {
      //printf("buffer is %s\n", buffer);
      
      read(socket_id, buffer, 3);
      //printf("buffer is %s\n", buffer);
      if(strcmp(buffer, "OR\n") == 0)
	{
	  size_t size_read = read(socket_id, buffer,50);
	  buffer[size_read - 1] = '\0';
	  printf("ERROR\n");
	  print_error_message(buffer);
	  //free(buffer);
	}
      else
	{
	  // printf("hi\n");
	  print_invalid_response();
	}
    }
  else
    {
      print_invalid_response();
      //free(buffer);
    }
  free(buffer);
}

static void c_put(char* remote, char* local)
{
  //printf("file name is %s\n", local);
  FILE* fd_ptr = fopen(local, "r");
  if(!fd_ptr)
    {
      printf("Failed to open file\n");
      return;
    }

  fseek(fd_ptr, 0L, SEEK_END);
  int file_size = ftell(fd_ptr);
  fclose(fd_ptr);

  //printf("start to open file\n");
  int fd = open(local, O_RDONLY);

  char* buffer = calloc(BUFFER_SIZE + 1, sizeof(char));
  strcpy(buffer, "PUT ");
  strcat(buffer, remote);
  strcat(buffer, "\n");
  //printf("buffer is %s\n", buffer);
  write(socket_id, buffer, strlen(buffer));
  //printf("%s\n", buffer);
  size_t fz = (size_t)file_size;
  write(socket_id, (void*)(&fz), 8);

  //printf("start loop\n");
  int total_bytes_read = 0;
  while(total_bytes_read < file_size)
    {
      memset(buffer, 0, BUFFER_SIZE + 1);
      int bytes_read = read(fd, buffer, BUFFER_SIZE);
      if(!bytes_read)
	{
	  //no byte to be read
	  fprintf(stderr, "no byte to be read from local file\n");
	  return;
	}
      else if(bytes_read == -1)
	{
	  fprintf(stderr, "connection closed\n");
	  write(socket_id, buffer, bytes_read);
	  print_connection_closed();
	  return;
	}
      else
	{
	  //printf("write to server\n");
	  //printf("buffer is %s\n", buffer);
	  write(socket_id, buffer, bytes_read);
	  total_bytes_read += bytes_read;
	}
    }
  
  //printf("clean up\n");
  close(fd);

  memset(buffer, 0, BUFFER_SIZE + 1);
  shutdown(socket_id, SHUT_WR);
  if(!read(socket_id, buffer, 3))
    {
      fprintf(stderr, "No byted to be read\n");
      return;
    }

  if(strcmp(buffer, "OK\n") == 0)
    {
      printf("OK\n");
      print_success();
    }
  else if(strcmp(buffer, "ERR") == 0)
    {
      memset(buffer, 0, BUFFER_SIZE + 1);
      read(socket_id, buffer,3);
      if(strcmp(buffer, "OR\n") == 0)
	{
	  //read(socket_id, buffer, 1);
	  size_t read_size = read(socket_id, buffer,50);
	  //fprintf(stderr, "read_size is %zu\n", read_size);
	  buffer[read_size - 1] = 0;
	  printf("ERROR\n");
	  print_error_message(strtok(buffer, "\n"));
	}
      else
	{
	  print_invalid_response();
	}
    }
  else
    {
      print_invalid_response();
    }
  //shutdown(socket_id, SHUT_RDWR);
  //print_success();
  free(buffer);
}

static void c_list()
{
  char* buffer = calloc(BUFFER_SIZE, sizeof(char));
  dprintf(socket_id, "LIST\n");

  //write(socket_id, buffer, strlen(buffer));
  shutdown(socket_id, SHUT_WR);

  read(socket_id, buffer, 3);

  if(strcmp(buffer, "OK\n")==0)
    {
      //memset(buffer,0,BUFFER_SIZE);
      char* size = malloc(8);
      //fprintf(stderr, "size is %zu\n", *(size_t*)size);
      read(socket_id, size, 8);
      fprintf(stderr, "size is %zu\n", *(size_t*)size);

      vector* v = string_vector_create();
      
      size_t read_size = 0;
      size_t fd_size = *(size_t*)size;
      free(size);
      //fprintf(stdout, "OK\n");
      while(read_size < fd_size)
	{
	  char* msg = calloc(BUFFER_SIZE, sizeof(char));
	  
	  int sub_read_size = read(socket_id, msg, BUFFER_SIZE);
	  if(sub_read_size == -1)
	    {
	      fprintf(stderr, "LIST: Read Line Failed"); 
	      free(buffer);
	      free(msg);
	      vector_destroy(v);
	      return;
	      
	    }
	  read_size += sub_read_size;
	  if(!sub_read_size)
	    {
	      free(msg);
	      break;
	    }
	  else
	    {
	      
	      vector_push_back(v, msg);
	    }

	}
      //read(socket_id, buffer, *size);
      if(read_size > fd_size)
	{
	  print_received_too_much_data();
	  vector_destroy(v);
	  free(buffer);
	  return;
	}
      else if(read_size < fd_size)
	{
	  print_too_little_data();
	  free(buffer);
	  vector_destroy(v);
	  return;
	}
      else
	{
	  fprintf(stdout, "OK\n");
	  VECTOR_FOR_EACH(v, s_v, {
	      fprintf(stdout, "%s", (char*)s_v);
	      
	    });
	  free(buffer);
	  vector_destroy(v);
	  return;
	}


      //fprintf(stdout, "OK\n");
      //fprintf(stdout, "%s", buffer);
      //free(size);
      //read(socket_id, buffer, *(size_t*)size);
      //fprintf(stdout, "LIST\n");
      //fprintf(stdout, "OK\n");
      //fprintf(stdout, "%s\n", buffer);
      
    }
  else if(strcmp(buffer, "ERR") == 0) 
    {
      memset(buffer, 0, BUFFER_SIZE);
      read(socket_id, buffer, 3);
      //fprintf(stderr, "buffer is %s\n", buffer);
      if(strcmp(buffer, "OR\n") == 0)
	{
	  //char* begin = buffer;
	  memset(buffer, 0, BUFFER_SIZE);
	  size_t i = 0;
	  while(read(socket_id, buffer+i, 1) > 0)
	    {
	      i++;
	    } 
	  //printf("after loop\n");
	  //read_all_from_socket(sock_id,buffer,);
	  //fprintf(stderr, "buffer os %s\n end",buffer);
	  char* tmp = strtok(buffer,"\n");
	  //tmp = strtok(NULL, "\n");
	  fprintf(stderr, "ERROR\n");
	  print_error_message(tmp);
	}
	else
	  {
	    print_invalid_response();
	  }
      /*
      char* tmp = strtok(buffer, "\n");
      tmp = strtok(NULL, "\n");
      print_error_message(tmp);
      */
    }
  else
    {
      print_invalid_response();
    }
 
  free(buffer);
  
}

static void c_delete(char* remote)
{
  char* buffer = calloc(BUFFER_SIZE, sizeof(char));
  strcpy(buffer, "DELETE ");
  strcat(buffer, remote);
  strcat(buffer, "\n");
  //fprintf(stdout, "buffer is %s", buffer);
  write(socket_id, buffer, strlen(buffer));
  shutdown(socket_id, SHUT_WR);

  //get message
  memset(buffer, 0, BUFFER_SIZE);

  
  read(socket_id, buffer, 3);
  //printf("buffer is %s\n", buffer);
  if(strcmp(buffer, "OK\n") == 0)
    {
      fprintf(stdout, "OK\n");
      print_success();
    }
  else if(strcmp(buffer, "ERR") == 0)
    {
      //fprintf(stdout, "error\n");
      /*
      char* begin = buffer;
      size_t i = 0;
      while(read(socket_id, begin+i, 1) > 0){
	i++;
      }
      */
      read(socket_id, buffer, 3);
      if(strcmp(buffer, "OR\n") == 0)
	{
	  read(socket_id, buffer, 50);
	  char* tmp = strtok(buffer, "\n");
      
	  //tmp = strtok(NULL, "\n");
	  //print_error_message(tmp);
	  //tmp = strtok(NULL, "\n");
	  printf("ERROR\n");
	  print_error_message(tmp);
	  //printf("%s", err_no_such_file);
	}
      else
	{
	  //printf("hi\n");
	  print_invalid_response();
	}
    }
  else
    {
      print_invalid_response();
    }

  free(buffer);
}

int main(int argc, char **argv) {
    // Good luck!
  char** args = parse_args(argc, argv);
  verb v = check_args(args);

  if(!args)
    {
      print_client_usage();
      return 0;
    }

  socket_id = connection(args[0], args[1]);
  if(socket_id == -1)
    {
      print_connection_closed();
      free(args);
      exit(1);
    }

  switch(v)
    {
    case GET:
      c_get(args[3], args[4]);
      break;
    case PUT:
      c_put(args[3], args[4]);
      break;
    case LIST:
      c_list();
      break;
    case DELETE:
      c_delete(args[3]);
      break;
    default:
      printf("This line should never be printed\n");
    }
  free(args);
  shutdown(socket_id, SHUT_RDWR);
  close(socket_id);
  //free(args);
}

/**
 * Given commandline argc and argv, parses argv.
 *
 * argc argc from main()
 * argv argv from main()
 *
 * Returns char* array in form of {host, port, method, remote, local, NULL}
 * where `method` is ALL CAPS
 */
char **parse_args(int argc, char **argv) {
    if (argc < 3) {
        return NULL;
    }

    char *host = strtok(argv[1], ":");
    char *port = strtok(NULL, ":");
    if (port == NULL) {
        return NULL;
    }

    char **args = calloc(1, 6 * sizeof(char *));
    args[0] = host;
    args[1] = port;
    args[2] = argv[2];
    char *temp = args[2];
    while (*temp) {
        *temp = toupper((unsigned char)*temp);
        temp++;
    }
    if (argc > 3) {
        args[3] = argv[3];
    }
    if (argc > 4) {
        args[4] = argv[4];
    }

    return args;
}

/**
 * Validates args to program.  If `args` are not valid, help information for the
 * program is printed.
 *
 * args     arguments to parse
 *
 * Returns a verb which corresponds to the request method
 */
verb check_args(char **args) {
    if (args == NULL) {
        print_client_usage();
        exit(1);
    }

    char *command = args[2];

    if (strcmp(command, "LIST") == 0) {
        return LIST;
    }

    if (strcmp(command, "GET") == 0) {
        if (args[3] != NULL && args[4] != NULL) {
            return GET;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "DELETE") == 0) {
        if (args[3] != NULL) {
            return DELETE;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "PUT") == 0) {
        if (args[3] == NULL || args[4] == NULL) {
            print_client_help();
            exit(1);
        }
        return PUT;
    }

    // Not a valid Method
    print_client_help();
    exit(1);
}
