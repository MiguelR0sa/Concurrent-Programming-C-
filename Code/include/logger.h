/*!
 * \file
 * \brief An extended log module
 *
 * \details This module allows a more versatile visualization of log information.
 * A log information is a text string, but its visualization depends on the module's mode:
 *  - It contains the usual line mode, in which each log corresponds to the output of a console line;
 *  - A more powerful window mode is also available, in which each registered log type is attached to
 *     a rectangular window in the console (that should be in a visible portion of the console).
 *
 * Any UTF8 character, and terminal color character sequence are accepted.
 * In the window mode, the visualization of the message is restricted to its registered window
 * (a new-line <code>'\\n'</code> character represents a new line within that window).
 * This module allows also the possibility to filter out (window mode) characters to a more textual and
 * simple line mode log line_mode_logger().
 *
 * The usage of this model requires the following steps:
 *
 * 0. setting line/window mode (default is window mode)
 *    - <code>set_line_mode_logger</code>, <code>set_window_mode_logger</code>
 * 1. initialization (thread or process version);
 *    - <code>init_thread_logger</code>, <code>init_process_logger</code>
 * 2. registration of log type messages;
 *    - <code>register_logger</code>, <code>add2filterout_logger(char**)</code>
 * 3. launch logger (thread/process)
 *    - <code>launch_logger</code>
 * 4. use (as a normal logger)
 *    - <code>send_log</code>
 * 5. termination
 *    - <code>term_logger</code>
 *  
 * \author Miguel Oliveira e Silva - 2017-2018
 */

#ifndef LOGGER_H
#define LOGGER_H


/**
 * \brief Is logger initialized?
 *
 * \return true if initialized, false otherwise
 */
int initialized_logger();

/**
 * \brief thread logger
 *
 * \return true on thread version, false otherwise
 */
int thread_logger();

/**
 * \brief process logger
 *
 * \return true on process version, false otherwise
 */
int process_logger();

/**
 * \brief Initialize thread version of logger
 */
void init_thread_logger();

/**
 * \brief Initialize process version of logger
 */
void init_process_logger();

/**
 * \brief Terminate logger execution
 */
void term_logger();


/**
 * In line mode all "logging" is filtered and presented line by line
 */
int line_mode_logger();
int window_mode_logger();
void set_line_mode_logger();
void set_window_mode_logger();


void add2filterout_logger(char** remove);

void launch_logger();

int get_num_lines_logger(int logId);
int get_num_columns_logger(int logId);

int valid_line_mode_translation_logger(char** lineModeTranslations);
int register_logger(char* name, int line, int column, int numLines, int numColumns, char** lineModeTranslations);

void send_log(int logId, char* text);

int valid_logId(int logId);
int get_line_logger(int logId);
int get_column_logger(int logId);

#endif

