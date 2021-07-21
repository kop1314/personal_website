/**
* Text Editor Lab
* CS 241 - Fall 2018
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "document.h"
#include "vector.h"

struct document {
    vector *vector;
};

document *document_create() {
    document *this = (document *)malloc(sizeof(document));
    assert(this);
    this->vector = vector_create(string_copy_constructor, string_destructor,
                                 string_default_constructor);
    return this;
}

void document_write_to_file(document *this, const char *path_to_file) {
    assert(this);
    assert(path_to_file);
    // see the comment in the header file for a description of how to do this!
    // TODO: your code here!
    FILE* file = fopen(path_to_file, "w");
    if(!file)
      {
	exit(1);
      }
    for(size_t i = 0; i < vector_size(this->vector); i++)
      {
	fprintf(file, "%s\n", vector_get(this->vector, i));
      }
    
    fclose(file);
}

document *document_create_from_file(const char *path_to_file) {
    assert(path_to_file);
    // this function will read a file which is created by document_write_to_file
    // TODO: your code here!
    document* d = document_create();
    //d->vector = string_vector_create();
    FILE* file = fopen(path_to_file, "r");
    if(!file)
      {
	return d;
      }
    char* buffer = NULL;
    size_t capacity = 0;
    size_t sizeOfLine;
    //printf("Before while loop\n");
    while((sizeOfLine = getline(&buffer, &capacity, file)) != (size_t)-1)
      {
	if(sizeOfLine > 0 && buffer[sizeOfLine - 1] == '\n')
	  {
	    buffer[sizeOfLine - 1] = 0;
	  }
	//char* temp = malloc(strlen(buffer) * sizeof(char) + 1);
	//temp[strlen(buffer)] = 0;
	//strcpy(temp, buffer);
	//printf("string is %s\n", buffer);
	vector_push_back(d->vector, buffer);
	//printf("I push a string\n");
      }
    free(buffer);
    fclose(file);
    return d;
}

void document_destroy(document *this) {
    assert(this);
    vector_destroy(this->vector);
    free(this);
}

size_t document_size(document *this) {
    assert(this);
    return vector_size(this->vector);
}

void document_set_line(document *this, size_t line_number, const char *str) {
    assert(this);
    assert(str);
    size_t index = line_number - 1;
    vector_set(this->vector, index, (void *)str);
}

const char *document_get_line(document *this, size_t line_number) {
    assert(this);
    assert(line_number > 0);
    size_t index = line_number - 1;
    return (const char *)vector_get(this->vector, index);
}

void document_insert_line(document *this, size_t line_number, const char *str) {
    assert(this);
    assert(str);
    // TODO: your code here!
    // How are you going to handle the case when the user wants to
    // insert a line past the end of the document?
    char* cpy = malloc(strlen(str) + 1);
    cpy[strlen(str)] = '\0';
    strcpy(cpy, str);
    if(line_number <= vector_size(this->vector) + 1)
      {
	vector_insert(this->vector, line_number - 1, cpy);
	free(cpy);
	return;
      }
    size_t index = vector_size(this->vector) + 1;
    while(index < line_number)
      {
	char* es = malloc(sizeof(char));
	es[0] = '\0';
	vector_push_back(this->vector, es);
	free(es);
	index++;
      }
    vector_insert(this->vector, line_number - 1, cpy);
    free(cpy);
}

void document_delete_line(document *this, size_t line_number) {
    assert(this);
    assert(line_number > 0);
    size_t index = line_number - 1;
    vector_erase(this->vector, index);
}
