/**
* Networking Lab
* CS 241 - Fall 2018
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>


#include "dictionary.h"
#include "format.h"
#include "vector.h"
#include "common.h"

#define BUFFER_SIZE (1024)
#define MAX_BUFFER_SIZE (4096)
#define MAX_EVENTS (32)
#define V_UK (-1)
#define V_GET (1)
#define V_PUT (2)
#define V_DELETE (3)
#define V_LIST (4)

typedef struct connection_state
{
  int verb;

  char* header;
  size_t size_offset;
  int msg_flag;
  
  char* file_name;
  size_t file_size;
  
  char* buffer;
  size_t  buffer_offset;
  
  int client_fd;
  int local_fd;
}c_s;


static dictionary* dict;
static vector* files;
static char* dir;
static int serverSocket;
static int running = 1;
static size_t files_size = -1;
static char tmp_buffer[BUFFER_SIZE];
static int epoll_fd;
//test git

static void free_c_s(c_s* conn_status)
{
  free(conn_status -> header);
  free(conn_status -> file_name);
  free(conn_status -> buffer);
  free(conn_status);
}

static void v_put(c_s* conn_status, char* data)
{
  //fprintf(stdout, "V_PUT\n");
  memset(tmp_buffer, 0, BUFFER_SIZE);
  strcpy(tmp_buffer, dir);
  strcat(tmp_buffer, "/");
  strcat(tmp_buffer, conn_status -> file_name);
  //fprintf(stdout, "tmp buffer is %s\n", tmp_buffer);
  
  conn_status -> local_fd = open(tmp_buffer, O_CREAT|O_RDWR | O_TRUNC, 0777);

  if(conn_status -> local_fd < 0)
    {
      fprintf(stderr, "%s\n", "Failed to uploade file");
      return;
    }

  char* real_data = data + 8;
  //fprintf(stdout, "real data is %s\n", real_data);
  //fprintf(stdout, "buffer is %s\n", conn_status -> buffer);
  //fprintf(stdout, "buffer offset is %zu\n", conn_status -> buffer_offset);
  //fprintf(stderr, "size is %zu\n", (data + 8) - (conn_status -> buffer + conn_status -> buffer_offset));
  //fprintf(stderr, "file size %zu\n", conn_status -> file_size);
  if(real_data - (conn_status -> buffer + conn_status -> buffer_offset) < 0)
    {
      conn_status -> file_size = *(size_t*)(data + 1);
      //fprintf(stderr, "file_size is %zu\n", files_size);
      size_t read_byte = real_data - conn_status -> buffer + 1;
      size_t remain = conn_status -> buffer_offset - read_byte;
      //fprintf(stderr, "have read is %zu\n", read_byte);
      //fprintf(stderr, "remaining is %zu\n", remain);

      
      if(remain > 0)
	{
	  //here
	  size_t byte_write = write_all_to_socket(conn_status -> local_fd, real_data + 1, remain);
	  conn_status -> size_offset += byte_write;
	}
      conn_status -> buffer_offset = 0;
    }
}

static void v_get(c_s* conn_status)
{
  int exit_flag = 0;
  VECTOR_FOR_EACH(files, file, {
      if(strcmp(file, conn_status -> file_name) == 0)
	{
	  exit_flag = 1;
	  break;
	}
    });
  if(!exit_flag)
    {
      dprintf(conn_status -> client_fd, "%s\n%s", "ERROR", err_no_such_file);
    }
  else
    {
      memset(tmp_buffer, 0, BUFFER_SIZE);
      strcpy(tmp_buffer, "./");
      strcat(tmp_buffer, dir);
      strcat(tmp_buffer, "/");
      strcat(tmp_buffer, conn_status -> file_name);

      //int fd = open(tmp_buffer, O_RDONLY);
      //fprintf(stderr, "file name is %s\n", tmp_buffer);
      FILE* fp = fopen(tmp_buffer, "r");
      if(!fp)
	{
	  fprintf(stderr, "%s\n", "Get: cannot open local file");
	  return;
	}
      //fprintf(stderr, "file name is %s\n", tmp_buffer);
      dprintf(conn_status -> client_fd, "%s\n", "OK");
      fseek(fp, 0L, SEEK_END);
      size_t size_of_size = (size_t)ftell(fp);
      write_all_to_socket(conn_status -> client_fd, (char*)&size_of_size, 8);
      fprintf(stderr, "size of size is %zu\n", size_of_size);
      
      fseek(fp, 0L, SEEK_SET);

      //int read_byte = 0;
      char max_buffer[MAX_BUFFER_SIZE];
      memset(max_buffer, 0, MAX_BUFFER_SIZE);
      char* str;

      while(1)
	{
	  str = fgets(max_buffer, MAX_BUFFER_SIZE, fp);
	  if(str == NULL)
	    {
	      break;
	    }
	  //fprintf(stderr,"str is %s\n", str);
	  write_all_to_socket(conn_status -> client_fd, max_buffer, strlen(str));  
	}
      /*
      while( (str = fgets(max_buffer, MAX_BUFFER_SIZE, fp)) != NULL)
	{
	  write_all_to_socket(conn_status -> client_fd, max_buffer, strlen(str));
	}
      */
      /*
      while((read_byte = read_all_from_socket(fp, max_buffer, MAX_BUFFER_SIZE)) > 0)
	{
	  //fprintf(stderr, "bytes are %d\n", read_byte);
	  write_all_to_socket(conn_status -> client_fd, max_buffer, read_byte);
	  
	}
      */
      fclose(fp); 
    }
}

