#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include "board.h"
#include "uart.h"
#include "FXOS8700CQ.h"
#include "data_manage.h"
#include "SysTick.h"
#include "send_handler.h"


/***************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 **************************/

/***************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 **************************/




/***************************
 ***************************
                        GLOBAL FUNCTION DEFINITIONS
 ***************************
 **************************/

/* Función que se llama 1 vez, al comienzo del programa */
void inter(void){
	while(1){
		send_handler();
	}
}


void App_Init (void)
{
	send_init();

	//SPI_driver_init();
	//init_CAN(4);
}


/* Función que se llama constantemente en un ciclo infinito */

void App_Run (void)
{
	send_handler();
}
