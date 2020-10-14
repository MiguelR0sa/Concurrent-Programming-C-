#include <stdlib.h>
#include "dbc.h"
#include "global.h"
#include "utils.h"
#include "box.h"
#include "timer.h"
#include "logger.h"
#include "service.h"
#include "client.h"

enum ClientState
{
   NONE = 0,                   // initial state
   WANDERING_OUTSIDE,          // client outside barshop doing (important) things
   WAITING_BARBERSHOP_VACANCY, // a place to sit in the barber shop clients benches
   SELECTING_REQUESTS,         // choosing barber shop services
   WAITING_ITS_TURN,           // waiting for a barber assignment
   WAITING_SERVICE,            // waiting service selection (includes seat id)
   WAITING_SERVICE_START,      // client already seated in chair/basin waiting service start
   HAVING_A_HAIRCUT,           // haircut in progress
   HAVING_A_SHAVE,             // shave in progress
   HAVING_A_HAIR_WASH,         // hair wash in progress
   DONE                        // final state
};

#define State_SIZE (DONE - NONE + 1)

static const char* stateText[State_SIZE] =
{
   "---------",
   "WANDERING",
   "W VACANCY",
   "REQUESTS ",
   "Wait TURN",
   "Wt SERVCE",
   "SERV STRT",
   "HAIRCUT  ",
   "SHAVE    ",
   "HAIR WASH",
   "DONE     ",
};

static const char* skel =
   "@---+---+---@\n"
   "|C##|B##|###|\n"
   "+---+---+-+-+\n"
   "|#########|#|\n"
   "@---------+-@";
static int skel_length = num_lines_client()*(num_columns_client()+1)*4; // extra space for (pessimistic) utf8 encoding!

static void log_client(Client* client);

static void life(Client* client);
void changeState(Client *client,int newState);
static void notify_client_birth(Client* client);
static void notify_client_death(Client* client);
static void wandering_outside(Client* client);
static int vacancy_in_barber_shop(Client* client);
static void select_requests(Client* client);
static void wait_its_turn(Client* client);
static void rise_from_client_benches(Client* client);
static void wait_all_services_done(Client* client);

static char* to_string_client(Client* client);

size_t sizeof_client()
{
   return sizeof(Client);
}

int num_lines_client()
{
   return string_num_lines((char*)skel);
}

int num_columns_client()
{
   return string_num_columns((char*)skel);
}

void init_client(Client* client, int id, BarberShop* shop, int num_trips_to_barber, int line, int column)
{
   require (client != NULL, "client argument required");
   require (id > 0, concat_3str("invalid id (", int2str(id), ")"));
   require (shop != NULL, "barber shop argument required");
   require (num_trips_to_barber > 0, concat_3str("invalid number of trips to barber (", int2str(num_trips_to_barber), ")"));
   require (line >= 0, concat_3str("Invalid line (", int2str(line), ")"));
   require (column >= 0, concat_3str("Invalid column (", int2str(column), ")"));


   client->id = id;
   client->state = NONE;
   client->shop = shop;
   client->barberID = 0;
   client->num_trips_to_barber = 1;//num_trips_to_barber;
   client->requests = 0;
   client->benchesPosition = -1;
   client->chairPosition = -1;
   client->basinPosition = -1;
   client->internal = NULL;
   client->logId = register_logger((char*)("Client: "), line ,column,
                                   num_lines_client(), num_columns_client(), NULL);
   log_client(client);
}

void term_client(Client* client)
{
   require (client != NULL, "client argument required");

   if (client->internal != NULL)
   {
      mem_free(client->internal);
      client->internal = NULL;
   }
}

void log_client(Client* client)
{
   require (client != NULL, "client argument required");

   spend(random_int(global->MIN_VITALITY_TIME_UNITS, global->MAX_VITALITY_TIME_UNITS));
   send_log(client->logId, to_string_client(client));
}

