/*
 * data_manage.c
 *
 *  Created on: Oct 6, 2019
 *      Author: gonzalosilva
 */


#include "data_manage.h"


static void int2chars (int number, char* number_to_send);


int get_board_buffer(char* data, int16_t data_in, int board_num, int r_p_o){
	char temp[5];
	int i, j;

	data[0] = 'G';
	data[1] = board_num + '0';

	if (r_p_o == 0)
		data[2] = 'R';
	else if(r_p_o == 1)
		data[2] = 'P';
	else if (r_p_o == 2)
		data[2] = 'O';

	int2chars(data_in, temp);
	for (i = 0 , j = 3; *(temp + i) != 0 ; i++, j++)
		data[j] = temp[i];
	return j;
}


void int2chars (int number, char* number_to_send){ //
     //termina con un 0 e.g si number es -10 number_to_send = {'-','1','0',0,x}

    if (number >= 0){
        if (number >= 100) {
            number_to_send[0] = (number / 100) + '0';
            number_to_send[1] = ((number % 100) / 10) + '0';
            number_to_send[2] = ((number % 100) % 10) + '0';
            number_to_send[3] = 0;
        }
        else if (number >= 10) {
            number_to_send[0] = (number / 10) + '0';
            number_to_send[1] = (number % 10) + '0';
            number_to_send[2] = 0;
        }
        else{
            number_to_send[0] = (number % 10) + '0';
            number_to_send[1] = 0;
        }
    }
    else if (number < 0){
        number_to_send[0] = '-';
        if (abs(number) >= 100) {
            number_to_send[1] = (abs(number) / 100) + '0';
            number_to_send[2] = ((abs(number) % 100) / 10) + '0';
            number_to_send[3] = ((abs(number) % 100) % 10) + '0';
            number_to_send[4] = 0;
        }
        else if (abs(number) >= 10) {
            number_to_send[1] = (abs(number) / 10) + '0';
            number_to_send[2] = (abs(number) % 10) + '0';
            number_to_send[3] = 0;
        }
        else{
            number_to_send[1] = (abs(number) % 10) + '0';
            number_to_send[2] = 0;
        }
    }
}