static void v_delete(c_s* conn_status)
{
  for(size_t i = 0; i < vector_size(files); i++)
    {
      char* local_file_name = vector_get(files, i);
      if(strcmp(conn_status -> file_name, local_file_name) == 0)
	{
	  fprintf(stderr, "file name to be deleted is %s\n", conn_status -> file_name);
	  vector_erase(files, (int)i);
	  files_size -= (strlen(conn_status -> file_name) - 1);
	  memset(tmp_buffer, 0, BUFFER_SIZE);
	  strcpy(tmp_buffer, dir);
	  strcat(tmp_buffer, "/");
	  strcat(tmp_buffer, conn_status -> file_name);

	  if(remove(tmp_buffer))
	    {
	      dprintf(conn_status -> client_fd, "%s\n%s\n", "ERROR", err_no_such_file);
	      return;
	    }
	  else
	    {
	      dprintf(conn_status -> client_fd, "OK\n");
	      return;
	    }
	}
    }
  dprintf(conn_status -> client_fd, "%s\n%s\n", "ERROR", err_no_such_file);
}

static void v_list(c_s* conn_status)
{
  dprintf(conn_status -> client_fd, "%s\n", "OK");
  if(files_size == (size_t)-1)
    {
      size_t size = 0;
      write(conn_status -> client_fd, &size, 8);
    }
  else
    {
      //files_size ++;
      write(conn_status -> client_fd, &files_size, 8);
    }
  //fprintf(stderr, "files size is %zu\n", vector_size(files));
  for(size_t i = 0; i < vector_size(files); i++)
    {
      char* local_file = vector_get(files, i);
      //fprintf(stderr, "local file is %s\n", local_file);
      if(i != (vector_size(files) - 1))
	{
	  write_all_to_socket(conn_status -> client_fd, local_file, strlen(local_file));
	  write_all_to_socket(conn_status -> client_fd, "\n", 1);
	}
      else
	{
	  write_all_to_socket(conn_status -> client_fd, local_file, strlen(local_file));
	}
    }
}

static int filter_cmd(char* header)
{
  char* cmd = strstr(header, "PUT");
  if(cmd && (cmd - header) == 0)
    {
      return V_PUT;
    }
  cmd = strstr(header, "GET");
  if(cmd && (cmd - header) == 0)
    {
      return V_GET;
    }

  cmd = strstr(header, "DELETE");
  if(cmd && (cmd - header) == 0)
    {
      return V_DELETE;
    }

  cmd = strstr(header, "LIST");
  if(cmd && (cmd - header) == 0)
    {
      return V_LIST;
    }
  return V_UK;
}

