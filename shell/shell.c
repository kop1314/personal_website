/**
* Shell Lab
* CS 241 - Fall 2018
*/
#include "format.h"
#include "shell.h"
#include "vector.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <linux/limits.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <ctype.h>
#include <fcntl.h>

typedef struct process {
  char *command;
  char *status;
  pid_t pid;
} process;

static vector* processes;
static vector* file_cmds;
static char* ffn = NULL;
static char* hfn = NULL;
static int hflag = 0;
static int fflag = 0;

int run_cmd(char* cmd, vector* history, int flag);
int check_logic_operation(char* cmd, char** cmds, vector* history);
void e_cmd(char* cmd, vector* history, int flag);

void destroy_processes() {
  VECTOR_FOR_EACH(processes, p, {
      kill(((process*)p)->pid, SIGTERM);
    });
  //vector_destroy(file_cmds);
}

void kill_process(int pid)
{
  size_t index = 0;
  while(index < vector_size(processes))
    {
      process * p = vector_get(processes, index);
      if(p -> pid == pid)
	{
	  kill(pid,SIGTERM);
	  print_killed_process(p->pid, p->command);
	  vector_erase(processes, index);
	  return;

	}
      index++;
    }
  print_no_process_found(pid);
}

void stop_process(int pid)
{
  VECTOR_FOR_EACH(processes, p, {
      if(((process*)p) -> pid == pid)
	{
	  kill(pid, SIGTSTP);
	  print_stopped_process(((process*)p)->pid, ((process*)p)->command);
	  ((process*)p)->status = STATUS_STOPPED;
	  return;
	}
    });
  print_no_process_found(pid);
  return;
}

void cont_process(pid_t pid)
{
  VECTOR_FOR_EACH(processes, p, {
      if(((process*)p) -> pid == pid)
	{
	  kill(pid, SIGCONT);
	  ((process*)p)->status = STATUS_RUNNING;
	  return;
	}
    });
  print_no_process_found(pid);
  return;
  return;
}

int check_prefix(char* p, vector* v)
{
  if(*p == '\0')
    {
      return vector_size(v) - 1;
    }
  char* ptr = NULL;
  for(size_t i = vector_size(v); i-- > 0;)
    {
      char* c = vector_get(v, i);
      ptr = strstr(c, p);
      if(ptr != NULL && (ptr - c) == 0)
	{
	  return i;
	}
    }
  return -1;
}

void print_all_process()
{
  VECTOR_FOR_EACH(processes, p, {
      print_process_info(((process*)p)->status, (int)((process*)p)->pid, ((process*)p)->command);
   });
}

int check_background(char* cmd)
    {

      size_t index = strlen(cmd) - 1;

      while(*(cmd + index) == ' ')
	{
	  index--;
	}

      if(*(cmd +index) == '&' && ((size_t)(strstr(cmd, "&") - cmd) == index))
	{
	  while(*(cmd + index) == '&' || *(cmd +index) == ' ')
	    {
	      index--;
	    }
	  index++;
	  memmove(cmd + index, cmd + strlen(cmd), index);
	  return 1;
	}
      return 0;
    }

void write_file_to_file_cmds(char* filename, vector* v)
  {
    
    FILE* file = fopen(filename, "r+");
    if(file == NULL)
      {
	print_script_file_error();
	return;
	
      }
    //fprintf(stderr, "-h file\n");
    size_t status = 0;
    char* buffer = NULL;
    int len;
    while((len = getline(&buffer, &status, file)) != -1)
      {
	if(len > 0 && buffer[len - 1] == '\n')
	  {
	    //printf("it has eq\n");
	    buffer[len - 1] = 0;
	  }
	//printf("buffer is %s\n", buffer);
	vector_push_back(v, buffer);

      }
    free(buffer);
    fclose(file);
  }

