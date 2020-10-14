/**
 * \brief Handle utf8 symbols.
 *  
 * \author Miguel Oliveira e Silva - 2018
 */

#ifndef UTF8_H
#define UTF8_H


#define MAX_UTF8_STRING 4 // does not include terminator '\0'

int num_entries_utf8();
int all_names_size_utf8();
int num_chars_utf8(char* utf8);
char* code2utf8(char*res, int code);
int exists_name_utf8(char* name);
int get_code_utf8(char* name);
char* all_names_utf8(char* res, int max_length);

#endif

