
/**
* Text Editor Lab
* CS 241 - Fall 2018
*/

#include "document.h"
#include "editor.h"
#include "format.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * You can programatically test your text editor.
*/
int main() {
    // Setting up a docment based on the file named 'filename'.
    char *filename = "test.txt";
    document *doc = document_create_from_file(filename);
    (void)doc;
    //document_write_to_file(doc, "test2.txt");
    /*document_insert_line(doc, 1, "1");
    document_insert_line(doc, 5, "5");
    document_insert_line(doc, 12, "12");
    document_insert_line(doc, 15, "15");
    document_write_to_file(doc, "test2.txt");
    */
    //document_destroy(doc);

    editor* e = malloc(sizeof(editor));
    e->document = doc;
    e->string = NULL;
    e->filename = NULL;

    handle_display_command(e, 2, 8, 1, 3);

    //test insert
    handle_insert_command(e, (location){1, 0}, "1");
    handle_insert_command(e, (location){2, 5}, "World!");
    handle_insert_command(e, (location){11, 0}, "11");
    handle_insert_command(e, (location){13, 0}, "13");
    handle_insert_command(e, (location){3, 1}, " 4 ");

    handle_display_command(e, 1, -1, 0, -1);

    //test append
    handle_append_command(e, 1, " World");
    handle_append_command(e, 1, "\\nW\\h\\a\\t's up\\n babe?\\");
    handle_append_command(e, 8, "\\\\n");
    handle_display_command(e, 1, -1, 0, -1);
    

    //test write
    /*handle_write_command(e, 15, "\\nwhat \\i\\s up \\nbabe?\\");

    handle_display_command(e, 1, -1, 0, -1);
    */
    //test delete char
    /* handle_delete_command(e, (location){2, 5}, 6);
    handle_delete_command(e, (location){11, 0}, 20);
    handle_delete_command(e, (location){3, 1}, 3);

    handle_delete_line(e, 11);
    handle_delete_line(e, 12);
    handle_delete_line(e, 13);
    
    handle_display_command(e, 1, -1, 0, -1);
    */
    //test search
    editor* e2 = malloc(sizeof(editor));
    e2 -> document = document_create_from_file("test3.txt");
    e2 -> string = NULL;
    e2 -> filename = "save.txt";
    handle_display_command(e2, 1, -1, 0, -1);
    location l = handle_search_command(e2, (location){1, 0}, "ana");
    printf("line_no is %zu idx is %zu\n", l.line_no, l.idx);
    
    //test merge
    /* handle_merge_line(e2, 2);
    handle_merge_line(e2, 1);

    handle_display_command(e2, 1, -1, 0, -1);
    
    handle_split_line(e2, (location){1, 40});
    handle_split_line(e2, (location){2, 36});
    handle_display_command(e2, 1, -1, 0, -1);

    handle_save_command(e2);*/
    handle_cleanup(e);
    free(e);
    handle_cleanup(e2);
    free(e2);
}