static void get_file_name(c_s* conn_status)
{
  char* head_ptr = strstr(conn_status -> header, " ");
  head_ptr += 1;
  size_t file_name_length = strstr(conn_status -> header, "\n") - head_ptr;
  strncat(conn_status -> file_name, head_ptr, file_name_length);
}

static void set_cmd(c_s* conn_status)
{
  int verb = filter_cmd(conn_status -> header);
  conn_status -> msg_flag = 1;
  if(verb == V_PUT)
    {
      conn_status -> verb = V_PUT;
      get_file_name(conn_status);
    }
  else if(verb == V_GET)
    {
      conn_status -> verb = V_GET;
      get_file_name(conn_status);
    }
  else if(verb == V_DELETE)
    {
      conn_status -> verb = V_DELETE;
      get_file_name(conn_status);
    }
  else if(verb == V_LIST)
    {
      conn_status -> verb = V_LIST;
    }
  else
    {
      conn_status -> verb = V_UK;
    }
}

static void run_cmd(c_s* conn_status, struct epoll_event e, char* data)
{
  if(conn_status -> verb == V_PUT)
    {
      v_put(conn_status, data);
    }
  else if(conn_status -> verb == V_GET)
    {
      v_get(conn_status);
      c_s* conn = (c_s*)dictionary_get(dict, &e.data.fd);
      free_c_s(conn);
      dictionary_remove(dict, &e.data.fd);
      close(e.data.fd);
    }
  else if(conn_status -> verb == V_DELETE)
    {
      v_delete(conn_status);
      c_s* conn = (c_s*)dictionary_get(dict, &e.data.fd);
      free_c_s(conn);
      dictionary_remove(dict, &e.data.fd);
      close(e.data.fd);
    }
  else
    {
      v_list(conn_status);
      c_s* conn = (c_s*)dictionary_get(dict, &e.data.fd);
      free_c_s(conn);
      dictionary_remove(dict, &e.data.fd);
      close(e.data.fd);
    }
}


static void accept_new_connection(int epoll_fd, struct epoll_event event)
{
  struct sockaddr_in sin;
  socklen_t socklen = sizeof(sin);
  int client_fd = accept(event.data.fd, (struct sockaddr*)&sin, &socklen);

  if(client_fd == -1)
    {
      fprintf(stderr, "Failed to accept new connection\n");
      return;
    }

  int flag = fcntl(client_fd, F_GETFL, 0);
  fcntl(client_fd, F_SETFL, flag | O_NONBLOCK);

  struct epoll_event e;
  e.data.fd = client_fd;
  e.events = EPOLLIN;

  if(epoll_ctl (epoll_fd, EPOLL_CTL_ADD, client_fd, &e) == -1){
    fprintf(stderr, "Failed to add file descriptor to epoll\n");
    close(epoll_fd);
    exit(-1);
  }

}


