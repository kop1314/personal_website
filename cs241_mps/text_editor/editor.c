/**
* Text Editor Lab
* CS 241 - Fall 2018
*/

#include "document.h"
#include "editor.h"
#include "format.h"
#include "sstring.h"

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_filename(int argc, char *argv[]) {
    // TODO implement get_filename
    // take a look at editor_main.c to see what this is used for
  
  return argv[argc - 1];
}

sstring *handle_create_string() {
    // TODO create empty string
  
  return cstr_to_sstring("");
}
document *handle_create_document(const char *path_to_file) {
    // TODO create the document
  return document_create_from_file(path_to_file);
}

void handle_cleanup(editor *editor) {
    // TODO destroy the document
  document_destroy(editor->document);
}

void handle_display_command(editor *editor, size_t start_line,
                            ssize_t max_lines, size_t start_col_index,
                            ssize_t max_cols) {
    // TODO implement handle_display_command
  if(start_line == 0 || document_size(editor->document) == 0)
    {
      print_document_empty_error();
    }
  if(max_lines == -1)
    {
      for(size_t i = start_line; i <= document_size(editor->document); i++)
	{
	  print_line(editor->document, i, start_col_index, max_cols);

	}
    }
  else
    {
      if(max_lines > (ssize_t)(document_size(editor->document) - start_line + 1))
	{
	  for(size_t i = start_line; i <= document_size(editor->document); i++)
	    {
	      print_line(editor->document, i, start_col_index, max_cols);

	    }
	}
      else
	{
	  for(size_t i = start_line; i < start_line + max_lines; i++)
	    {
	      print_line(editor->document, i, start_col_index, max_cols);

	    }
	}
    }
}

void handle_insert_command(editor *editor, location loc, const char *line) {
    // TODO implement handle_insert_command
  //insert line pass the end of document size
  if(loc.line_no > document_size(editor->document))
    {
      document_insert_line(editor->document, loc.line_no, line);
      return;
    }
  const char* oLine = document_get_line(editor -> document, loc.line_no);
  char* buffer = malloc((strlen(line) + strlen(oLine)) * sizeof(char) + 1);
  strncpy(buffer, oLine, loc.idx);
  buffer[loc.idx] = '\0';
  strcat(buffer, line);
  strcat(buffer, oLine + loc.idx);
  document_set_line(editor->document, loc.line_no, (const char*)buffer);
  free(buffer);
}

void handle_append_command(editor *editor, size_t line_no, const char *line) {
    // TODO implement handle_append_command
  //char* buffer = document_get_line(editor-> document, line_no);
 
  size_t index = 0;
  size_t startP = 0;
  size_t newLineNo = line_no;
  while(index < strlen(line))
    {
      if(line[index] == '\\' && line[index + 1] != 'n')
	{
	  index++;
	}
      else if(line[index] == '\\' && line[index + 1] =='n')
	{
	  size_t numOfChar = 0;
	  size_t index2 = startP;
	  while(index2 < index)
	    {
	      if(line[index2] == '\\')
		{
		  numOfChar++;
		  index2++;
		}
	      else
		{
		  numOfChar++;
		}
	      index2++;
	    }
	  char* buffer = malloc(numOfChar* sizeof(char) + 1);
	  buffer[numOfChar] = '\0';
	  index2 = startP;
	  size_t index3 = 0;
	  while(index2 < index)
	    {
	      if(line[index2] == '\\')
		{
		  buffer[index3] = line[index2+1];
		  index3++;
		  index2++;
		}
	      else
		{
		  buffer[index3] = line[index2];
		  index3++;
		}
	      index2++;

	    }
	  //set line
	  if(newLineNo == line_no)
	    {
	      if(newLineNo <= document_size(editor->document))
		{
		  const char* temp = document_get_line(editor->document, newLineNo);
		  handle_insert_command(editor, (location){newLineNo, strlen(temp)}, buffer);

		}
	      else
		{
		  document_insert_line(editor->document, newLineNo, buffer);
		}
	    }
	  //insert new lines
	  else
	    {
	      document_insert_line(editor->document, newLineNo, buffer);
	    }
	  newLineNo ++;
	  startP = index +2;
	  index++;
	  free(buffer);
	}
      else{
	//do nothing
      }
      index++;
    }

  //last segment
  if(startP < strlen(line))
    {
      size_t index3 = startP;
      size_t index4 = 0;
      size_t numOfChar = 0;

      while(index3 < strlen(line))
	{
	  if(line[index3] == '\\')
		{
		  numOfChar++;
		  index3++;
		}
	      else
		{
		  numOfChar++;
		}
	      index3++;
	}
      
      index3 = startP;
      char* buffer = malloc(numOfChar * sizeof(char) + 1);
      buffer[numOfChar] = '\0';
      while(index3 < strlen(line))
	{

	  if(line[index] == '\\' && index == strlen(line) - 1)
	    {
	    }
	  else if(line[index3] == '\\')
	    {
	      buffer[index4] = line[index3+1];
	      index4++;
	      index3++;
	    }
	  else
	    {
	      buffer[index4] = line[index3];
	      index4++;
	    }
	      
	  index3++;
	}
       //set line
	  if(newLineNo == line_no)
	    {
	      if(newLineNo <= document_size(editor->document))
		{
		  //handle_insert_command(editor, (location){newLineNo, strlen(line)}, buffer);
		  const char* temp = document_get_line(editor->document, newLineNo);
		  handle_insert_command(editor, (location){newLineNo, strlen(temp)}, buffer);

		}
	      else
		{
		  document_insert_line(editor ->document, newLineNo, buffer);
		}
	    }
	  //insert new lines
	  else
	    {
	      document_insert_line(editor ->document, newLineNo, buffer);;
	    }
	  free(buffer);
    }
  if(startP == strlen(line))
    {
      document_insert_line(editor -> document, newLineNo, "");
    }
}

