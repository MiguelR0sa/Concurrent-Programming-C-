#include <stdlib.h>
#include "dbc.h"
#include "global.h"
#include "utils.h"
#include "box.h"
#include "timer.h"
#include "logger.h"
#include "barber-shop.h"
#include "barber.h"
#include "service.h"


enum State
{
   NONE = 0,
   CUTTING,
   SHAVING,
   WASHING,
   WAITING_CLIENTS,
   WAITING_BARBER_SEAT,
   WAITING_WASHBASIN,
   REQ_SCISSOR,
   REQ_COMB,
   REQ_RAZOR,
   DONE
};

#define State_SIZE (DONE - NONE + 1)

static const char* stateText[State_SIZE] =
{
   "---------",
   "CUTTING  ",
   "SHAVING  ",
   "WASHING  ",
   "W CLIENT ", // Waiting for client
   "W SEAT   ", // Waiting for barber seat
   "W BASIN  ", // Waiting for washbasin
   "R SCISSOR", // Request a scissor
   "R COMB   ", // Request a comb
   "R RAZOR  ", // Request a razor
   "DONE     ",
};

static const char* skel = 
   "@---+---+---@\n"
   "|B##|C##|###|\n"
   "+---+---+-+-+\n"
   "|#########|#|\n"
   "@---------+-@";
static int skel_length = num_lines_barber()*(num_columns_barber()+1)*4; // extra space for (pessimistic) utf8 encoding!

static void life(Barber* barber);
void changeClientState(Barber *barber,int newState);
static void sit_in_barber_bench(Barber* barber);
static void wait_for_client(Barber* barber);
static int work_available(Barber* barber);
static void rise_from_barber_bench(Barber* barber);
static void process_resquests_from_client(Barber* barber);
static void release_client(Barber* barber);
static void done(Barber* barber);
static void process_haircut_request(Barber* barber);
static void process_hairwash_request(Barber* barber);

static char* to_string_barber(Barber* barber);

size_t sizeof_barber()
{
   return sizeof(Barber);
}

int num_lines_barber()
{
   return string_num_lines((char*)skel);
}

int num_columns_barber()
{
   return string_num_columns((char*)skel);
}

void init_barber(Barber* barber, int id, BarberShop* shop, int line, int column)
{
   require (barber != NULL, "barber argument required");
   require (id > 0, concat_3str("invalid id (", int2str(id), ")"));
   require (shop != NULL, "barber shop argument required");
   require (line >= 0, concat_3str("Invalid line (", int2str(line), ")"));
   require (column >= 0, concat_3str("Invalid column (", int2str(column), ")"));

   barber->id = id;
   barber->state = NONE;
   barber->shop = shop;
   barber->clientID = 0;
   barber->reqToDo = 0;
   barber->benchPosition = -1;
   barber->chairPosition = -1;
   barber->basinPosition = -1;
   barber->tools = 0;
   barber->internal = NULL;
   barber->logId = register_logger((char*)("Barber:"), line ,column,
                                   num_lines_barber(), num_columns_barber(), NULL);
}

void term_barber(Barber* barber)
{
   require (barber != NULL, "barber argument required");

   if (barber->internal != NULL)
   {
      mem_free(barber->internal);
      barber->internal = NULL;
   }
}

void log_barber(Barber* barber)
{
   require (barber != NULL, "barber argument required");

   spend(random_int(global->MIN_VITALITY_TIME_UNITS, global->MAX_VITALITY_TIME_UNITS));
   send_log(barber->logId, to_string_barber(barber));
}

void* main_barber(void* args)
{
   Barber* barber = (Barber*)args;
   require (barber != NULL, "barber argument required");
   
   life(barber);
   return NULL;
}

static void life(Barber* barber)
{
   require (barber != NULL, "barber argument required");
   printf("ola\n");
   barber->shop=shopConnect();
   printf("connected\n");
   sit_in_barber_bench(barber);
   printf("seated1\n"); 
    wait_for_client(barber);
   printf("waiting\n");
   while(work_available(barber)) // no more possible clients and closes barbershop
   {  printf("there is work\n");
      rise_from_barber_bench(barber);
      printf("standing %d\n",barber->benchPosition);
      process_resquests_from_client(barber);
      printf("processed\n");
      release_client(barber);
   //   printf("released\n");
      sit_in_barber_bench(barber);
      printf("seated\n");
	printf("dudes vivos%d\n",barber->shop->livingClients);      
	wait_for_client(barber);
      printf("waiting\n");
	
   }
   printf("im here\n");
   done(barber);
   printf("finished\n");
   shopDisconnect(barber->shop);
}

