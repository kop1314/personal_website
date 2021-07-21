/**
* Mad Mad Access Patterns Lab
* CS 241 - Fall 2018
*/

#include "tree.h"
#include "utils.h"
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
/*
  Look up a few nodes in the tree and print the info they contain.
  This version uses mmap to access the data.

  ./lookup2 <data_file> <word> [<word> ...]
*/

static void* addr;

void tree_traversal(int offset, char* word)
{
  if(offset == 0)
    {
      printNotFound(word);
      return;

    }
  //printf("set node\n");
  BinaryTreeNode *node = (BinaryTreeNode*)(addr + offset);

  //printf("get result\n");
  int result = strcmp(word, node -> word);
  //printf("start recur\n");
  if(result == 0)
    {
      printFound(word, node -> count, node -> price);
      return;
    }
  else if(result > 0)
    {
      tree_traversal(node -> right_child, word);
    }
  else
    {
      tree_traversal(node -> left_child, word);
    }
}

int main(int argc, char **argv) {
  if(argc < 3)
    {
      printArgumentUsage();
      exit(1);
    }

  char* file = argv[1];
  struct stat sb;

  int fd = open(file, O_RDONLY);
  if (fd == -1)
    {
      openFail(file);
      exit(2);
    }

  if(fstat(fd, &sb) == -1)
    {
      openFail(file);
      exit(2);
    }

  //check it is a regular file
  if(!S_ISREG(sb.st_mode))
    {
      openFail(file);
      exit(2);
    }

  addr = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
  if(addr == MAP_FAILED)
    {
      mmapFail(file);
      exit(3);
    }

  if(strncmp(addr, "BTRE", 4) != 0)
    {
      formatFail(file);
      exit(2);
    }

  int index = 2;
  while(index < argc)
    {
      //printf("word is %s\n", argv[index]);
      tree_traversal(4, argv[index]);
      index++;
    }

  munmap(addr, sb.st_size);
  close(fd);
  return 0;
}
