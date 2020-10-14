#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "utf8.h"
#include "box.h"

#define MAX 10000

int main()
{
   char text[MAX+1];

   printf("\n");
   printf("gen_boxes:\n");
   printf("%s\n", gen_boxes(text, MAX,                    // destination string
                            "  @--------@\n"              // BOX definition
                            "  |Philo ##|\n"              // first argument with 2 positions
                            "@-+--------+-@\n"
                            "|#|########|#|\n"            // arguments 2, 3 and 4
                            "@-+--------+-@",
                            "#1", "F", "THINKING", "F"));
   printf("\n");
   printf("%s\n", code2utf8(NULL, get_code_utf8((char*)"SCISSOR")));
   printf("%s\n", gen_boxes(text, MAX,
                            "  +--+--+--+\n"
                            "  |##|##|##|\n"
                            "  |##|##|##|\n"
                            "  +--+--+--+\n"
                            "     |##|   \n"
                            "  +--+--+--+\n"
                            "     +  +   \n"
                            "   +      + \n",
                            "1", code2utf8(NULL, get_code_utf8((char*)"SCISSOR")), "33", "4", "55", "66", ""));

   set_ascii_mode_boxes();
   printf("%s\n", gen_boxes(text, MAX,                    // destination string
                            "  +--------+\n"              // BOX definition
                            "  |Philo ##|\n"              // first argument with 2 positions
                            "+-+--------+-+\n"
                            "|#|########|#|\n"            // arguments 2, 3 and 4
                            "+-+--------+-+",
                            "#1", "F", "THINKING", "F"));
   printf("\n");
   set_utf8_mode_boxes();
   printf("<press ENTER>\n");
   getchar();
   printf("%s\n", gen_boxes(text, MAX,
                            "................\n"
                            ".+--+.+--+.+--+.\n"
                            ".|  |.|  |.|  |.\n"
                            ".+--+.+--+.+--+.\n"
                            "................\n" // "\n" also works!
                            ".+--+.+--+.+--+.\n"
                            ".|  |.|  |.|  |.\n"
                            ".+--+.+--+.+--+.\n"
                            "................\n"
                            ));
   printf("\n");
   printf("<press ENTER>\n");
   getchar();
   printf("\n");

   printf("gen_rect:\n");
   printf("%s\n", gen_boxes(text, MAX, gen_rect(NULL, 0, 4, 8, 0xF)));
   printf("\n");
   /*
   char* r = gen_rect(NULL, 0, 2, 20, 0xF);
   r[0] = r[19] = r[21] = r[40] = '@';
   printf("%s\n", gen_boxes(text, MAX, r));
   */
   printf("%s\n", gen_boxes(text, MAX, gen_round_rect(NULL, 0, 2, 20, 0xF)));
   printf("\n");
   printf("%s\n", gen_boxes(text, MAX, gen_rect(NULL, 0, 3, 10, 0x7)));
   printf("\n");
   printf("<press ENTER>\n");
   getchar();
   printf("\n");

   printf("gen_lines:\n");
   printf("%s\n", gen_boxes(text, MAX,
                            gen_lines(NULL, 0,
                                      BOX_LEFT_DIRECTION, 8, BOX_UP_DIRECTION, 10,
                                      BOX_RIGHT_DIRECTION, 14, BOX_DOWN_DIRECTION, 10,
                                      BOX_LEFT_DIRECTION, 2, BOX_NO_DIRECTION)));
   printf("\n");
   printf("<press ENTER>\n");
   getchar();
   printf("\n");

   printf("gen_overlap_boxes:\n");
   int nlins = 15;
   int ncols = 30;
   char rect[nlins*(ncols+1)+1];
   gen_rect(rect, nlins*41, nlins, ncols, 0xF);
   printf("%s\n", gen_boxes(text, MAX,
                            gen_overlap_boxes(NULL, 0, rect,
                                              gen_rect(NULL, 0, 4, 4, 0xE), 2, 2,
                                              gen_rect(NULL, 0, 4, 4, 0x7), 2, 9,
                                              gen_empty_rect(NULL, 0, 1, 5), 0, 3,
                                              "+", 0, 2, "+", 0, 7,
                                              gen_empty_rect(NULL, 0, 1, 5), nlins-1, 3,
                                              NULL)));
   printf("%s\n", gen_boxes(text, MAX,
                            gen_overlap_boxes(rect, 0, rect, 
                                              gen_rect(NULL, 0, 4, 4, 0xE), 2, 2,
                                              gen_rect(NULL, 0, 4, 4, 0x7), 2, 9,
                                              gen_empty_rect(NULL, 0, 1, 5), 0, 3,
                                              "+", 0, 2, "+", 0, 7,
                                              gen_empty_rect(NULL, 0, 1, 5), nlins-1, 3,
                                              NULL)));
   printf("\n");
   printf("<press ENTER>\n");
   getchar();
//#define COMPLETE   code2utf8(NULL, get_code_utf8((char*)"BLACK-SMALL-SQUARE"))
//#define INCOMPLETE code2utf8(NULL, get_code_utf8((char*)"WHITE-SMALL-SQUARE"))
#define COMPLETE   code2utf8(NULL, get_code_utf8((char*)"MEDIUM-BLACK-CIRCLE"))
#define INCOMPLETE code2utf8(NULL, get_code_utf8((char*)"MEDIUM-WHITE-CIRCLE"))

   printf("%s\n", progress_bar(NULL, 60, 10, COMPLETE, INCOMPLETE));

   printf("\n");
   printf("<press ENTER>\n");
   getchar();
   printf("gen_matrix:\n");
   printf("%s\n", gen_boxes(text, MAX, gen_matrix(NULL, 0, 4, 4, 3,5)));
   printf("\n");
   printf("%s\n", gen_boxes(text, MAX, gen_matrix(NULL, 0, 1, 7, 2,4)));
   printf("\n");

   return 0;
}

