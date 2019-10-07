/*
 * data_manage.h
 *
 *  Created on: Oct 6, 2019
 *      Author: gonzalosilva
 */

#ifndef DATA_MANAGE_H_
#define DATA_MANAGE_H_

#include <stdint.h>
#include <stdbool.h>

int get_board_buffer(char* data, int16_t data_in, int board_num, int r_p_o);

//se le envia toda la informacion que se recibe de una placa y la pone en un arreglo de chars

#endif /* DATA_MANAGE_H_ */