static void sit_in_barber_bench(Barber* barber)
{
   /** TODO:
    * 1: sit in a random empty seat in barber bench (always available)
    **/

   	require (barber != NULL, "barber argument required");
   	require (num_seats_available_barber_bench(barber_bench(barber->shop)) > 0, "seat not available in barber shop");
   	require (!seated_in_barber_bench(barber_bench(barber->shop), barber->id), "barber already seated in barber shop");

   	//1
   	barber -> benchPosition = random_sit_in_barber_bench(&(barber -> shop -> barberBench), barber -> id);
      
   	log_barber(barber);
}

static void wait_for_client(Barber* barber)
{
   /** TODO:
    * 1: set the client state to WAITING_CLIENTS
    * 2: get next client from client benches (if empty, wait) (also, it may be required to check for simulation termination)
    * 3: receive and greet client (receive its requested services, and give back the barber's id)
    **/
	RQItem clientid;
	require (barber != NULL, "barber argument required");

	//1 
	barber->state = WAITING_CLIENTS;

	//2    
 	if(barber->shop->numClientBenchesSeats>0) 
  	barber->state = WAITING_BARBER_SEAT;

    //	if(no_more_clients(&(barber -> shop -> clientBenches))==1)	
        if(barber->shop->livingClients==0)	
        {	
	barber -> state = DONE;
        return;
  	}
	sem_down(CLIENTS_SEATED); //menos um moço sentado

	clientid = next_client_in_benches(&(barber -> shop -> clientBenches));
  barber->clientID=clientid.clientID;
  // barber->reqToDo=clientid.request;
       

	//3
	receive_and_greet_client(barber -> shop,barber -> id, barber->clientID);
	semIDaccess_up(barber->clientID);  
  log_barber(barber);  // (if necessary) more than one in proper places!!!
}

static int work_available(Barber* barber)
{
   /** TODO:
    * 1: find a safe way to solve the problem of barber termination
    **/

  require (barber != NULL, "barber argument required");

  //1
  if(barber->state==DONE)
   return 0;
  if(barber->shop->numClientsInside>0)
  {
   
    return 1;
  }

	return 0;
}

static void rise_from_barber_bench(Barber* barber)
{
   /** TODO:
    * 1: rise from the seat of barber bench
    **/

  require (barber != NULL, "barber argument required");
  require (seated_in_barber_bench(barber_bench(barber->shop), barber->id), "barber not seated in barber shop");

  //1
  //sem_down(BARBERBENCHES);
	rise_barber_bench(&(barber -> shop -> barberBench), barber -> benchPosition );

  log_barber(barber);
}

static void process_resquests_from_client(Barber* barber)
{
   /** TODO:
    * Process one client request at a time, until all requests are fulfilled.
    * For each request:
    * 1: select the request to process (any order is acceptable)
    * 2: reserve the chair/basin for the service (setting the barber's state accordingly) 
    *    2.1: set the client state to a proper value
    *    2.2: reserve a random empty chair/basin 
    *    2.2: inform client on the service to be performed
    * 3: depending on the service, grab the necessary tools from the pot (if any)
    * 4: process the service (see [incomplete] process_haircut_request as an example)
    * 3: return the used tools to the pot (if any)
    *
    *
    * At the end the client must leave the barber shop
    **/
  
   	require (barber != NULL, "barber argument required");

   	Service services;
   	RQItem it;
   	//1	

 //   sem_down(CLIENTS_SEATED);
   	it = next_client_in_benches(&(barber -> shop -> clientBenches));
   	barber -> reqToDo = it.request * WASH_HAIR_REQ;

sem_up(FREE_CLIENT_BENCHES);
   	//2
   	//2.1
   	changeClientState(barber,9);
   	//2.2

    sem_down(FREE_WASHBASINS);
   	barber -> basinPosition =  reserve_random_empty_washbasin(barber -> shop,barber -> id);
   	services.barberChair = 0;
   	services.washbasin = 1;
   	services.barberID = barber -> id;
   	services.clientID = it.clientID;
   	services.pos = barber -> basinPosition;
   	services.request = it.request;
	

   	inform_client_on_service(barber -> shop, services, barber -> id);
        semIDaccess_up(barber->clientID);  

	//3
   	//se for necessario tools set_tools_barber_chair(BarberChair* chair, int tools);
   	//4
   	process_hairwash_request(barber);
	 //3 return tool
	sem_up(FREE_WASHBASINS);
        semWorkingInWashbasins_up(barber->basinPosition);
   	//at the end
        semIDaccess_down(barber->clientID);
	printf("arrived\n");
   	release_client(barber);
        
   	log_barber(barber);  // (if necessary) more than one in proper places!!!
}