void handle_write_command(editor *editor, size_t line_no, const char *line) {
    // TODO implement handle_write_command

  size_t index = 0;
  size_t startP = 0;
  size_t newLineNo = line_no;
  while(index < strlen(line))
    {
      if(line[index] == '\\' && line[index + 1] != 'n')
	{
	  index++;
	}
      else if(line[index] == '\\' && line[index + 1] =='n')
	{
	  size_t numOfChar = 0;
	  size_t index2 = startP;
	  while(index2 < index)
	    {
	      if(line[index2] == '\\')
		{
		  numOfChar++;
		  index2++;
		}
	      else
		{
		  numOfChar++;
		}
	      index2++;
	    }
	  char* buffer = malloc(numOfChar* sizeof(char) + 1);
	  buffer[numOfChar] = '\0';
	  index2 = startP;
	  size_t index3 = 0;
	  while(index2 < index)
	    {
	      if(line[index2] == '\\')
		{
		  buffer[index3] = line[index2+1];
		  index3++;
		  index2++;
		}
	      else
		{
		  buffer[index3] = line[index2];
		  index3++;
		}
	      index2++;

	    }
	  //set line
	  if(newLineNo == line_no)
	    {
	      if(newLineNo <= document_size(editor->document))
		{
		  document_set_line(editor->document, newLineNo, buffer);

		}
	      else
		{
		  document_insert_line(editor->document, newLineNo, buffer);
		}
	    }
	  //insert new lines
	  else
	    {
	      document_insert_line(editor ->document, newLineNo, buffer);
	    }
	  newLineNo ++;
	  startP = index +2;
	  index++;
	  free(buffer);
	}
      else{
	//do nothing
      }
      index++;
    }

  //last segment
  if(startP < strlen(line))
    {
      size_t index3 = startP;
      size_t index4 = 0;
      size_t numOfChar = 0;

      while(index3 < strlen(line))
	{
	  if(line[index3] == '\\')
		{
		  numOfChar++;
		  index3++;
		}
	      else
		{
		  numOfChar++;
		}
	      index3++;
	}
      
      index3 = startP;
      char* buffer = malloc(numOfChar * sizeof(char) + 1);
      buffer[numOfChar] = '\0';
      while(index3 < strlen(line))
	{

	  if(line[index] == '\\' && index == strlen(line) - 1)
	    {
	    }
	  else if(line[index3] == '\\')
	    {
	      buffer[index4] = line[index3+1];
	      index4++;
	      index3++;
	    }
	  else
	    {
	      buffer[index4] = line[index3];
	      index4++;
	    }
	      
	  index3++;
	}
       //set line
	  if(newLineNo == line_no)
	    {
	      if(newLineNo <= document_size(editor->document))
		{
		  document_set_line(editor->document, newLineNo, buffer);

		}
	      else
		{
		  document_insert_line(editor ->document, newLineNo, buffer);
		}
	    }
	  //insert new lines
	  else
	    {
	      document_insert_line(editor ->document, newLineNo, buffer);;
	    }
	  free(buffer);
    }
  if(startP == strlen(line))
    {
      document_insert_line(editor -> document, newLineNo, "");
    }
}

