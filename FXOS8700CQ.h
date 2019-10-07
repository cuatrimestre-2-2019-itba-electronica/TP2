/*
 * accelerometer.h
 *
 *  Created on: Oct 2, 2019
 *      Author: mlste
 */

#ifndef FXOS8700CQ_H_
#define FXOS8700CQ_H_


/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "MK64F12.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	int16_t x;
	int16_t y;
	int16_t z;
} FXOS_magnet_data_t;

typedef struct {
	int16_t x;
	int16_t y;
	int16_t z;
} FXOS_accel_data_t;

typedef struct
{
	FXOS_magnet_data_t magnet;
	FXOS_accel_data_t  accel;
} FXOS_data_t;


typedef struct
{
	int16_t roll;
	int16_t pitch;
	int16_t orientation;
} FXOS_data_polar_t;

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void FXOS_init();

FXOS_data_t	FXOS_get_data_cartesian();
void FXOS_get_data_polar(FXOS_data_polar_t* polar_data);
void int2chars (int number, char*); //pasa el angulo (int) a un arreglo de chars


/*******************************************************************************
 ******************************************************************************/



#endif /* FXOS8700CQ_H_ */