static void handle_request(struct epoll_event event)
{
  if(!dictionary_contains(dict, (void*)(&event.data.fd)))
    {
      //create c_s
      c_s* new_c_s = calloc(1, sizeof(c_s));
      new_c_s -> verb = V_UK;

      new_c_s -> header = calloc(128, sizeof(char));
      new_c_s -> size_offset = 0;
      new_c_s -> msg_flag = 0;
      
      new_c_s -> file_name = calloc(300, sizeof(char));
      new_c_s -> file_size = 0;
      
      new_c_s -> buffer = calloc(MAX_BUFFER_SIZE, sizeof(char));
      new_c_s -> buffer_offset = 0;

      new_c_s -> client_fd = event.data.fd;
      dictionary_set(dict, &event.data.fd, new_c_s);
    }

  c_s* conn_status = dictionary_get(dict, (void*)(&event.data.fd));

  char* ptr = conn_status -> buffer + conn_status -> buffer_offset;
  size_t num_to_read = MAX_BUFFER_SIZE - conn_status -> buffer_offset;
  size_t read_byte = read(event.data.fd, ptr, num_to_read); 
  conn_status -> buffer_offset += read_byte;
  
  if(!read_byte)
    {
      //fprintf(stderr, "NO BYTE TO READ\n");
      if(conn_status -> verb == V_PUT)
	{
	  if(conn_status -> size_offset == conn_status -> file_size)
	    {
	      int exit_flag = 0;
	      VECTOR_FOR_EACH(files, file, {
		  
		  if(strcmp(conn_status -> file_name, file) == 0)
		    {
		      exit_flag = 1;
		      break;
		    }
		});
	      
	      if(!exit_flag)
		{
		  vector_push_back(files, conn_status -> file_name);
		}
	      //what?
	      files_size += strlen(conn_status -> file_name) + 1;
	      dprintf(conn_status -> client_fd, "OK\n");
	    }
	  else if(conn_status -> size_offset < conn_status -> file_size)
	    {
	      print_too_little_data();
	      dprintf(conn_status -> client_fd, "%s\n%s\n", "ERROR", err_bad_file_size);
	    }
	  else
	    {
	      print_received_too_much_data();
	      dprintf(conn_status -> client_fd, "%s\n%s\n", "ERROR", err_bad_file_size);
	     
	    }
	  
	  close(event.data.fd);
	  //what?
	  close(conn_status -> local_fd);
	  c_s* conn = (c_s*)dictionary_get(dict, &event.data.fd);
	  free_c_s(conn);
	  dictionary_remove(dict, &event.data.fd);
	}
    }
  else
    {
      //fprintf(stderr, "READ BYTE\n");
      if(conn_status -> msg_flag)
	{
	  char* data = strstr(conn_status -> buffer, "\n");
	  memset(tmp_buffer, 0, BUFFER_SIZE);
	  strcpy(tmp_buffer, dir);
	  strcat(tmp_buffer, "/");
	  strcat(tmp_buffer, conn_status -> file_name);

	  if(conn_status -> file_size)
	    {
	      int byte_write = write_all_to_socket(conn_status -> local_fd, conn_status -> buffer, read_byte);
	      conn_status -> size_offset += byte_write;
	      conn_status -> buffer_offset = 0;
	    }
	  else
	    {
	      char* real_data = data + 8;
	      char* read_data = conn_status -> buffer + conn_status -> buffer_offset;
	      if(real_data - read_data < 0)
		{
		  size_t remain = conn_status -> buffer_offset - (real_data - conn_status -> buffer + 1);
		  conn_status -> file_size = *((size_t*)(data + 1));
		  if(remain > 0)
		    {
		      size_t byte_write = write_all_to_socket(conn_status -> local_fd, real_data + 1, remain);
		      conn_status -> size_offset += byte_write;
		    }
		  conn_status -> buffer_offset = 0;
		}
	    }
	}
      else
	{
	  //fprintf(stderr, "run cmd\n");
	  char* data = strstr(conn_status -> buffer, "\n");
	  if(data)
	    {
	      size_t merge_size = data - conn_status -> buffer + 1;
	      strncat(conn_status -> header, conn_status -> buffer, merge_size);
	      set_cmd(conn_status);
	      if(conn_status -> verb == V_UK)
		{
		  print_error_message((char*)err_bad_request);
		  dprintf(event.data.fd, "%s\n%s", "ERROR", err_bad_request);
		  close(event.data.fd);
		  c_s* conn = (c_s*)dictionary_get(dict, &event.data.fd);
		  free_c_s(conn);
		  dictionary_remove(dict, &event.data.fd);
		  return;
		}
	      else
		{
		  run_cmd(conn_status, event, data);
		}
	    }
	}
    }

}