void* main_client(void* args) // alterar -> attach;sincronizar barbershop com mem partilhada
{
   Client* client = (Client*)args;
   require (client != NULL, "client argument required");
   life(client);
   return NULL;
}

static void life(Client* client)
{
   require (client != NULL, "client argument required");

   int i = 0;
   notify_client_birth(client);
   printf("vivo\n");

   while(i < client->num_trips_to_barber)
   {
      wandering_outside(client);
      printf("lá fore\n");
      if (vacancy_in_barber_shop(client))
      {
 //	printf("entrou\n");
         select_requests(client);
//	printf("escolheu\n");
         wait_its_turn(client);
//	printf("esperou\n");
         rise_from_client_benches(client);
	printf("levantou\n");    
     wait_all_services_done(client);
         i++;
     printf("acabou\n");
      }
   }
  printf("quase morrrrrrrreu\n");
   notify_client_death(client);
   printf("morrrrrrrreu\n");
}

static void notify_client_birth(Client* client)
{
   require (client != NULL, "client argument required");

   // TODO:
    // 1: (is necessary) inform simulation that a new client begins its existence.
      client-> shop = shopConnect( );

      // >>>>> SEMAPHORE
      client->shop->livingClients++;
      /////////////////////////

    //

   log_client(client);
}

static void notify_client_death(Client* client)
{
   require (client != NULL, "client argument required");

   // TODO:
      // 1: (if necessary) inform simulation that a new client ceases its existence.
	client->shop->livingClients--;   
         printf("gajos vivos%d\n",client->shop->livingClients);
	shopDisconnect( client->shop );
  //
   
   log_client(client);
}

static void wandering_outside(Client* client)
{
    require (client != NULL, "client argument required");

    // TODO:
      // 1: set the client state to WANDERING_OUTSIDE
        //  client->state = WANDERING_OUTSIDE;
       changeState(client,WANDERING_OUTSIDE);
      // 2. random a time interval [global->MIN_OUTSIDE_TIME_UNITS, global->MAX_OUTSIDE_TIME_UNITS]
   
          spend(random_int(global->MIN_OUTSIDE_TIME_UNITS, global->MAX_OUTSIDE_TIME_UNITS));
    //

   log_client(client);
}

static int vacancy_in_barber_shop(Client* client)
{
    require (client != NULL, "client argument required");
    /* int res = 0;
    */

    // TODO:
      // 1: set the client state to WAITING_BARBERSHOP_VACANCY
         // client->state = WAITING_BARBERSHOP_VACANCY;
        changeState(client,WAITING_BARBERSHOP_VACANCY);

      // 2: check if there is an empty seat in the client benches (at this instante, later on it may fail)

          // >>>>> SEMAPHORE
          sem_down(FREE_CLIENT_BENCHES);
          /////////////////////////

          int res = num_available_benches_seats( &(client->shop->clientBenches) );
          //int client_benches_not_empty(BarberShop* shop);
    //

    log_client(client);
    return res;
}

static void select_requests(Client* client)
{
    require (client != NULL, "client argument required");

    // TODO:
        // 1: set the client state to SELECTING_REQUESTS
           //  client->state = SELECTING_REQUESTS;
         changeState(client,SELECTING_REQUESTS);

        // 2: choose a random comnination of requests
            // ?????????????????

            //int req[3] = HAIRCUT_REQ, SHAVE_REQ, WASH_HAIR_REQ;
            //client->requests = req[random_int(0,3)];

            client->requests = WASH_HAIR_REQ;

            // ?????????????????
    //

    log_client(client);
}

