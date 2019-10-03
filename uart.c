#include "uart.h"
#include "MK64F12.h"

#define UART_HAL_DEFAULT_BAUDRATE		9600
#define __CORE_CLOCK__					100000000

enum { PA, PB, PC, PD, PE };

#define PORTNUM2PIN(p,n)    (((p)<<5) + (n))
#define PIN2PORT(p)         (((p)>>5) & 0x07)
#define PIN2NUM(p)          ((p) & 0x1F)

#define UART0_TX			PORTNUM2PIN(PB, 17)
#define UART0_RX			PORTNUM2PIN(PB, 16)
#define UART0_ALT			3

#define UART1_TX			PORTNUM2PIN(PC, 4)
#define UART1_RX			PORTNUM2PIN(PC, 3)
#define UART1_ALT			3

#define UART2_TX			PORTNUM2PIN(PD, 3)
#define UART2_RX			PORTNUM2PIN(PD, 2)
#define UART2_ALT			3

#define UART3_TX			PORTNUM2PIN(PC, 17)
#define UART3_RX			PORTNUM2PIN(PC, 16)
#define UART3_ALT			3

#define UART4_TX			PORTNUM2PIN(PE, 24)
#define UART4_RX			PORTNUM2PIN(PE, 25)
#define UART4_ALT			3

static PORT_Type * ports[] = PORT_BASE_PTRS;

void UART_SetBaudRate (UART_Type *uart, uint32_t baudrate);
UART_Type * getUART(uint8_t id);

void uartInit (uint8_t id, uart_cfg_t config){

	UART_Type * uartch = getUART(id);
	int portNum;
	int pinNum;

	switch (id){
	case 0:
		portNum = PIN2PORT(UART0_TX);
		pinNum = PIN2NUM(UART0_TX);
		ports[portNum]->PCR[pinNum] = 0x00;

		//SIM->SCGC5|= SIM_SCGC5_PORTA_MASK;
		ports[portNum]->PCR[pinNum] |= PORT_PCR_MUX(UART0_ALT);
		ports[portNum]->PCR[pinNum] |= PORT_PCR_IRQC(0);

		portNum = PIN2PORT(UART0_RX);
		pinNum = PIN2NUM(UART0_RX);
		ports[portNum]->PCR[pinNum] = 0x00;
		ports[portNum]->PCR[pinNum] |= PORT_PCR_MUX(UART0_ALT);
		ports[portNum]->PCR[pinNum] |= PORT_PCR_IRQC(0);

		SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
		break;
	case 1:
		portNum = PIN2PORT(UART1_TX);
		pinNum = PIN2NUM(UART1_TX);
		ports[portNum]->PCR[pinNum] = 0x00;

		//SIM->SCGC5|= SIM_SCGC5_PORTA_MASK;
		ports[portNum]->PCR[pinNum] |= PORT_PCR_MUX(UART1_ALT);
		ports[portNum]->PCR[pinNum] |= PORT_PCR_IRQC(0);

		portNum = PIN2PORT(UART1_RX);
		pinNum = PIN2NUM(UART1_RX);
		ports[portNum]->PCR[pinNum] = 0x00;
		ports[portNum]->PCR[pinNum] |= PORT_PCR_MUX(UART1_ALT);
		ports[portNum]->PCR[pinNum] |= PORT_PCR_IRQC(0);

		SIM->SCGC4 |= SIM_SCGC4_UART1_MASK;
		break;
	case 2:
		SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
		break;
	case 3:
		SIM->SCGC4 |= SIM_SCGC4_UART3_MASK;
		break;
	case 4:
		portNum = PIN2PORT(UART4_TX);
		ports[portNum]->PCR[UART4_TX] = 0x00;

		//SIM->SCGC5|= SIM_SCGC5_PORTA_MASK;
		ports[portNum]->PCR[UART4_TX] |= PORT_PCR_MUX(UART4_ALT);
		ports[portNum]->PCR[UART4_TX] |= PORT_PCR_IRQC(0);

		portNum = PIN2PORT(UART4_RX);
		ports[portNum]->PCR[UART4_RX] = 0x00;
		ports[portNum]->PCR[UART4_RX] |= PORT_PCR_MUX(UART4_ALT);
		ports[portNum]->PCR[UART4_RX] |= PORT_PCR_IRQC(0);
		SIM->SCGC1 |= SIM_SCGC1_UART4_MASK;
		break;
	}

	uartch->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);

	/* Configure the UART for 8-bit mode, no parity */
	 /* We need all default settings, so entire register is cleared */

	UART_SetBaudRate(uartch, config.baudrate);
	uartch->C1 = 0;
	uartch->CFIFO = UART_CFIFO_RXFLUSH_MASK | UART_CFIFO_TXFLUSH_MASK; // Clear fifo's
	uartch->PFIFO = UART_PFIFO_TXFE_MASK; // Enable tx fifo
	uartch->CFIFO = UART_CFIFO_TXOFE_MASK; // Enable tx fifo overflow interrup
	uartch->TWFIFO = UART_TWFIFO_TXWATER(2);
	uartch->C2 = (UART_C2_TE_MASK | UART_C2_RE_MASK);
}