void write_hfile_to_file_cmd(char* filename, vector* v)
{
  FILE * file = fopen(filename, "r+");
  if (!file) {
    FILE * n_s = fopen(filename, "w+");
    fclose(n_s);
    return;
  }
  size_t status = 0;
  char* buffer = NULL;
  int len;
  while((len = getline(&buffer, &status, file)) != -1)
    {
      if(len > 0 && buffer[len - 1] == '\n')
	{
	  //printf("it has eq\n");
	  buffer[len - 1] = 0;
	}
      //printf("buffer is %s\n", buffer);
      vector_push_back(v, buffer);

    }
  
  free(buffer);
  fclose(file);
   
}

  void write_histories_to_file(char* filename, vector* v)
  {
    FILE* file = fopen(filename, "w+");
    if(!file)
      {
	print_history_file_error();
      }
    for(size_t i = 0; i < vector_size(v); i++){
      fprintf(file,"%s\n",vector_get(v,i));
    }
    fclose(file);
  }

  char* check_escape_sequence(char* cmd)
  {
    char* newCmd = strdup(cmd);

    char* ptr = cmd;
    size_t index = 0;
    while(ptr[index])
      {
	if(ptr[index] == '\\')
	  {
	    char* t = ptr + 1;
	    size_t size = strlen(t);
	    ptr = memmove(ptr, t, size + 1);
	  }
	index++;
      }
    //printf("newCmd is %s\n", newCmd);
    return newCmd;
  }

  int check_logic_operation(char* cmd, char** cmds, vector* history)
  {
    if(strcmp(cmd, "") != 0)
      {
	vector_push_back(history, cmd);
      }
    char* ptr = NULL;
    size_t size = 0;
    if((ptr = strstr(cmd, "&&")))
      {
	size = ptr - cmd - 1;
	if(isspace(cmd[size]))
	  {
	    while(isspace(cmd[size]))
	      {
		size--;
	      }
	    size++;
	  }
	else
	  {
	    size++;
	  }

	//copy cmds
	memcpy(cmds[0], cmd, size);

	size_t index = ptr - cmd;
	while(cmd[index] == ' ' || cmd[index] == '&')
	  {
	    index++;
	  }
	memcpy(cmds[1], cmd + index, strlen(cmd) - index);
	return 1;
      }
    else if((ptr = strstr(cmd, "||")))
      {
	size = ptr - cmd - 1;
	if(isspace(cmd[size]))
	  {
	    while(isspace(cmd[size]))
	      {
		size--;
	      }
	    size++;
	  }
	else
	  {
	    size++;
	  }

	//copy cmds
	memcpy(cmds[0], cmd, size);

	size_t index = ptr - cmd;
	while(cmd[index] == ' ' || cmd[index] == '|')
	  {
	    index++;
	  }
	memcpy(cmds[1], cmd + index, strlen(cmd) - index);
	return 2;
      }
    else if((ptr = strstr(cmd, ";")))
      {
	size = ptr - cmd - 1;
	if(isspace(cmd[size]))
	  {
	    while(isspace(cmd[size]))
	      {
		size--;
	      }
	    size++;
	  }
	else
	  {
	    size++;
	  }

	//copy cmds
	memcpy(cmds[0], cmd, size);

	size_t index = ptr - cmd;
	while(cmd[index] == ' ' || cmd[index] == ';')
	  {
	    index++;
	  }
	memcpy(cmds[1], cmd + index, strlen(cmd) - index);
	return 3;
      }
    else
      {

      }
    return 0;
  }

  void e_cmd(char* cmd, vector* history, int flag)
  {
    char** cmds = calloc(2, sizeof(char*));
    cmds[0] = calloc(PATH_MAX, sizeof(char*));
    cmds[1] = calloc(PATH_MAX, sizeof(char*));
    int logic = check_logic_operation(cmd, cmds, history);
    //printf("logic is %d\n", logic);
    int status = 0;
    switch(logic)
      {
      case 1:
	//and run_cmd
        status = run_cmd(cmds[0], history, flag);
	if(!status)
	  {
	    run_cmd(cmds[1], history, flag);
	    break;
	  }
	else
	  {
	    break;
	  }
      case 2:
	//or run cmd
	status = run_cmd(cmds[0], history, flag);
	if(!status)
	  {
	    break;
	  }
	else
	  {
	    run_cmd(cmds[1], history, flag);
	    break;
	  }
      case 3:
	//: run cmd
	run_cmd(cmds[0], history, flag);
	run_cmd(cmds[1], history, flag);
	break;
      default:
	run_cmd(cmd, history, flag);
      }
    //run cmd
    free(cmds[0]);
    free(cmds[1]);
    free(cmds);

  }


  //success 0 fail 1
  int run_cmd(char* cmd, vector* history, int flag)
  {
    //filter cmd
    char* newCmd = check_escape_sequence(cmd);
    //printf("newCmd is %s\n", newCmd);
    //printf("get bp\n");
    //not done yet
    char* ptr = NULL;
    //int ret = 0;
    if((ptr = strstr(newCmd, "cd")) && *(ptr + 2) == ' ')
      {
	ptr += 2;
	while(*ptr == ' ')
	  {
	    ptr++;
	  }
	if(strchr(ptr, ' '))
	  {
	    print_invalid_command(cmd);
	  }
	if(chdir(ptr) == -1)
	  {
	    print_no_directory(ptr);
	    return 1;
	  }
	else
	  {
	    return 0;
	  }
      }

    else if(strcmp(newCmd, "!history") == 0)
      {
	vector_pop_back(history);
	for(size_t i = 0; i < vector_size(history); i++)
	  {
	    print_history_line(i, (char*)vector_get(history, i));
	  }
	return 0;
      }
    else if((ptr = strstr(newCmd, "#")))
      {
	vector_pop_back(history);
	size_t cmd_index = atoi(ptr + 1);
	if((int)cmd_index < 0)
	  {
	    print_invalid_index();
	  }
	if(cmd_index < vector_size(history))
	  {
	    char* print_cmd = (char*)vector_get(history, cmd_index);
	    if(print_cmd != NULL)
	      {
		printf("%s\n", print_cmd);
		e_cmd(print_cmd, history, flag);
	      }
	  }
	else
	  {
	    print_invalid_index();

	  }
	return 0;
      }
    else if(newCmd[0] == '!')
      {
	vector_pop_back(history);
	char* prefix = newCmd + 1;
	int pflag = vector_size(history) - 1;
	if(strlen(newCmd) > 1)
	  {
	    pflag = check_prefix(prefix, history);
	  }
	//printf("pflag is %d\n", pflag);
	if(pflag >= 0)
	  {
	    char* re_cmd = vector_get(history, pflag);
	    printf("%s\n", re_cmd);
	    e_cmd(re_cmd, history, flag);
	  }
	else
	  {
	    print_no_history_match();

	  }
	return 0;
      }
    else if(strcmp(newCmd, "ps") == 0)
      {
	print_all_process();
	return 0;
      }
    else if(strlen(newCmd) > 4 && (strstr(newCmd, "kill") - newCmd) == 0)
      {
	int pid = atoi(newCmd + 5);
	if(!pid)
	  {
	    print_invalid_command(cmd);
	    return 1;
	    //ret = -1;
	  }
	else
	  {
	    kill_process(pid);
	  }
	return 0;
      }
    else if(strlen(newCmd) > 4 && (strstr(newCmd, "stop") - newCmd) == 0)
      {
	pid_t pid = atoi(newCmd + 5);
	if(!pid)
	  {
	    print_invalid_command(cmd);
	    return 1;
	  }
	else
	  {
	    stop_process(pid);
	  }
      }
    else if(strlen(newCmd) > 4 && (strstr(newCmd, "cont") - newCmd) == 0)
      {
	pid_t pid = atoi(newCmd + 5);
	if(!pid)
	  {
	    print_invalid_command(cmd);
	    return 1;
	  }
	else
	  {
	    cont_process(pid);
	  }
      }
    else if(strcmp(newCmd, "exit") == 0)
      {
	//fprintf(stderr, "exit hfn is %s\n", hfn);
	if(hfn)
	  {
	    vector_pop_back(history);
	    char* path = get_full_path(hfn);
	    //fprintf(stdout, "path is %s\n", path);
	    write_histories_to_file(path, history);
	  }
	destroy_processes();
	vector_destroy(history);
	exit(0);

	//do not handle EOP
      }
    else
      {
	//printf("external cmd\n");
	int bg = check_background(newCmd);
	//printf("bg is %d\n", bg);
	if(strcmp(newCmd, "&") == 0)
	  {
	    printf("invalid cmd\n");
	    return 0;
	  }
	if(!bg)
	  {
	    //printf("no bg\n");
	    char* args[128];
	    int index = 0;
	    char s = ' ';
	    char* token = strtok(newCmd,&s);
	    args[0] = token;
	    while(token != NULL)
	      {
		token = strtok(NULL, &s);
		index++;
		args[index] = token;
	      }
	    pid_t pid = fork();
	    if(!pid)
	      {
		//printf("i am child\n");
		/*
		char* args[128];
		int index = 0;
		char s = ' ';
		char* token = strtok(newCmd,&s);
		args[0] = token;
		while(token != NULL)
		  {
		    token = strtok(newCmd, &s);
		    index++;
		    args[index] = token;
		  }
		*/
		//printf("cmd to be run is %s\n", args[0]);
		int ret = execvp(args[0], args);
		if(ret == -1)
		  {
		    print_exec_failed(cmd);
		    exit(1);
		  }
	      }
	    else if(pid > 0)
	      {
		print_command_executed(pid);
		int status;
		if(waitpid(pid, &status, 0) == -1)
		  {
		    print_wait_failed();
		    destroy_processes();
		    exit(1);
		  }
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
		  {
		    //printf("failed\n");
		    return 1;

		  }
	      }
	    else
	      {
		print_fork_failed();
		destroy_processes();
		exit(1);
	      }
	  }
	else
	  {
	    //printf("non_bg cmd is %s\n", newCmd);
	    pid_t pid = fork();
	    if(!pid)
	      {
		char* args[128];
		int index = 0;
		char s = ' ';
		char* token = strtok(newCmd,&s);
		args[0] = token;
		while(token != NULL)
		  {
		    token = strtok(NULL, &s);
		    index++;
		    args[index] = token;
		  }
		//printf("child newCmd is%s\n", newCmd);
		int ret = execvp(args[0], args);
		if(ret == -1)
		  {
		    print_exec_failed(cmd);
		    exit(1);
		  }
	      }
	    else if(pid > 0)
	      {
		if (setpgid(pid, pid) == -1)
		  {
		    print_setpgid_failed();
		    destroy_processes();
		    exit(1);

		  }
		print_command_executed(pid);
		process * my_process = calloc(1, sizeof(process));
		char* p_cmd = strdup(cmd);
		my_process->command = p_cmd;
		my_process->pid = pid;
		my_process->status = STATUS_RUNNING;


		vector_push_back(processes, my_process);
	      }
	    else
	      {
		print_fork_failed();
		destroy_processes();
		exit(1);
	      }
	  }
      }
    return 0;
  }

  int invalid_hfn()
  {
    //fprintf(stderr, "invalid\n");
    size_t index = 0;
    int invalid = 0;
    while(index < strlen(hfn))
      {
	if(hfn[index] == '/')
	  {
	    invalid = 1;
	    break;
	  }
	index++;
      }
    if(invalid)
      {
	print_usage();
	return 1;
      }
    // char* tmp = hfn;
    hfn = get_full_path(hfn);
    //free(tmp);
    return 0;
  }

  void handle_sigint()
  {
    return;
  }

  void handle_sigchld() {
    int status;
    pid_t pid = 0;
    if ((pid = waitpid(-1, &status, WNOHANG)) != -1){
      size_t i = 0;
      while(i < vector_size(processes))
	{
	  process* p = vector_get(processes, i);
	  if(p -> pid == pid)
	    {
	      vector_erase(processes, i);
	    }
	  i++;
	}
    }
  }

  int shell(int argc, char *argv[]) {
    // TODO: This is the entry point for your shell.
    //signal(SIGCHLD, handle_sigchld);
    signal(SIGINT, handle_sigint);
    signal(SIGCHLD, handle_sigchld);
    if(argc%2 ==0 || argc > 5)
      {
	print_usage();
      }
    int opt;

    while((opt = getopt(argc, argv, "f:h:")) != -1)
      {
	switch(opt)
	  {
	  case 'f':
	    if(fflag == 0)
	      {
		fflag = 1;
		ffn = optarg;
		//printf("fflag is %d, optarg is %s\n", fflag, ffn);
	      }
	    else
	      {
		print_usage();
		return 0;
	      }
	    break;
	  case 'h':
	    if(hflag == 0)
	      {
		hflag = 1;
		hfn = optarg;
		//printf("hflag is %d, optarg is %s\n", hflag, hfn);
	      }
	    else
	      {
		print_usage();
		return 0;
	      }
	    break;
	  default:
	    print_usage();
	  }
      }

    //what?
    // int escape_seq_flag = 0;
    vector* histories = string_vector_create();
    file_cmds = string_vector_create();
    if(hfn)
      {
	//???
	write_hfile_to_file_cmd(hfn, histories);
      }
    if(ffn)
      {
	write_file_to_file_cmds(ffn, file_cmds);
      }

    processes = shallow_vector_create();
    process* parent_process = malloc(sizeof(process));
    parent_process -> command = argv[0];
    parent_process -> status = STATUS_RUNNING;
    parent_process -> pid = getpid();
    vector_push_back(processes, parent_process);

    if(hflag)
      {
	if(invalid_hfn())
	  {
	    return 0;
	  }
      }
    //fprintf(stderr, "path is %s!\n", hfn);
    if(fflag)
      {
	//fprintf(stdout,"-f flag\n");
	size_t index = 0;
	while(index < vector_size(file_cmds))
	  {
	    char* cmd = vector_get(file_cmds, index);

	    char cwd[PATH_MAX];
	    getcwd(cwd, PATH_MAX);
	    pid_t pid = getpid();
	    print_prompt(cwd, pid);
	    printf("%s\n", cmd);


	    char** cmds = calloc(2, sizeof(char*));
	    cmds[0] = calloc(PATH_MAX, sizeof(char*));
	    cmds[1] = calloc(PATH_MAX, sizeof(char*));
	    int logic = check_logic_operation(cmd, cmds, histories);
	    int status = 0;
	    switch(logic)
	      {
	      case 1:
		//and run_cmd
		status = run_cmd(cmds[0], histories, 1);
		if(!status)
		  {
		    run_cmd(cmds[1], histories, 1);
		    break;
		  }
		else
		  {
		    break;
		  }
	      case 2:
		//or run cmd
		status = run_cmd(cmds[0], histories, 1);
		if(!status)
		  {
		    break;
		  }
		else
		  {
		    run_cmd(cmds[1], histories, 1);
		    break;
		  }
	      case 3:
		//: run cmd
		run_cmd(cmds[0], histories, 1);
		run_cmd(cmds[1], histories, 1);
		break;
	      default:
		run_cmd(cmd, histories, 0);
	      }
	    //run cmd
	    free(cmds[0]);
	    free(cmds[1]);
	    free(cmds);
	    index++;
	  }
	e_cmd("exit", histories, 0);
	//printf("read to destroy\n");
	destroy_processes();
	//printf("go to exit\n");
	//e_cmd("exit", histories, 0);
	//printf("done scan file!\n");
      }

    if(!fflag)
      {
	//fprintf(stderr, "here!\n");
	while(true)
	  {
	    if(feof(stdin))
	      {
		fprintf(stderr, "end of line\n");
		run_cmd("exit", histories, 0);
		destroy_processes();
		//run_cmd("exit", histories, 0);
		exit(0);
	      }
	    char cwd[PATH_MAX];
	    getcwd(cwd, PATH_MAX);
	    pid_t pid = getpid();
	    print_prompt(cwd, pid);

	    char cmd[PATH_MAX];
	  fgets(cmd, PATH_MAX, stdin);
	  cmd[strlen(cmd) - 1] = '\0';

	  check_escape_sequence(cmd);
	  
	  if (strcmp(cmd, "") == 0)
	    {
	      continue;

	    }

	  if(strcmp(cmd, "exit") == 0)
	    {
	      if(hflag)
		{
		  write_histories_to_file(hfn,histories);
		}
	      
	      destroy_processes();
	      vector_destroy(histories);
	      exit(0);
	    }
	  //printf("cwd is %s", cwd);
	  e_cmd(cmd,histories, 0);
      }
  }
  return 0;
}
