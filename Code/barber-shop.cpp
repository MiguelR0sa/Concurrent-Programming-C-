#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "dbc.h"
#include "utils.h"
#include "box.h"
#include "logger.h"
#include "global.h"
#include "barber-shop.h"

/* TODO: change here this file to your needs */

///////////////////// SHARED MEM SETUP
/** \brief resource key */
const key_t key = IPC_PRIVATE;


/** \brief internal storage region of barber-shop type */
static int shmid = -1;
static BarberShop *barbershop = NULL;
//////////////////
//semaphores
 //semaphores
static int sem_Access;
static int sem_FreeClientBenches;
static int sem_ClientsSeated;
static int sem_FreeBarbers;
static int sem_FreeWashbasins;
static int sem_IDaccess;
static int sem_WorkingInWashbasins;
/* index of access, full and empty semaphores */



//////////////

static const int skel_length = 10000;
static char skel[skel_length];

//static void log_barber_shop(BarberShop* shop);
static char* to_string_barber_shop(BarberShop* shop);

int num_lines_barber_shop(BarberShop* shop)
{
   require (shop != NULL, "shop argument required");

   return 1+3+num_lines_barber_chair()+num_lines_tools_pot()+3*shop->numClientBenches+1;
}

int num_columns_barber_shop(BarberShop* shop)
{
   require (shop != NULL, "shop argument required");

   struct winsize w;
   ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

   return w.ws_col == 0 ? 80 : w.ws_col;
}

void init_barber_shop(BarberShop* shop, int num_barbers, int num_chairs,
                      int num_scissors, int num_combs, int num_razors, int num_basins, 
                      int num_client_benches_seats, int num_client_benches)
{
   require (shop != NULL, "shop argument required");
   require (num_barbers > 0 && num_barbers <= MAX_BARBERS, concat_5str("invalid number of barbers (", int2str(num_barbers), " not in [1,", int2str(MAX_BARBERS), "])"));
   require (num_chairs > 0 && num_chairs <= MAX_BARBER_CHAIRS, concat_5str("invalid number of chairs (", int2str(num_chairs), " not in [1,", int2str(MAX_BARBER_CHAIRS), "])"));
   require (num_scissors > 0 && num_scissors <= MAX_NUM_TOOLS, concat_5str("invalid number of scissors (", int2str(num_scissors), " not in [1,", int2str(MAX_NUM_TOOLS), "])"));
   require (num_combs > 0 && num_combs <= MAX_NUM_TOOLS, concat_5str("invalid number of combs (", int2str(num_combs), " not in [1,", int2str(MAX_NUM_TOOLS), "])"));
   require (num_razors > 0 && num_razors <= MAX_NUM_TOOLS, concat_5str("invalid number of razors (", int2str(num_razors), " not in [1,", int2str(MAX_NUM_TOOLS), "])"));
   require (num_basins > 0 && num_basins <= MAX_WASHBASINS, concat_5str("invalid number of washbasins (", int2str(num_basins), " not in [1,", int2str(MAX_WASHBASINS), "])"));
   require (num_client_benches_seats > 0 && num_client_benches_seats <= MAX_CLIENT_BENCHES_SEATS, concat_5str("invalid number of client benches seats (", int2str(num_client_benches_seats), " not in [1,", int2str(MAX_CLIENT_BENCHES_SEATS), "])"));
   require (num_client_benches > 0 && num_client_benches <= num_client_benches_seats, concat_5str("invalid number of client benches (", int2str(num_client_benches), " not in [1,", int2str(num_client_benches_seats), "])"));
   shop->numBarbers = num_barbers;
   shop->numChairs = num_chairs;
   shop->numScissors = num_scissors;
   shop->numCombs = num_combs;
   shop->numRazors = num_razors;
   shop->numWashbasins = num_basins;
   shop->numClientBenchesSeats = num_client_benches_seats;
   shop->numClientBenches = num_client_benches;
   shop->numClientsInside = 0;
   for(int i = 0; i < MAX_CLIENTS; i++)
      shop->clientsInside[i] = 0;
   shop->opened = 1;

   gen_rect(skel, skel_length, num_lines_barber_shop(shop), num_columns_barber_shop(shop), 0xF, 1);
   gen_overlap_boxes(skel, 0, skel,
                     (char*)" BARBER SHOP ", 0, 2,
                     (char*)" Idle Barbers:", 2, 1,
                     (char*)"Barber Chairs:", 2+3, 1,
                     (char*)"Washbasins:", 3+3+num_lines_barber_chair(), num_columns_tools_pot()+3,
                     (char*)" Waiting Room:", 2+3+num_lines_barber_chair()+num_lines_tools_pot(), 1,
                     (char*)"+          +", num_lines_barber_shop(shop)-1, num_columns_barber_shop(shop)-15, NULL);

   shop->internal = (char*)mem_alloc(skel_length + 1);

   shop->logId = register_logger((char*)"Barber Shop: ", 0, 0, num_lines_barber_shop(shop), num_columns_barber_shop(shop), NULL);
   log_barber_shop(shop);

   // init components:
   init_barber_bench(&shop->barberBench, num_barbers, 0, 1, 16);
   for (int i = 0; i < num_chairs; i++)
      init_barber_chair(shop->barberChair+i, i+1, 1+3, 16+i*(num_columns_barber_chair()+2));
   init_tools_pot(&shop->toolsPot, num_scissors, num_combs, num_razors, 1+3+num_lines_barber_chair(), 1);
   for (int i = 0; i < num_basins; i++)
      init_washbasin(shop->washbasin+i, i+1, 1+3+num_lines_barber_chair(), num_columns_tools_pot()+3+11+1+i*(num_columns_washbasin()+2));
   init_client_benches(&shop->clientBenches, num_client_benches_seats, num_client_benches, 1+3+num_lines_barber_chair()+num_lines_tools_pot(), 16);

////////////////////////////////////

////////////////////////////////////

}


