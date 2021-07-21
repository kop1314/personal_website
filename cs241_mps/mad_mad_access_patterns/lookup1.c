/**
* Mad Mad Access Patterns Lab
* CS 241 - Fall 2018
*/

#include "tree.h"
#include "utils.h"
#include <string.h>
#include <stdlib.h>

/*
  Look up a few nodes in the tree and print the info they contain.
  This version uses fseek() and fread() to access the data.

  ./lookup1 <data_file> <word> [<word> ...]
*/
static char* file;

void tree_traversal(int offset, char* word, FILE* fp)
{
  if(!offset)
    {
      printNotFound(word);
      return;
    }

  //fseek to offset and exit if fail
  if(fseek(fp, offset, SEEK_SET) == -1)
    {
      formatFail(file);
      exit(3);
    }

  //printf("init node\n");
  BinaryTreeNode node;
  //printf("read node\n");
  fread(&node, sizeof(BinaryTreeNode), 1, fp);

  //printf("seek word pos\n");
  if(fseek(fp, offset + sizeof(BinaryTreeNode), SEEK_SET) == -1)
    {
      formatFail(file);
      exit(3);
    }

  //printf("get word\n");
  char a_word[256];
  if(!fgets(a_word, 256, fp))
    {
      formatFail(file);
      exit(4);
    }

  //printf("start recursion\n");
  int ret = strcmp(word, a_word);
  if(ret == 0)
    {
      printFound(word, node.count, node.price);
      return;
    }
  else if(ret > 0)
    {
      tree_traversal(node.right_child, word, fp);
    }
  else
    {
      tree_traversal(node.left_child, word, fp);
    }
  
}

int main(int argc, char **argv) {
  if(argc < 3)
    {
      printArgumentUsage();
      exit(1);
    }
  
  file = argv[1];

  FILE *fp = fopen(file, "r");
  if(!fp)
    {
      openFail(file);
      exit(2);
    }

  char buffer[4];
  fread(buffer, 4, 1, fp);
  if(strcmp(buffer, "BTRE") != 0)
    {
      formatFail(file);
      exit(2);
    }
  //printf("start loop\n");
  int index = 2;
  while(index < argc)
    {
      //printf("word is %s\n", argv[index]);
      tree_traversal(4, argv[index], fp);
      index++;
    }
  fclose(fp);
  return 0;
}