static void release_client(Barber* barber)
{
   /** TODO:
    * 1: notify client the all the services are done
    **/

   require (barber != NULL, "barber argument required");
   
   if(barber -> shop -> clientStates[barber -> clientID-1] == 10) client_done(barber -> shop,barber -> clientID);
   int h= washbasin_service_finished(&(barber->shop->washbasin[barber->basinPosition]));

   log_barber(barber);
}

static void done(Barber* barber)
{
   /** TODO:
    * 1: set the client state to DONE
    **/
   require (barber != NULL, "barber argument required");

   //1
   barber -> shop -> clientStates[barber -> clientID-1] = 10;

   log_barber(barber);
}

static void process_haircut_request(Barber* barber)
{
   /** TODO:
    * ([incomplete] example code for task completion algorithm)
    **/

   require (barber != NULL, "barber argument required");
   require (barber->tools & SCISSOR_TOOL, "barber not holding a scissor");
   require (barber->tools & COMB_TOOL, "barber not holding a comb");

   int steps = random_int(5,20);
   int slice = (global->MAX_WORK_TIME_UNITS-global->MIN_WORK_TIME_UNITS+steps)/steps;
   int complete = 0;
   while(complete < 100)
   {
      spend(slice);
      complete += 100/steps;
      if (complete > 100)
         complete = 100;
      set_completion_barber_chair(barber_chair(barber->shop, barber->chairPosition), complete);
   }

   log_barber(barber);  // (if necessary) more than one in proper places!!!
}

static void process_hairwash_request(Barber* barber)
{
   /** TODO:
    * ([incomplete] example code for task completion algorithm)
    **/

   require (barber != NULL, "barber argument required");
  

   int steps = random_int(5,20);
   int slice = (global->MAX_WORK_TIME_UNITS-global->MIN_WORK_TIME_UNITS+steps)/steps;
   int complete = 0;
   while(complete < 100)
   {
      spend(slice);
      complete += 100/steps;
      if (complete > 100)
         complete = 100;
      set_completion_washbasin(&(barber->shop->washbasin[barber->basinPosition]), complete);
     log_barber(barber);   
   }
    set_completion_washbasin(&(barber->shop->washbasin[barber->basinPosition]), 100);
	printf("complete");
   log_barber(barber);  // (if necessary) more than one in proper places!!!
   
}


static char* to_string_barber(Barber* barber)
{
   require (barber != NULL, "barber argument required");

   if (barber->internal == NULL)
      barber->internal = (char*)mem_alloc(skel_length + 1);

   char tools[4];
   tools[0] = (barber->tools & SCISSOR_TOOL) ? 'S' : '-',
      tools[1] = (barber->tools & COMB_TOOL) ?    'C' : '-',
      tools[2] = (barber->tools & RAZOR_TOOL) ?   'R' : '-',
      tools[3] = '\0';

   char* pos = (char*)"-";
   if (barber->chairPosition >= 0)
      pos = int2nstr(barber->chairPosition+1, 1);
   else if (barber->basinPosition >= 0)
      pos = int2nstr(barber->basinPosition+1, 1);

   return gen_boxes(barber->internal, skel_length, skel,
         int2nstr(barber->id, 2),
         barber->clientID > 0 ? int2nstr(barber->clientID, 2) : "--",
         tools, stateText[barber->state], pos);
}
void changeClientState(Barber *barber,int newState)
{	
	sem_down(ACCESS);
	barber->shop->clientStates[barber->clientID-1]=newState;
	sem_up(ACCESS);
}

