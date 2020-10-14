#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>
#include "thread.h"
#include "process.h"
#include "utils.h"
#include "logger.h"

pid_t launchProcessClient(int id);
static void* clientMain(void* arg);

static int thread_version = 1;
static int line_mode = 0;

#define DEFAULT_NUM_CLIENTS 2
#define DEFAULT_NUM_MESSAGES 10
static int num_clients = DEFAULT_NUM_CLIENTS;
static int num_messages = DEFAULT_NUM_MESSAGES;

static void help(char* prog);
static void processArgs(int argc, char* argv[]);

int main(int argc, char*argv[])
{
   processArgs(argc, argv);

   if (thread_version)
   {
      printf("Simple logger test: thread version\n");
      sleep(1);
      if (!initialized_logger())
         init_thread_logger();
   }
   else
   {
      printf("Simple logger test: process version\n");
      sleep(1);
      if (!initialized_logger())
         init_process_logger();
   }
   if (line_mode)
      set_line_mode_logger();
   else
      set_window_mode_logger();

   int* ids = (int*)alloca(sizeof(int)*num_clients);
   for(int i = 0; i < num_clients; i++)
   {
      char msg[1000];
      sprintf(msg, "Client %d", i+1);
      ids[i] = register_logger(strdup(msg), 1+i, 1+i, 1, 20, NULL);
   }

   launch_logger();

   if (thread_version)
   {
      pthread_t* clients = (pthread_t*)alloca(sizeof(pthread_t)*num_clients);
      for(int i = 0; i < num_clients; i++)
      {
         int* pid = (int*)mem_alloc(sizeof(int));
         *pid = ids[i];
         thread_create(clients+i, NULL, clientMain, (void*)pid);
      }
      for(int i = 0; i < num_clients; i++)
         thread_join(clients[i], NULL);
   }
   else
   {
      pid_t* clients = (pid_t*)alloca(sizeof(pid_t)*num_clients);
      for(int i = 0; i < num_clients; i++)
         clients[i] = launchProcessClient(ids[i]);
      int status;
      for(int i = 0; i < num_clients; i++)
         pwaitpid(clients[i], &status, 0);
   }
   term_logger();
   return 0;
}

pid_t launchProcessClient(int id)
{
   pid_t res = pfork();
   if (res == 0) // child -> client
   {
      int* pid = (int*)mem_alloc(sizeof(int));
      *pid = id;
      clientMain((void*)pid);
      exit(0);
   }
   return res;
}

static void* clientMain(void* arg)
{
   int id = *((int*)arg);
   srand(time(0)+getpid()+random_int(0,10000));
   char msg[1000];
   for(int i = 0; i < num_messages; i++)
   {
      usleep(random_int(100000, 1000000));
      sprintf(msg, "Message %d", i+1);
      send_log(id, msg);
   }
   sprintf(msg, "Client %d DONE.", id+1);
   send_log(id, msg);
   return NULL;
}

static void help(char* prog)
{
   assert (prog != NULL);

   printf("\n");
   printf("Usage: %s [OPTION] ...\n", prog);
   printf("\n");
   printf("Options:\n");
   printf("\n");
   printf("  -h,--help                                   show this help\n");
   printf("  -t,--thread-version\n");
   printf("  -p,--process-version\n");
   printf("  -l,--line-mode\n");
   printf("  -w,--window-mode (default)\n");
   printf("  -c,--num-clients <NUM>\n");
   printf("  -m,--num-messages <NUM>\n");
   printf("\n");
}

static void processArgs(int argc, char* argv[])
{
   assert(argc >= 0 && argv != NULL && argv[0] != NULL);

   static struct option long_options[] =
   {
      {"help",                         no_argument,       NULL, 'h'},
      {"--thread-version",             no_argument,       NULL, 't'},
      {"--process-version",            no_argument,       NULL, 'p'},
      {"--line-mode",                  no_argument,       NULL, 'l'},
      {"--window-mode",                no_argument,       NULL, 'w'},
      {"--num-clients",                required_argument, NULL, 'c'},
      {"--num-messages",               required_argument, NULL, 'm'},
      {0, 0, NULL, 0}
   };
   int op=0;

   while (op != -1)
   {
      int option_index = 0;

      op = getopt_long(argc, argv, "htplwc:m:", long_options, &option_index);
      int st;
      switch (op)
      {
         case -1:
            break;

         case 't':
            thread_version = 1;
            break;

         case 'p':
            thread_version = 0;
            break;

         case 'h':
            help(argv[0]);
            exit(EXIT_SUCCESS);

         case 'l':
            line_mode = 1;
            break;

         case 'w':
            line_mode = 0;
            break;

         case 'c':
            st = sscanf(optarg, "%d", &num_clients);
            if (st != 1 || num_clients < 1)
            {
               fprintf(stderr, "ERROR: invalid number of clients \"%s\"\n", optarg);
               exit(EXIT_FAILURE);
            }
            break;

         case 'm':
            st = sscanf(optarg, "%d", &num_messages);
            if (st != 1 || num_messages < 1)
            {
               fprintf(stderr, "ERROR: invalid number of messages \"%s\"\n", optarg);
               exit(EXIT_FAILURE);
            }
            break;

         default:
            help(argv[0]);
            exit(EXIT_FAILURE);
            break;
      }
   }

   if (optind < argc)
   {
      fprintf(stderr, "ERROR: invalid extra arguments\n");
      exit(EXIT_FAILURE);
   }
}