void handle_delete_command(editor *editor, location loc, size_t num_chars) {
    // TODO implement handle_delete_command
  if(loc.line_no > document_size(editor->document))
    {
      for(size_t i = 1; i<= document_size(editor->document); i++)
	{
	  handle_delete_line(editor, i);
	}
      return;
    }

  const char* temp = document_get_line(editor->document, loc.line_no);
  if((strlen(temp) - loc.idx) < num_chars)
    {
      char* buffer = malloc(loc.idx * sizeof(char) + 1);
      buffer[loc.idx] = '\0';
      strncpy(buffer, temp, loc.idx);
      document_set_line(editor->document, loc.line_no, buffer);
      free(buffer);
    }
  else
    {
      char* buffer = malloc((strlen(temp) - num_chars) * sizeof(char) + 1);
      buffer[strlen(temp) - num_chars] = '\0';
      strncpy(buffer,temp, loc.idx);
      strncpy(buffer + loc.idx, temp + loc.idx + num_chars, strlen(temp) - loc.idx - num_chars);
      document_set_line(editor->document, loc.line_no, buffer);
      free(buffer);
    }
}

void handle_delete_line(editor *editor, size_t line_no) {
    // TODO implement handle_delete_line
  if(line_no >document_size(editor->document))
    {
      return;
    }
   document_delete_line(editor->document, line_no);
}

location handle_search_command(editor *editor, location loc,
                               const char *search_str) {
    // TODO implement handle_search_command
  if(strcmp(search_str, "") == 0){
    return (location){0, 0};
  }
  
  location loc1 = (location){0,0};
  if(loc.idx >= strlen(document_get_line(editor->document, loc.line_no)))
    {
      
       for(size_t i  = (loc.line_no + 1); i <= document_size(editor->document); i++)
	 {
	   const char* temp = document_get_line(editor->document, i);
	   
	   char* ret = strstr(temp, search_str);
	   
	   if(ret != NULL)
	     {
	       //printf("ret is not null\n");
	       
		   loc1 = (location){i, (size_t)(ret - temp)};
		   //		   printf("loc1: line_no is %zu, idx is %zu\n", loc1.line_no, loc1.idx);
		   return loc1;
		
	     }
	 }
	
    }
  else
    {
      for(size_t i  = loc.line_no; i <= document_size(editor->document); i++)
	 {
	   const char* temp = document_get_line(editor->document, i);
	   if(i == loc.line_no)
	     {
	       char* ret = strstr(temp + loc.idx, search_str);
	       //printf("ret is %s\n", ret);
	       if(ret!=NULL)
		 {
		   loc1 = (location){i, (size_t)(ret - temp)};
		   return loc1;
		 }
	     }
	   else{
	     char* ret = strstr(temp, search_str);
	     if(ret!=NULL)
	       {
		 loc1 = (location){i, (size_t)(ret - temp)};
		 return loc1;
	       }
	   }
	   
	 }


    }
  for(size_t i = 1; i <= document_size(editor->document); i++)
    {
      const char* temp = document_get_line(editor->document, i);
      char* ret = strstr(temp, search_str);
      if(ret != NULL)
	{
	 
	      loc1 = (location){i, (size_t)(ret - temp)};
	      //printf("loc3: line_no is %zu, idx is %zu\n", loc1.line_no, loc1.idx);
	      return loc1;
	   
	}

    }
  //printf("cannot find\n");
  return loc1;
}

void handle_merge_line(editor *editor, size_t line_no) {
    // TODO implement handle_merge_line
  if(line_no == document_size(editor->document)){
      return;
    }
  const char* line1 = document_get_line(editor->document, line_no);
  const char* line2 = document_get_line(editor->document, line_no + 1);
  char* buffer = malloc((strlen(line1) + strlen(line2))*sizeof(char) + 1);
  buffer[strlen(line1)] = '\0';
  strcpy(buffer, line1);
  strcat(buffer, line2);
  document_delete_line(editor->document, line_no + 1);
  document_set_line(editor->document, line_no, buffer);
  free(buffer);
}

void handle_split_line(editor *editor, location loc) {
    // TODO implement handle_split_line
  const char* line = document_get_line(editor->document, loc.line_no);
  char* newLine = malloc((strlen(line) - loc.idx) * sizeof(char) + 1);
  newLine[strlen(line) - loc.idx] = '\0';
  strcpy(newLine, line  + loc.idx);
  handle_insert_command(editor, (location){loc.line_no + 1, 0}, newLine);
  handle_delete_command(editor, loc, strlen(line) - loc.idx);
  free(newLine);
}

void handle_save_command(editor *editor) {
    // TODO implement handle_save_command
  document_write_to_file(editor->document, editor->filename);
}
