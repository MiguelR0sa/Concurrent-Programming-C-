#include <stdio.h>
#include <stdlib.h>
 #include <string.h>
#include <assert.h>
#include "utf8.h"

void show(int code)
{
   char utf8seq[MAX_UTF8_STRING+1];
   code2utf8(utf8seq, code);
   printf("code 0x%x -> (1)\"%s\" (2)\"%s \" (3)\"%s  \"-> size: %d ->", code, utf8seq, utf8seq, utf8seq, num_chars_utf8(utf8seq));
   for(int j = 0; utf8seq[j]; j++)
      printf(" %x", 0xFF & utf8seq[j]);
   printf("\n");
}

int main(int argc, char* argv[])
{
   if (argc == 1 || (strcasecmp(argv[1], "code") != 0 && strcasecmp(argv[1], "name") != 0))
   {
      printf("Usage: test [code <code1> ... | name <name1> ...]\n");
      if (argc == 1)
         printf("All available names:\n%s", all_names_utf8(NULL, 0));
      return 1;
   }
   if (strcasecmp(argv[1], "code") == 0)
   {
      for(int i = 2; i < argc; i++)
         show((int)strtol(argv[i], NULL, 0));
   }
   else
   {
      for(int i = 2; i < argc; i++)
         if (exists_name_utf8(argv[i]))
            show(get_code_utf8(argv[i]));
   }

   return 0;
}

