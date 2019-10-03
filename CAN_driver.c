#include"SPI_driver.h"
#include <stdio.h>
#include"gpio.h"
#include"board.h"

//Defines de comandos de SPI
#define RESET 0xC0
#define READ 0x03
#define READ_RX_BUFFER 0x90
#define WRITE 0x02
#define LOAD_RX_BUFFER 0x40
#define RTS 0x80
#define READ_STATUS 0xA0
#define RX_STATUS 0xB0
#define BIT_MODIFY 0x05

//Defines de direcciones de registros
#define CNF1_REG 0x2A
#define CNF2_REG 0x29
#define CNF3_REG 0x28

#define RXM0SIDH_REG 0x20
#define RXM0SIDL_REG 0x21

#define RXF0SIDH_REG 0x00
#define RXF0SIDL_REG 0x01

#define CANCTRL_REG 0x0F

#define TXB0CTRL_REG 0x30
#define TXB0SIDH_REG 0x31
#define TXB0DLC_REG 0x35
#define TXB0D0_REG 0x36

#define MAX_BUFF_LEN 30

#define NO_COMMAND 0x00

void irqt(void){
	char readf[30];
	static int edge=1;
	if(edge){
		edge=0;
		CAN_READ(0X2C, readf, 1+2);
		CAN_BIT_MODIFY(0X2C,0X01,0x00);//reseteo rl flag
		CAN_READ(0X60, readf, 14+2);
		CAN_READ(0X2C, readf, 1+2);

	}else{
		edge=1;
	}


}


void CAN_RESET(void)
{
	char frame2send = RESET;
	SPI_driver_sendRecive(&frame2send, 1,NULL);

	//SPI_driver_sendRecive(uint8_t * data2end, uint8_t size,uint8_t * recivedData);
	//SPI_send_frame(RESET,1);
}

void CAN_BIT_MODIFY(char address, char mask, char data)
{
	char buffer[4];
	buffer[0]=BIT_MODIFY;
	buffer[1]=address;
	buffer[2]=mask;
	buffer[3]=data;
	SPI_driver_sendRecive(buffer,4,NULL);
}

void CAN_WRITE(char address, char * buffer, int bufflen)
{
	char send_buff[MAX_BUFF_LEN];
	send_buff[0]=WRITE;
	send_buff[1]=address;

	for(int i=0;(i<bufflen)&&(i<MAX_BUFF_LEN);i++)
	{
		send_buff[2+i]=buffer[i];
	}
	SPI_driver_sendRecive(send_buff,bufflen+2,NULL);
}

void CAN_READ(char address, char * buffer, int bufflen)
{
	char send_buff[MAX_BUFF_LEN];
	send_buff[0]=READ;
	send_buff[1]=address;

	for(int i=0;(i<bufflen-2)&&(i<MAX_BUFF_LEN);i++)
	{
		send_buff[2+i]=NO_COMMAND;
	}
	SPI_driver_sendRecive(send_buff,bufflen,buffer);
}
//buff2send debe ser un 1,2,4 dependiendo que buffer se desea transmitir. Si es 0 no se transmite nada.
void CAN_RTS(char buff2send)
{
	char frame2send=0;
	frame2send=RTS+buff2send;
	//SPI_send_frame(RTS+ID);
	SPI_driver_sendRecive(& frame2send,1,NULL);
}

//Oscilador de 16MHz
//Tengo 8 time quantas por bit
//BRP tiene que valer 7
void init_CAN(int ID)
{
	char temp[20];
	SPI_driver_init();
	char buffer[2];
	char u[10];
	int t=0x04;
	CAN_RESET();//reseteo el controlador y lo pongo en modo configuración
	CAN_BIT_MODIFY(CANCTRL_REG,0xEF,0x8C);
	//Seteo el bitrate y los time quantas
	CAN_BIT_MODIFY(CNF1_REG,0xFF,0X07);//aca seteo el time quanta como 1us
	//CAN_READ(CNF1_REG,u,3);
	CAN_BIT_MODIFY(CNF2_REG,0xFF,0x98);//aca seteo cuantos time quantas tiene el PHSEG1 y el PRSEG
	//CAN_READ(CNF2_REG,u,3);
	CAN_BIT_MODIFY(CNF3_REG,0x47,0x41);//Seteo time quantas asociados a PHSEG2 y WAKFIL
	//CAN_READ(CNF3_REG,u,3);
	//VER QUE hago con TXRTSCTRL register

	CAN_BIT_MODIFY(0X60,0x60,0x60);

	CAN_WRITE(TXB0CTRL_REG,&t,1);
	CAN_READ(TXB0CTRL_REG, temp, 3);

	buffer[0]=0x00;
	buffer[1]=0x00;
	CAN_WRITE(RXM0SIDH_REG,buffer,2);
	buffer[0]=0x20;
	buffer[1]=0x00;
	CAN_WRITE(RXF0SIDH_REG,buffer,2);
	//VER SI NECESITO SETEARLO PARA MAS REGISTROS
	//CAN_BIT_MODIFY(CANCTRL_REG,0xEF,0x0C);//pongo en modosingle shot, habilito el clock y prescaler de 1.
	t=0X01;
	CAN_BIT_MODIFY(0X2B,0x01,0x01);


	CAN_BIT_MODIFY(CANCTRL_REG,0xEF,0x0C);

	//CAN_BIT_MODIFY(CANCTRL_REG,0xEF,0x4C);//lopback





	gpioMode(IRQ_CAN, INPUT);
	gpioIRQ(IRQ_CAN, GPIO_IRQ_MODE_BOTH_EDGES, irqt);


}

void send_CAN(int ID,char * databuffer, int bufflen)//por ahora lo hago solo con el buffer 0
{
//setear el DLC, el RTR, el ID, los datos
/*
*PARA HACER ESTO TENGO QUE MODIFICAR LOS REGISTROS QUE ARRANCAN CON TXB0, EN TXBODN POR EJEMPLO TENGO LOS BYTES QUE QUIERO MANDAR
*/
//Me fijo si el buffer no esta mandando algo
	if(bufflen>8)
		bufflen=8;

	char bufferaux[2];
	CAN_READ(TXB0CTRL_REG,bufferaux,1);
	{
		bufferaux[0]=0x00;
		bufferaux[1]=0x00;
		CAN_WRITE(TXB0SIDH_REG, bufferaux,2);//Seteo el ID a mandar
		bufferaux[0]=bufflen;
		CAN_WRITE(TXB0DLC_REG, bufferaux,1);//seteo dataframe y data len

		CAN_WRITE(TXB0D0_REG, databuffer,bufflen);
		CAN_RTS(1);//mando el comando RTS y le sumo 1 si quiero el primer buffer, 2 si quiero el segundo, 4 si quiero el tercero
	}
}

void receive_CAN()
{

}