void term_barber_shop(BarberShop* shop)
{
   require (shop != NULL, "shop argument required");

   // term components:
   term_client_benches(&shop->clientBenches);
   for (int i = 0; i < shop->numWashbasins; i++)
      term_washbasin(shop->washbasin+i);
   term_tools_pot(&shop->toolsPot);
   for (int i = 0; i < shop->numChairs; i++)
      term_barber_chair(shop->barberChair+i);
   term_barber_bench(&shop->barberBench);

   mem_free(shop->internal);
}
void show_barber_shop(BarberShop* shop)
{
   require (shop != NULL, "shop argument required");

   log_barber_shop(shop);
   log_barber_bench(&shop->barberBench);
   for (int i = 0; i < shop->numChairs; i++)
      log_barber_chair(shop->barberChair+i);
   log_tools_pot(&shop->toolsPot);
   for (int i = 0; i < shop->numWashbasins; i++)
      log_washbasin(shop->washbasin+i);
   log_client_benches(&shop->clientBenches);
}
void log_barber_shop(BarberShop* shop)
{
   require (shop != NULL, "shop argument required");

   spend(random_int(global->MIN_VITALITY_TIME_UNITS, global->MAX_VITALITY_TIME_UNITS));
   send_log(shop->logId, to_string_barber_shop(shop));
}

int valid_barber_chair_pos(BarberShop* shop, int pos)
{
   require (shop != NULL, "shop argument required");

   return pos >= 0 && pos < shop->numChairs;
}

BarberChair* barber_chair(BarberShop* shop, int pos)
{
   require (shop != NULL, "shop argument required");
   require (valid_barber_chair_pos(shop, pos), concat_5str("invalid chair position (", int2str(pos), " not in [0,", int2str(shop->numChairs), "[)"));

   return shop->barberChair + pos;
}

ToolsPot* tools_pot(BarberShop* shop)
{
   require (shop != NULL, "shop argument required");

   return &shop->toolsPot;
}

int valid_washbasin_pos(BarberShop* shop, int pos)
{
   require (shop != NULL, "shop argument required");

   return pos >= 0 && pos < shop->numWashbasins;
}

