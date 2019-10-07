/*
 * send_handler.c
 *
 *  Created on: Oct 6, 2019
 *      Author: gonzalosilva
 */
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include "board.h"

#include "uart.h"
#include "FXOS8700CQ.h"
#include "data_manage.h"
#include "SysTick.h"
#include "send_handler.h"
#include "coordStruct.h"
#include "coordenadas.h"

static void callback(void);
static void reset_can_struct(coordenadasCan* boards_information);

FXOS_data_t my_cartesian_data;
FXOS_data_polar_t my_polar_data;
bool can_flag; //si esta en 1, nos deja enviar por can
uart_cfg_t configUart;// configuracion del UART
coordenadasCan boards_information;
bool uart_flag;

;
int parameter2send; //determines witch parameter to send via can. pitch, orientation or roll
int buffer_counter;
int data_length;
int send_can_length;
int send_uart_length;

void send_init(void){
	configUart.baudrate=MUART_BAUDRATE;
	configUart.parity=MUART_PARITY;
	configUart.stopBit=MUART_STOPBIT;
	uartInit (MUART_ID, configUart);//configuro el uart para ser usado con la pc
	FXOS_init();
	SysTick_Init ();
	SysTick_append(callback);
	coordenadasInit();
}

void send_handler(void){
	char _3send[9]; //buffer que manda la informacion por uart y can


	my_cartesian_data = FXOS_get_data_cartesian();
	FXOS_get_data_polar(&my_polar_data);

	if (can_flag){ //sends roll

		if(parameter2send == 0){
			data_length = get_board_buffer(_3send, my_polar_data.roll, 3, parameter2send);
			//data_length=5;
			//_3send[0]='G';
			//_3send[1]='3';
			//_3send[2]='R';
			//_3send[3]='1';
			//_3send[4]='2';
			parameter2send++;
		}
		else if(parameter2send == 1){
			data_length = get_board_buffer(_3send, my_polar_data.pitch, 3, parameter2send);
			//data_length=5;
			//_3send[0]='G';
			//_3send[1]='3';
			//_3send[2]='C';
			//_3send[3]='-';
			//_3send[4]='2';
			parameter2send++;
		}
		else if(parameter2send == 2){
			data_length = get_board_buffer(_3send, my_polar_data.orientation, 3, parameter2send);
			//data_length=5;
			//_3send[0]='G';
			//_3send[1]='3';
			//_3send[2]='O';
			//_3send[3]='9';
			//_3send[4]='9';
			parameter2send = 0;
		}

		boards_information.Grupo[0] = _3send[0];
		boards_information.Grupo[1] = _3send[1];
		boards_information.type = _3send[2];
		buffer_counter = 3;
		for (int i = 0; i < data_length - 3; i++, buffer_counter++)
			boards_information.data[i] = _3send[i + 3];
		boards_information.size = data_length-3;
		coordenadasSend(boards_information);
		reset_can_struct(&boards_information);


		can_flag = 0;
	}


	//if(uart_flag){
		if (coordenadasRecived()){
			coordenadasPop(&boards_information);
			_3send[0] = boards_information.Grupo[0];
			_3send[1] = boards_information.Grupo[1];
			_3send[2] = boards_information.type;
			for (send_uart_length = 0; send_uart_length < boards_information.size; send_uart_length++)
				_3send[send_uart_length + 3] = boards_information.data[send_uart_length];
			_3send[send_uart_length + 3] = '\r';
			_3send[send_uart_length + 4] = '\n';
			uartWriteMsg(MUART_ID,_3send,send_uart_length + 5);

			for(int i = 0; i < sizeof(_3send); i++)
				_3send[i] = 0;
			reset_can_struct(&boards_information);
	//	uart_flag = false;
		}
//	}

	//is can data
	//can receive
	//uartWriteMsg(MUART_ID,_3send,29);
}


void callback(void){
	can_flag = true;
	uart_flag = true;
}

void reset_can_struct(coordenadasCan* boards_information){
	boards_information->Grupo[0] = 0;
	boards_information->Grupo[1] = 0;
	boards_information->type = 0;
	for (int i = 0; i < boards_information->size - 3 ; i++)
		boards_information->data[i] = 0;
	boards_information->size = 0;
}
