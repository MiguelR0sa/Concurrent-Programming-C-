/**
 *  \brief DbC module.
 *
 * Why dbc.h instead of assert.h:
 *   - separates different assertion types;
 *   - implements both policies: EXCEPTION_POLICY and EXIT_POLICY (default).
 *   - augments assert with error message strings;
 *  
 * \author Miguel Oliveira e Silva - 2018
 */

#ifndef DBC_H
#define DBC_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "utils.h"

#ifdef NDEBUG

#define check(condition, message)
#define require(condition, message)
#define ensure(condition, message)
#define invariant(condition, message)

#else

#ifdef EXCEPTION_POLICY

#define check(condition, message) \
   if (!(condition)) \
      throw string_concat(NULL, 0, (char*)"Assertion fail", message[0] ? (char*)": " : (char*)"", message, (char*)", assertion: \"", #condition, (char*)"\", function: \"", __FUNCTION__, (char*)"\":", int2str(__LINE__), (char*)", file: \"", __FILE__, (char*)"\"\n", NULL)

#define require(condition, message) \
   if (!(condition)) \
      throw string_concat(NULL, 0, (char*)"Precondition fail", message[0] ? (char*)": " : (char*)"", message, (char*)", assertion: \"", #condition, (char*)"\", function: \"", __FUNCTION__, (char*)"\":", int2str(__LINE__), (char*)", file: \"", __FILE__, (char*)"\"\n", NULL)

#define ensure(condition, message) \
   if (!(condition)) \
      throw string_concat(NULL, 0, (char*)"Postcondition fail", message[0] ? (char*)": " : (char*)"", message, (char*)", assertion: \"", #condition, (char*)"\", function: \"", __FUNCTION__, (char*)"\":", int2str(__LINE__), (char*)", file: \"", __FILE__, (char*)"\"\n", NULL)

#define invariant(condition, message) \
   if (!(condition)) \
      throw string_concat(NULL, 0, (char*)"Invariant fail", message[0] ? (char*)": " : (char*)"", message, (char*)", assertion: \"", #condition, (char*)"\", function: \"", __FUNCTION__, (char*)"\":", int2str(__LINE__), (char*)", file: \"", __FILE__, (char*)"\"\n", NULL)

#else // EXIT_POLICY (default)

#define check(condition, message) \
   if (!(condition)) \
      do { \
         fprintf (stderr, "Assertion fail%s%s, assertion: \"%s\", function: \"%s\":%d, file: \"%s\"\n", \
                  message[0] ? ": " : "", message, #condition, __FUNCTION__, __LINE__ , __FILE__); \
         *((int*)0) = 0; \
         abort (); \
      } while (0)

#define require(condition, message) \
   if (!(condition)) \
      do { \
         fprintf (stderr, "Precondition fail%s%s, assertion: \"%s\", function: \"%s\":%d, file: \"%s\"\n", \
                  message[0] ? ": " : "", message, #condition, __FUNCTION__, __LINE__ , __FILE__); \
         *((int*)0) = 0; \
         abort (); \
      } while (0)

#define ensure(condition, message) \
   if (!(condition)) \
      do { \
         fprintf (stderr, "Postcondition fail%s%s, assertion: \"%s\", function: \"%s\":%d, file: \"%s\"\n", \
                  message[0] ? ": " : "", message, #condition, __FUNCTION__, __LINE__ , __FILE__); \
         *((int*)0) = 0; \
         abort (); \
      } while (0)

#define invariant(condition, message) \
   if (!(condition)) \
      do { \
         fprintf (stderr, "Invariant fail%s%s, assertion: \"%s\", function: \"%s\":%d, file: \"%s\"\n", \
                  message[0] ? ": " : "", message, #condition, __FUNCTION__, __LINE__ , __FILE__); \
         *((int*)0) = 0; \
         abort (); \
      } while (0)

#endif

#endif

#endif