Washbasin* washbasin(BarberShop* shop, int pos)
{
   require (shop != NULL, "shop argument required");
   require (valid_washbasin_pos(shop, pos), concat_5str("invalid basin position (", int2str(pos), " not in [0,", int2str(shop->numWashbasins), "[)"));

   return shop->washbasin + pos;
}

BarberBench* barber_bench(BarberShop* shop)
{
   require (shop != NULL, "shop argument required");

   return &shop->barberBench;
}

ClientBenches* client_benches(BarberShop* shop)
{
   require (shop != NULL, "shop argument required");

   return &shop->clientBenches;
}

int num_available_barber_chairs(BarberShop* shop)
{
   require (shop != NULL, "shop argument required");

   int res = 0;
   for(int pos = 0; pos < shop->numChairs ; pos++)
      if (empty_barber_chair(shop->barberChair+pos))
         res++;

   return res;
}

int reserve_random_empty_barber_chair(BarberShop* shop, int barberID)
{
   require (shop != NULL, "shop argument required");
   require (barberID > 0, concat_3str("invalid barber id (", int2str(barberID), ")"));
   require (num_available_barber_chairs(shop) > 0, "barber chair not available");

   int r = random_int(1, num_available_barber_chairs(shop));
   int res;
   for(res = 0; r > 0 && res < shop->numChairs ; res++)
      if (empty_barber_chair(shop->barberChair+res))
         r--;
   res--;
   reserve_barber_chair(shop->barberChair+res, barberID);

   ensure (res >= 0 && res < shop->numChairs, "");

   return res;
}

int num_available_washbasin(BarberShop* shop)
{
   require (shop != NULL, "shop argument required");

   int res = 0;
   for(int pos = 0; pos < shop->numWashbasins ; pos++)
      if (empty_washbasin(shop->washbasin+pos))
         res++;

   return res;
}

int reserve_random_empty_washbasin(BarberShop* shop, int barberID)
{
   require (shop != NULL, "shop argument required");
   require (barberID > 0, concat_3str("invalid barber id (", int2str(barberID), ")"));
   require (num_available_washbasin(shop) > 0, "washbasin not available");

   int r = random_int(1, num_available_washbasin(shop));
   int res;
   for(res = 0; r > 0 && res < shop->numWashbasins ; res++)
      if (empty_washbasin(shop->washbasin+res))
         r--;
   res--;
   reserve_washbasin(shop->washbasin+res, barberID);

   ensure (res >= 0 && res < shop->numWashbasins, "");

   return res;
}

int is_client_inside(BarberShop* shop, int clientID)
{
   require (shop != NULL, "shop argument required");
   require (clientID > 0, concat_3str("invalid client id (", int2str(clientID), ")"));

   int res = 0;

   for(int i = 0; !res && i < shop->numClientsInside; i++)
      res = (clientID == shop->clientsInside[i]);

   return res;
}

Service wait_service_from_barber(BarberShop* shop, int barberID)
{
   /** TODO:
    * function called from a client, expecting to be informed of the next Service to be provided by a barber
    **/
  
   require (shop != NULL, "shop argument required");
 
   sem_down(ACCESS);
  

 //////// func
   Service res;
   res = shop->services[barberID-1];
///////


   sem_up(ACCESS);
   return res;


}

void inform_client_on_service(BarberShop* shop, Service service, int barberID)
{
   /** TODO:
    * function called from a barber, expecting to inform a client of its next service
    **/
   
   require (shop != NULL, "shop argument required");
   sem_down(ACCESS);
   
   shop->services[barberID-1] = service;
   
   sem_up(ACCESS);

		
}

void client_done(BarberShop* shop, int clientID)
{
   /** TODO:
    * function called from a barber, notifying a client that all its services are done
    **/

   require (shop != NULL, "shop argument required");
   require (clientID > 0, concat_3str("invalid client id (", int2str(clientID), ")"));
   sem_down(ACCESS);
   shop->clientStates[clientID-1]=10;
   sem_up(ACCESS);

}

/**
 * Function called from a client when entering the barbershop
 */
