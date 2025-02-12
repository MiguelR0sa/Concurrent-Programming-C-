/**
 * \brief barber shop representation
 *  
 * \author Miguel Oliveira e Silva - 2018
 */

#ifndef BARBER_SHOP_H
#define BARBER_SHOP_H

#include "global.h"
#include "barber-chair.h"
#include "tools-pot.h"
#include "washbasin.h"
#include "barber-bench.h"
#include "service.h"
#include "client-benches.h"

typedef struct _BarberShop_
{
   int numBarbers;

   int numChairs;                         // num barber chairs
   BarberChair barberChair[MAX_BARBERS]; // index related with position

   int numScissors;
   int numCombs;
   int numRazors;
   ToolsPot toolsPot;

   int numWashbasins;
   Washbasin washbasin[MAX_WASHBASINS];  // index related with position

   BarberBench barberBench;

   int numClientBenchesSeats;
   int numClientBenches;
   ClientBenches clientBenches;

   int numClientsInside;
   int clientsInside[MAX_CLIENTS];

   int opened;

   int logId;
   char* internal;
  
   //IPC
   
   Service services[MAX_CLIENTS]; // index related to barberID -1
   int assignedClients[MAX_BARBERS];
   int clientStates[MAX_CLIENTS]; // index related to clientID -1
   int livingClients;
} BarberShop;

//shared mem funcitons
void createSharedMemory(void);
void detachSharedMemory(void);
BarberShop * shopConnect();
void shopDisconnect(BarberShop *res);
void shopDestroy(BarberShop *shop);
//////////////////////
//semaphores funcs
void initSemaphores(BarberShop *shop);
void changeState(int clientID,int newState);
int clientState(int clientID);//to be used by barbers
void sem_down(int id);
void sem_up(int id);
void semIDaccess_up(int clientID);
void semIDaccess_down(int clientID);
void  semWorkingInWashbasins_up(int basinPosition);
void  semWorkingInWashbasins_down(int basinPosition);
///////////////////////

int num_lines_barber_shop(BarberShop* shop);
int num_columns_barber_shop(BarberShop* shop);
void init_barber_shop(BarberShop* shop, int num_barbers, int num_chairs,
                      int num_scissors, int num_combs, int num_razors, int num_basins, 
                      int num_client_benches_seats, int num_client_benches);
void term_barber_shop(BarberShop* shop);

int valid_barber_chair_pos(BarberShop* shop, int pos);
BarberChair* barber_chair(BarberShop* shop, int pos);
ToolsPot* tools_pot(BarberShop* shop);
Washbasin* washbasin(BarberShop* shop, int pos);
BarberBench* barber_bench(BarberShop* shop);
ClientBenches* client_benches(BarberShop* shop);

int client_benches_not_empty(BarberShop* shop);

int num_available_barber_chairs(BarberShop* shop);
int reserve_random_empty_barber_chair(BarberShop* shop, int barberID);
int num_available_washbasin(BarberShop* shop);
int reserve_random_empty_washbasin(BarberShop* shop, int barberID);

int is_client_inside(BarberShop* shop, int clientID);

Service wait_service_from_barber(BarberShop* shop, int barberID);
void inform_client_on_service(BarberShop* shop, Service service, int barberID);

void client_done(BarberShop* shop, int clientID);

int enter_barber_shop(BarberShop* shop, int clientID, int request);
void leave_barber_shop(BarberShop* shop, int clientID);
void receive_and_greet_client(BarberShop* shop, int barberID, int clientID);
int greet_barber(BarberShop* shop, int clientID); // returns barberID

int shop_opened(BarberShop* shop);
void close_shop(BarberShop* shop); // no more outside clients accepted

#endif
