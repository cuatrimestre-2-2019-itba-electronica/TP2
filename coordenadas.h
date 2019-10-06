/*
 * coordenadas.h
 *
 *  Created on: Oct 6, 2019
 *      Author: Matias Pierdominici
 */

#ifndef COORDENADAS_H_
#define COORDENADAS_H_
#include<stdbool.h>
#include"coordStruct.h"


//envias las cordenadas a las otras placas
void coordenadasSend(coordenadasCan coord);//DATOS > R-120

//devuelve true si si se recivio, alguna posicion por can, y se puede leer con coordenadasPop
bool coordenadasRecived(void);

//si coordenadasRecived devuelve true, se puede llamar a esta funcion para recuperar la cordenada que se recivio
void coordenadasPop(coordenadasCan * coord);


//inicicializacion
void coordenadasInit(void);


#endif /* COORDENADAS_H_ */