int enter_barber_shop(BarberShop* shop, int clientID, int request)
{
   require (shop != NULL, "shop argument required");
   require (clientID > 0, concat_3str("invalid client id (", int2str(clientID), ")"));
   require (request > 0 && request < 8, concat_3str("invalid request (", int2str(request), ")"));
   require (num_available_benches_seats(client_benches(shop)) > 0, "empty seat not available in client benches");
   require (!is_client_inside(shop, clientID), concat_3str("client ", int2str(clientID), " already inside barber shop"));

   int res = random_sit_in_client_benches(&shop->clientBenches, clientID, request);
   shop->clientsInside[shop->numClientsInside++] = clientID;
   return res;
}

/**
 * Function called from a client when leaving the barbershop
 */
void leave_barber_shop(BarberShop* shop, int clientID)
{
   require (shop != NULL, "shop argument required");
   require (clientID > 0, concat_3str("invalid client id (", int2str(clientID), ")"));
   require (is_client_inside(shop, clientID), concat_3str("client ", int2str(clientID), " already inside barber shop"));

   int i;
   for(i = 0; shop->clientsInside[i] != clientID; i++)
      ;
   shop->numClientsInside--;
   check (shop->clientsInside[i] == clientID, "");
   for(; i < shop->numClientsInside; i++)
      shop->clientsInside[i] = shop->clientsInside[i+1];
}

void receive_and_greet_client(BarberShop* shop, int barberID, int clientID)
{
   /** TODO:
    * function called from a barber, when receiving a new client
    * it must send the barber ID to the client
    **/

   require (shop != NULL, "shop argument required");
   require (barberID > 0, concat_3str("invalid barber id (", int2str(barberID), ")"));
   require (clientID > 0, concat_3str("invalid client id (", int2str(clientID), ")"));
  
   sem_down(ACCESS);
   shop-> assignedClients[barberID-1]=clientID;	
   sem_up(ACCESS);
}

int greet_barber(BarberShop* shop, int clientID)
{
   /** TODO:
    * function called from a client, expecting to receive its barber's ID
    **/ 

   require (shop != NULL, "shop argument required");
   require (clientID > 0, concat_3str("invalid client id (", int2str(clientID), ")"));
   sem_down(ACCESS);
   
   int res = 0;
   for(res=0; clientID!=shop->assignedClients[res];res++)
   {
	if(res==MAX_BARBERS-1)
             return 0;
   }
   sem_up(ACCESS);
   return res+1;
}

int shop_opened(BarberShop* shop)
{
   require (shop != NULL, "shop argument required");

   return shop->opened;
}

void close_shop(BarberShop* shop)
{
   require (shop != NULL, "shop argument required");
   require (shop_opened(shop), "barber shop already closed");
 
   shop->opened = 0;
}

static char* to_string_barber_shop(BarberShop* shop)
{
   return gen_boxes(shop->internal, skel_length, skel);
}
void createSharedMemory()
{
    /* create the shared memory */ 
   printf("hello");
    shmid = pshmget(key, sizeof(BarberShop), 0600 | IPC_CREAT | IPC_EXCL);
   printf("hello");
  
}
void detachSharedMemory(BarberShop * shop)
{
    /* detach shared memory from process addressing space */
    pshmdt(shop);
    barbershop = NULL;
}



/* ************************************************* */

BarberShop * shopConnect()
{
    
    /* attach shared memory to process addressing space */ 
    BarberShop *res;	
    res = (BarberShop*)pshmat(shmid, NULL, 0);
 
    //printf("atach success\n ");
   return res;		
}

/* ************************************************* */

void shopDisconnect(BarberShop *shop)
{
    /* detach shared memory from process addressing space */
    pshmdt(shop);
    shop = NULL;
  //  shmid = -1;
}

/* ************************************************* */