static void run_server(char* port)
{
  printf("Initializing server\n");
  
  int s;
  serverSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
  int flags = fcntl(serverSocket, F_GETFL, 0);
  fcntl(serverSocket, F_SETFL, flags | O_NONBLOCK);
  
  int optval = 1;
  setsockopt(serverSocket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

  struct addrinfo hints, *result;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  s = getaddrinfo(NULL, port, &hints, &result);
  if(s != 0){
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
    freeaddrinfo(result);
    exit(1);
  }

  if(bind(serverSocket, result->ai_addr, result->ai_addrlen) != 0) {
    freeaddrinfo(result);
    perror("bind()");
    exit(1);
  }

  if(listen(serverSocket, 10) != 0){
    freeaddrinfo(result);
    perror("listen()");
    exit(1);
  }

  struct sockaddr_in sin;
  socklen_t socklen = sizeof(sin);
  if(getsockname(serverSocket, (struct sockaddr*)&sin, &socklen) == -1)
    {
      perror("getsockname()");
    }
  else
    {
      printf("Listening on port %hu\n", ntohs(sin.sin_port));
    }
  
  //free memory
  freeaddrinfo(result);

  //set up
  char tmp[] = "filesXXXXXX";
  dir = mkdtemp(tmp);
  print_temp_directory(dir);

  dict = int_to_shallow_dictionary_create();
  files = vector_create(string_copy_constructor,string_destructor,string_default_constructor);

  
  
  //set up epoll
  struct epoll_event events[MAX_EVENTS];
  //events = malloc(sizeof(struct epoll_event) * 32);
  //memset(events, 0, sizeof(struct epoll_event) * 32);

  epoll_fd = epoll_create(1);
  if(epoll_fd == -1)
    {
      fprintf(stderr, "Failed to create epoll file descriptor\n");
      exit(-1);
    }
  
  struct epoll_event event;
  event.data.fd = serverSocket;
  event.events = EPOLLIN;

  if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serverSocket, &event)){
    fprintf(stderr, "Failed to add file descriptor to epoll\n");
    close(epoll_fd);
    exit(-1);
  }

  printf("Ready to accept incoming connections\n");

  //int running = 1;
  int event_count;
  
  while(running)
    {
      event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
      
      for(int i = 0; i < event_count; i++)
	{
	  if(serverSocket == events[i].data.fd)
	    {
	      fprintf(stdout, "Accept a new connection\n");
	      accept_new_connection(epoll_fd, events[i]);
	    }
	  else
	    {
	      //fprintf(stdout, "%d run\n", i);
	      handle_request(events[i]);
	    }
	}
    }
  //close(epoll_fd);
}

void destroy()
{
  printf("destrou\n");
  size_t index = 0;
  while(index < vector_size(files))
    {
      char max_buffer[MAX_BUFFER_SIZE];
      memset(max_buffer, 0, MAX_BUFFER_SIZE);
      char* filename = vector_get(files, index);
      strcpy(max_buffer, dir);
      strcat(max_buffer, "/");
      strcat(max_buffer, filename);
      unlink(max_buffer);
      index++;
    }

  vector* v = dictionary_values(dict);
  index = 0;
  while(index < vector_size(v))
    {
      printf("free\n");
      c_s* conn_status = (c_s*)vector_get(v, index);
      free(conn_status -> buffer);
      free(conn_status -> header);
      free(conn_status -> file_name);
      free(conn_status);
      index++;
    }
  vector_destroy(v);
  /*
  for(size_t i = 0; i < vector_size(files); i++)
    {
      free((char*)vector_get(files, i));
    }
  */
  vector_destroy(files);
  dictionary_destroy(dict);
  if(!rmdir(dir))
    {
      running = 0;
    }
  close(epoll_fd);
  exit(-1);
}

int main(int argc, char **argv) {
  (void)argc;
  signal(SIGPIPE, SIG_IGN);
  signal(SIGINT, destroy);
  /*
  char tmp[] = "filesXXXXXX";
  dir = mkdtemp(tmp);
  print_temp_directory(dir);
  */
  //dict = int_to_shallow_dictionary_create();
  //files = vector_create(string_copy_constructor,string_destructor,string_default_constructor);

  run_server(argv[1]);

  //free memory
  //dictionary_destroy(dict);
  //vector_destroy(files);
  destroy();
  printf("Shutting down...\n");
  return 0;
}