static void wait_its_turn(Client* client)
{
    require (client != NULL, "client argument required");

    // TODO:
      // 1: set the client state to WAITING_ITS_TURN
//	          client->state = WAITING_ITS_TURN;
      changeState(client,WAITING_ITS_TURN);

      // 2: enter barbershop (if necessary waiting for an empty seat)
          enter_barber_shop(client->shop, client->id, client->requests);

          // >>>>> SEMAPHORE

          sem_down(FREE_CLIENT_BENCHES);
          /////////////////////////

        client->benchesPosition=  random_sit_in_client_benches(&(client -> shop -> clientBenches), client -> id, client-> requests);

      // 3. "handshake" with assigned barber (greet_barber)
          //printf("client clients inside= %d \n",client->shop->numClientsInside);
          //printf("client semid= %d \n",client->shop->semID);

          // >>>>> SEMAPHORE
          sem_up(CLIENTS_SEATED);
	  semIDaccess_down(client->id);
          /////////////////////////
          //psem_up(client->shop->semID,CLIENTS); //mais um moço sentado

          client->barberID = greet_barber( client->shop, client->id); // returns barberID
        
     

      //

     log_client(client);
}

static void rise_from_client_benches(Client* client)
{
    require (client != NULL, "client argument required");
    require (client != NULL, "client argument required");
    require (seated_in_client_benches(client_benches(client->shop), client->id), concat_3str("client ",int2str(client->id)," not seated in benches"));

    // TODO:
      // 1: (exactly what the name says)
          rise_client_benches( &(client->shop->clientBenches), client->benchesPosition, client->id);

          // >>>>> SEMAPHORE
          sem_up(FREE_CLIENT_BENCHES);
          /////////////////////////
    //

   log_client(client);
}

static void wait_all_services_done(Client* client)
{
    require (client != NULL, "client argument required");

    // TODO:
      // Expect the realization of one request at a time, until all requests are fulfilled.
      // For each request:
        // 1: set the client state to WAITING_SERVICE
//            client->state = WAITING_SERVICE;
            changeState(client, WAITING_SERVICE);

        // 2: wait_service_from_barber from barbershop
	   semIDaccess_down(client->id);
           Service smt = wait_service_from_barber(client->shop, client-> barberID);
             client->basinPosition=smt.pos;

        // 3: set the client state to WAITING_SERVICE_START
            //client->state = WAITING_SERVICE_START;
	changeState(client, WAITING_SERVICE_START);
        // 4: sit in proper position in destination (chair/basin depending on the service selected)
            //basin only

            sit_in_washbasin(&(client->shop->washbasin[client->basinPosition]), client->id);
 	//printf("cheguei\n");
        // 5: set the client state to the active service
           // client->state = HAVING_A_HAIR_WASH;
	    changeState(client, HAVING_A_HAIR_WASH);
        // 6: rise from destination
	
semWorkingInWashbasins_down(client->basinPosition);
semWorkingInWashbasins_down(client->basinPosition);
	printf("cheguei\n");
            rise_from_washbasin(&(client->shop->washbasin[client->basinPosition]), client->id);
        semIDaccess_up(client->id);
        // At the end the client must leave the barber shop
            leave_barber_shop(client->shop, client->id);

        log_client(client); // more than one in proper places!!!
}


static char* to_string_client(Client* client)
{
   require (client != NULL, "client argument required");

   if (client->internal == NULL)
      client->internal = (char*)mem_alloc(skel_length + 1);

   char requests[4];
   requests[0] = (client->requests & HAIRCUT_REQ) ?   'H' : ':',
   requests[1] = (client->requests & WASH_HAIR_REQ) ? 'W' : ':',
   requests[2] = (client->requests & SHAVE_REQ) ?     'S' : ':',
   requests[3] = '\0';

   char* pos = (char*)"-";
   if (client->chairPosition >= 0)
      pos = int2nstr(client->chairPosition+1, 1);
   else if (client->basinPosition >= 0)
      pos = int2nstr(client->basinPosition+1, 1);

   return gen_boxes(client->internal, skel_length, skel,
                    int2nstr(client->id, 2),
                    client->barberID > 0 ? int2nstr(client->barberID, 2) : "--",
                    requests, stateText[client->state], pos);
}
void changeState(Client *client,int newState)
{	
	sem_down(ACCESS);
	client->shop->clientStates[client->id-1]=newState;
        client->state=newState;
	sem_up(ACCESS);
}