void shopDestroy(BarberShop *shop)
{	
    /* destroy semaphore set */
    psemctl(sem_Access, 0, IPC_RMID, NULL);
    psemctl(sem_FreeClientBenches, 0, IPC_RMID, NULL);
    psemctl(sem_ClientsSeated, 0, IPC_RMID, NULL);
    psemctl(sem_FreeBarbers, 0, IPC_RMID, NULL);
    psemctl(sem_FreeWashbasins, 0, IPC_RMID, NULL);
    for(int i =0;i<global->NUM_CLIENTS;i++)    
       psemctl(sem_IDaccess, i, IPC_RMID, NULL);
    for(int i =0;i<shop->numWashbasins;i++)    
       psemctl( sem_WorkingInWashbasins, i, IPC_RMID, NULL);

    
    /* ask OS to destroy the shared memory */
    pshmctl(shmid, IPC_RMID, NULL);
    shmid = -1;
}
void initSemaphores(BarberShop *shop)
{  //client benches semaphore
      sem_Access = psemget(key, 1, 0600 | IPC_CREAT | IPC_EXCL);
     sem_up(ACCESS);	//access granted
	
      sem_FreeClientBenches = psemget(key, 1, 0600 | IPC_CREAT | IPC_EXCL);
   
     for (int i = 0; i <= shop->numClientBenches; i++)
    	sem_up(FREE_CLIENT_BENCHES);	//access granted
   

     sem_ClientsSeated= psemget(key, 1, 0600 | IPC_CREAT | IPC_EXCL);
 //sem_up(CLIENTS_SEATED);
	//access denied



      sem_FreeBarbers= psemget(key, 1, 0600 | IPC_CREAT | IPC_EXCL);
  
     for (int i = 0; i <= shop->numBarbers; i++)
            sem_up(FREE_BARBERS);	//access granted
    

      sem_FreeWashbasins= psemget(key, 1, 0600 | IPC_CREAT | IPC_EXCL);
  
	 for (int i = 0; i <=shop->numWashbasins; i++)
         sem_up(FREE_WASHBASINS); //access granted 	
 
    sem_IDaccess= psemget(key, global->NUM_CLIENTS, 0600 | IPC_CREAT | IPC_EXCL);
    //  sem_up(IDACCESS); //access granted 
     
    
//all dead
	sem_WorkingInWashbasins= psemget(key, global->NUM_WASHBASINS, 0600 | IPC_CREAT | IPC_EXCL);
         for (int i = 0; i <shop->numWashbasins; i++)
         semWorkingInWashbasins_up(i);
      

	  
    
    
}
 int clientState(int clientID)
{
	sem_down(ACCESS);
        int res;
	res =barbershop->clientStates[clientID-1];
	sem_up(ACCESS);	
	return res;
}
void sem_up(int id)
{	
    switch(id)
    {
	case ACCESS:	
	psem_up(sem_Access, 0);
	break;
    	case FREE_CLIENT_BENCHES:	
	psem_up(sem_FreeClientBenches, 0);
	break;
        case CLIENTS_SEATED:	
	psem_up(sem_ClientsSeated, 0);
	break; 
 	case FREE_BARBERS:	
	psem_up(sem_FreeBarbers, 0);
	break;   
        case FREE_WASHBASINS:	
	psem_up(sem_FreeWashbasins, 0);
	break;
       
    }
}

void sem_down(int id)
{
    switch(id)
    {
	case ACCESS:
	psem_down(sem_Access, 0);
	break;
    	case FREE_CLIENT_BENCHES:	
	psem_down(sem_FreeClientBenches, 0);
	break;
        case CLIENTS_SEATED:	
	psem_down(sem_ClientsSeated, 0);
	break; 
 	case FREE_BARBERS:	
	psem_down(sem_FreeBarbers, 0);
	break;   
        case FREE_WASHBASINS:	
	psem_down(sem_FreeWashbasins, 0);
	break;
	
	
    }
}
void semIDaccess_up(int clientID)
{
	psem_up(sem_IDaccess, clientID-1);
}
void semIDaccess_down(int clientID)
{
	psem_down(sem_IDaccess, clientID-1);
}
void semWorkingInWashbasins_up(int basinPosition)
{
	psem_up(sem_WorkingInWashbasins, basinPosition);
}
void  semWorkingInWashbasins_down(int basinPosition)
{
	psem_down(sem_WorkingInWashbasins, basinPosition);
}


