void UART_SetBaudRate (UART_Type *uart, uint32_t baudrate){
	uint16_t sbr, brfa;
	uint32_t clock;

	clock = ((uart == UART0) || (uart == UART1)) ? (__CORE_CLOCK__):(__CORE_CLOCK__>>1);

	baudrate = ((baudrate == 0)?(UART_HAL_DEFAULT_BAUDRATE):
			((baudrate > 0x1FFF)?(UART_HAL_DEFAULT_BAUDRATE):(baudrate)));

	sbr = clock / (baudrate << 4);
	brfa = (clock << 1) / baudrate - (sbr << 5);

	uart->BDH = UART_BDH_SBR(sbr >> 8);
	uart->BDL = UART_BDL_SBR(sbr);
	uart->C4 = (uart->C4 & ~UART_C4_BRFA_MASK) | UART_C4_BRFA(brfa);
}

/*
void UART_SetBaudRate (UART_Type *uart, uint32_t baud){

	uart->BDH = UART_BDH_SBR(66);
	uart->BDL = UART_BDL_SBR(66);
	uart->C4 = (uart->C4 & ~UART_C4_BRFA_MASK) | UART_C4_BRFA(0);
}*/

void UART_Send_Data(uint8_t id, unsigned char tx_data)
{
	UART_Type * uartch = getUART(id);

	while(((uartch->S1)& UART_S1_TDRE_MASK) ==0); //Puedo Transmitir ?
	uartch->D = (uint8_t) tx_data; // Transmit
	uartch->SFIFO;
}

// Si el dato que sale de CAN ya esta un (char *)/String seria mas facil de mandarlo directamente con uartWriteMsg (sin olvidar de agregar el numero del grupo)
void UART_send_board_info(int gn, int rolido, int cabeceo, int orientacion){

	uint8_t uart_id = 0;

	UART_Send_Data(uart_id,'G');
	UART_Send_Data(uart_id, (unsigned char) gn);
	UART_Send_Data(uart_id,'R');
	UART_Send_Data(uart_id, (unsigned char) rolido);
	UART_Send_Data(uart_id,'C');
	UART_Send_Data(uart_id, (unsigned char) cabeceo);
	UART_Send_Data(uart_id,'O');
	UART_Send_Data(uart_id, (unsigned char) orientacion);
	UART_Send_Data(uart_id,'\r\n');
}

/*void UART_FIFO(uint8_t id){
	UART_Type * uartch = getUART(id);
	while(1){
		uartch->D = (uint8_t) 'dddddd\r\n';
		if(uartch->SFIFO & UART_SFIFO_TXOF_MASK){
			uartch->TCFIFO;
			if(uartch->TCFIFO > 4){
						uartch->TCFIFO;
					}
		}

	}
}*/

uint8_t uartWriteMsg(uint8_t id, const char* msg, uint8_t cant){
	for(uint8_t i=0;i<cant;i++){
		UART_Send_Data(id, msg[i]);
	}

	return 1;
}

UART_Type * getUART(uint8_t id){
	id = (id>UART_CANT_IDS || id<0) ? 0 : id;
	UART_Type * uartch;

	switch (id){
	case 0:
		uartch = UART0;
		break;
	case 1:
		uartch = UART1;
		break;
	case 2:
		uartch = UART2;
		break;
	case 3:
		uartch = UART3;
		break;
	case 4:
		uartch = UART4;
		break;
	}

	return uartch;
}
