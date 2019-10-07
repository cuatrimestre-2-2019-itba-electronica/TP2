/*
 * send_handler.h
 *
 *  Created on: Oct 6, 2019
 *      Author: gonzalosilva
 */

#ifndef SEND_HANDLER_H_
#define SEND_HANDLER_H_


#define MUART_BAUDRATE 9600
#define MUART_PARITY 0
#define MUART_STOPBIT 0
#define MUART_ID 0

#define AMOUNT_GROUPS 7

void send_init (void);
void send_handler(void);

#endif /* SEND_HANDLER_H_ */
