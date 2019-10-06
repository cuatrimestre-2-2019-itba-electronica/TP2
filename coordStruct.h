/*
 * coordStruct.h
 *
 *  Created on: Oct 6, 2019
 *      Author: Matias Pierdominici
 */

#ifndef COORDSTRUCT_H_
#define COORDSTRUCT_H_

typedef struct coordenadasCan {
   char type;	//R: rolido, C: cabeceo, O: orientacion
   char Grupo[2];
   char data[4];//-120 ,12
   char size;
} coordenadasCan;

#endif /* COORDSTRUCT_H_ */
