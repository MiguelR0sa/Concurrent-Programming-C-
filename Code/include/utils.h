/**
 *  \brief Useful common functions and macros.
 *  
 * \author Miguel Oliveira e Silva - 2017
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string.h>
#include <errno.h>

//#define EXCEPTION_POLICY
//#define EXIT_POLICY // DEFAULT

#ifdef EXCEPTION_POLICY
#define check_alloc(pnt) \
   if (pnt == NULL) \
      throw ENOMEM
#else
#define check_alloc(pnt) \
   if (pnt == NULL) \
      do { \
         fprintf (stderr, "%s at \"%s\":%d: %s\n", \
                  __FUNCTION__ , __FILE__, __LINE__, strerror (ENOMEM)); \
         *((int*)0) = 0; \
         abort (); \
      } while (0)
#endif

#define concat_2str(str1,str2) \
        ({ \
           char* __res__ = (char*)alloca(strlen(str1)+strlen(str2)+1); \
           strcpy(__res__, str1); \
           strcat(__res__, str2); \
           __res__; \
        })

#define concat_3str(str1,str2,str3) \
        ({ \
           char* __res__ = (char*)alloca(strlen(str1)+strlen(str2)+strlen(str3)+1); \
           strcpy(__res__, str1); \
           strcat(__res__, str2); \
           strcat(__res__, str3); \
           __res__; \
        })

#define concat_4str(str1,str2,str3,str4) \
        ({ \
           char* __res__ = (char*)alloca(strlen(str1)+strlen(str2)+strlen(str3)+strlen(str4)+1); \
           strcpy(__res__, str1); \
           strcat(__res__, str2); \
           strcat(__res__, str3); \
           strcat(__res__, str4); \
           __res__; \
        })

#define concat_5str(str1,str2,str3,str4,str5) \
        ({ \
           char* __res__ = (char*)alloca(strlen(str1)+strlen(str2)+strlen(str3)+strlen(str4)+strlen(str5)+1); \
           strcpy(__res__, str1); \
           strcat(__res__, str2); \
           strcat(__res__, str3); \
           strcat(__res__, str4); \
           strcat(__res__, str5); \
           __res__; \
        })

#define concat_6str(str1,str2,str3,str4,str5,str6) \
        ({ \
           char* __res__ = (char*)alloca(strlen(str1)+strlen(str2)+strlen(str3)+strlen(str4)+strlen(str5)+strlen(str6)+1); \
           strcpy(__res__, str1); \
           strcat(__res__, str2); \
           strcat(__res__, str3); \
           strcat(__res__, str4); \
           strcat(__res__, str5); \
           strcat(__res__, str6); \
           __res__; \
        })

#define concat_7str(str1,str2,str3,str4,str5,str6,str7) \
        ({ \
           char* __res__ = (char*)alloca(strlen(str1)+strlen(str2)+strlen(str3)+strlen(str4)+strlen(str5)+strlen(str6)+strlen(str7)+1); \
           strcpy(__res__, str1); \
           strcat(__res__, str2); \
           strcat(__res__, str3); \
           strcat(__res__, str4); \
           strcat(__res__, str5); \
           strcat(__res__, str6); \
           strcat(__res__, str7); \
           __res__; \
        })

#define concat_string_in_stack(str1,str2) \
        ({ \
           char* __res__ = (char*)alloca(strlen(str1)+strlen(str2)+1); \
           strcpy(__res__, str1); \
           strcat(__res__, str2); \
           __res__; \
        })

#define int2str(num) \
        ({ \
           char* __res__ = (char*)alloca(numDigits((int)num)+1); \
           sprintf(__res__, "%d", (int)num); \
           __res__; \
        })

#define int2nstr(num, len) \
        ({ \
           int d = numDigits((int)num); \
           if (len > d) \
              d = len; \
           char* __res__ = (char*)alloca(d+1); \
           sprintf(__res__, "%0*d", d, (int)num); \
           __res__; \
        })

#define perc2str(perc) \
        ({ \
           char* __res__ = (char*)alloca(4+1); \
           sprintf(__res__, "%3d%%", (int)perc); \
           __res__; \
        })

#define length_vargs_string_list(first) \
        ({ \
           int __res__ = 0; \
           va_list ap; \
           va_start(ap, first); \
           char* t = first; \
           while (t != NULL) \
           { \
              __res__ += strlen(t); \
              t = va_arg(ap, char*); \
           } \
           va_end(ap); \
           __res__; \
        })

void* mem_alloc(int size);
void mem_free(void* ptr);
char* string_clone(char* str);
int string_num_lines(char* text);
int string_num_columns(char* text);
int string_count_char(char* text, char* ch);
int string_starts_with(char* text, char* prefix);
int string_ends_with(char* text, char* suffix);
char* string_concat(char* result, int max_length, char* text, ...);
int random_boolean(int trueProb);
int random_int(int min, int max);
char* random_string(char** list, int* used, int length);
void clear_console();
void move_cursor(int line,int column);
void hide_cursor();
void show_cursor();
int string_list_length(char** list);
char** string_list_clone(char** list);
char** string_list_free(char** list);
int numDigits(int num);
char* int2nstring(char* res, int num, int len);
char* percentage2string(char* res, int perc);

#endif
