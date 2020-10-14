/*!
 * \file
 * \brief Useful functions to construct textual boxes.
 *
 * \details 
 *  
 * \author Miguel Oliveira e Silva - 2018
 */

#ifndef BOX_H
#define BOX_H

#include <stdio.h>
#include <string.h>
#include <errno.h>

#define BOX_NORTH_DIRECTION 0x8
#define BOX_EAST_DIRECTION  0x4
#define BOX_WEST_DIRECTION  0x2
#define BOX_SOUTH_DIRECTION 0x1
#define BOX_NO_DIRECTION    0x0

#define BOX_UP_DIRECTION BOX_NORTH_DIRECTION
#define BOX_RIGHT_DIRECTION BOX_EAST_DIRECTION
#define BOX_LEFT_DIRECTION BOX_WEST_DIRECTION
#define BOX_DOWN_DIRECTION BOX_SOUTH_DIRECTION

void set_ascii_mode_boxes();
void set_utf8_mode_boxes();
int ascii_mode_boxes();
int utf8_mode_boxes();
void logger_filter_out_boxes();
char* gen_boxes(char* result, int max_length, const char* box, ...);
char* gen_rect(char* result, int max_length, int num_lines, int num_columns, int wall_mask, int round_corners);
int matrix_length(int num_lines, int num_columns, int rect_num_lines, int rect_num_columns);
char* gen_matrix(char* result, int max_length, int num_lines, int num_columns, int rect_num_lines, int rect_num_columns, int round_corners);
char* gen_empty_rect(char* result, int max_length, int num_lines, int num_columns);
int valid_direction(int direction);
char* gen_lines(char* result, int max_length, int direction, ...);
void box_dimensions(char* box, int* pnum_lins, int* pnum_cols);
char* gen_overlap_boxes(char* result, int max_length, char* first_box, ...);
char* progress_bar(char* text, int percentage, int num_cols, char* complete_char, char* incomplete_char);

#endif

