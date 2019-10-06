#include"MCP25625.h"
#include"SPI_driver.h"
#include <stdio.h>
#include"gpio.h"
#include"board.h"

#include"canCircBuff.h"

#define CAN_DRIVER_SIZE_RECEPCTION_BUFFER 30
#define CAN_DRIVER_ONE_BIT_READ 3
#define CAN_DRIVER_RAED_DATA 14

#define CAN_DRIVER_SHIFT_HIGH_ID 3
#define CAN_DRIVER_POS_HIGH_ID 3
#define CAN_DRIVER_SHIFT_LOW_ID 5
#define CAN_DRIVER_POS_LOW_ID 4
#define CAN_DRIVER_POS_DLC 7


#define CAN_DRIVER_WRITE_POS_ADRESS 1
#define CAN_DRIVER_WRITE_POS_INSTR 0
#define CAN_DRIVER_WRITE_POS_MSK 2
#define CAN_DRIVER_WRITE_POS_DATA 3

#define CAN_DRIVER_MAX_DATA_2_SEND 8

#define CAN_DRIVER_SIZE_CIRC_BUFF 30




static canDataStruct dataStored[CAN_DRIVER_SIZE_CIRC_BUFF];
static cbuf_handle_t rxCircBuff;



//recive el buffer que hay que chequiar y los flags, devuelve 1 si esta lleno
static bool canDriverRxBuffFull(int wichBuff, uint8_t wichFlag){
	bool temp=false;
	switch (wichBuff){
    	case 1:
    	if(wichFlag &((uint8_t)RX1IF_MASK)){
    		temp=true;
    	}
     	 break;
    	case 0:
    	if(wichFlag & ((uint8_t)RX0IF_MASK)){
			temp=true;
    	}
      	break;
      	default:
      		temp=false;
      	break;
  }

  return temp;

}


//interrupcion que se llama al llenarse el buffer RX0
static void canDriverIrq(void){
	canDataStruct tempReciveData; //estructura que se pushea a la buffer circular donde se almacena lo recivido
	uint8_t flag;
	uint8_t regBuffReception[CAN_DRIVER_SIZE_RECEPCTION_BUFFER];//buffer donde se almacena temporalmente los registros leidos
	canDriverRead(CANINTF,regBuffReception,CAN_DRIVER_ONE_BIT_READ);//leo si hubo recepcion
	flag=regBuffReception[2];
	//canDriverRead(RXBXCTRL(0),regBuffReception, CAN_DRIVER_RAED_DATA +2);
	if(canDriverRxBuffFull(0,flag)){//verifico si el buffer de recepcion 0 esta lleno

		canDriverRead(RXBXCTRL(0),regBuffReception, CAN_DRIVER_RAED_DATA +2);//leo el buffer de datos
		tempReciveData.msgId=(regBuffReception[CAN_DRIVER_POS_HIGH_ID]<<CAN_DRIVER_SHIFT_HIGH_ID)+(regBuffReception[CAN_DRIVER_POS_LOW_ID]>>CAN_DRIVER_SHIFT_LOW_ID);//calculo el id recivido
		tempReciveData.size=regBuffReception[CAN_DRIVER_POS_DLC];//busco cuantos bytes me mandaron

		for(int i=CAN_DRIVER_POS_DLC+1;i<CAN_DRIVER_POS_DLC+ 1 + tempReciveData.size;i++)
		{
			tempReciveData.reciveData[i-(CAN_DRIVER_POS_DLC+1)]=regBuffReception[i];//copio los datos a la estructura
		}
		//push_bufferCAN(&RxBuf, tempReciveData);
		//todo buffer circular
		canCircBuff_put(rxCircBuff,tempReciveData);
	}



	canDriverBitModify(CANINTF,RX0IF_MASK,RX0IF_DATA(0));//apago la interrupcion
}


void canDriverRead(uint8_t address2read, uint8_t * recepctionBuffer, uint8_t bufSize){
	uint8_t data2send[CAN_DRIVER_SIZE_RECEPCTION_BUFFER];
	data2send[CAN_DRIVER_WRITE_POS_ADRESS]=address2read;
	data2send[CAN_DRIVER_WRITE_POS_INSTR]=READ;
	for(int i=0;(i<bufSize-2)&&(i<CAN_DRIVER_SIZE_RECEPCTION_BUFFER);i++)
		{
			data2send[2+i]=1;//mando basura
		}
	SPI_driver_sendRecive(data2send,bufSize,recepctionBuffer);
}

void canDriverWrite(uint8_t address2write, uint8_t * buffer2send, uint8_t size){
	uint8_t data2send[CAN_DRIVER_SIZE_RECEPCTION_BUFFER];
	data2send[CAN_DRIVER_WRITE_POS_ADRESS]=address2write;
	data2send[CAN_DRIVER_WRITE_POS_INSTR]=WRITE;
		for(int i=0;(i<size)&&(i<CAN_DRIVER_SIZE_RECEPCTION_BUFFER);i++)
	{
		data2send[2+i]=buffer2send[i];
	}
		SPI_driver_sendRecive(data2send,size+2,NULL);
}


void canDriverBitModify(uint8_t address2modify, uint8_t masc, uint8_t data){
	uint8_t data2send[4];
	data2send[CAN_DRIVER_WRITE_POS_DATA]=data;
	data2send[CAN_DRIVER_WRITE_POS_ADRESS]=address2modify;
	data2send[CAN_DRIVER_WRITE_POS_INSTR]=BIT_MODIFY;
	data2send[CAN_DRIVER_WRITE_POS_MSK]=masc;
	SPI_driver_sendRecive(data2send,4,NULL);

}

void canDriverReset(void){
	uint8_t data2send=RESET;
	SPI_driver_sendRecive(&data2send,1,NULL);

}

void canDriverRts(uint8_t internalBuffer2Send){
	uint8_t data2send;
	data2send=RTS+internalBuffer2Send;
	SPI_driver_sendRecive(&data2send,1,NULL);

}


void canDriverInit(int msk, int fltr){
	uint8_t tempBuff[4];
	rxCircBuff=canCircBuff_init(dataStored,CAN_DRIVER_SIZE_CIRC_BUFF);
	SPI_driver_init();
	gpioMode(IRQ_CAN, INPUT);
	gpioIRQ(IRQ_CAN, GPIO_IRQ_MODE_FALLING_EDGE,canDriverIrq);

	canDriverReset();//reseteo la placa
	canDriverBitModify(CANCTRL,REQOP_MASK,REQOP_DATA(CONFIGURATION));//pongo la placa en modo config
	canDriverBitModify(CNF1,SJW_MASK|BRP_MASK,SJW_DATA(SJW-1) | BRP_DATA(BAUDRATE_PRESCALER));//time quanta de 1 u

	//seto el tamano del bit a 8 tq
	canDriverBitModify(CNF2,BTLMODE_MASK|SAM_MASK|PHSEG1_MASK|PRSEG_MASK,BTLMODE_DATA(BTLMODE)|SAM_DATA(SAM)|PHSEG1_DATA(PHSEG1-1)|PRSEG_DATA(PRSEG-1));
	canDriverBitModify(CNF3,WAKFIL_MASK| PHSEG2_MASK,WAKFIL_DATA(WAKFIL)| PHSEG2_DATA(PHSEG2-1));

	canDriverBitModify(0x0D,0x07,0x01);//todo ver

	//configuracion de filtros y mascara
	tempBuff[0]=(char)(0xFF &(fltr>>CAN_DRIVER_SHIFT_HIGH_ID));
	tempBuff[1]=(char)(0xFF &(fltr<<CAN_DRIVER_SHIFT_LOW_ID));
	canDriverWrite(RXF0SIDH,tempBuff,2);
	canDriverRead(RXF0SIDH, tempBuff, 4);
	tempBuff[0]=(char)(0xFF & (msk>>CAN_DRIVER_SHIFT_HIGH_ID));
	tempBuff[1]=(char)((msk<<CAN_DRIVER_SHIFT_LOW_ID)& 0xFF);
	canDriverWrite(RXMXSIDH(0),tempBuff,2);
	//fin configuracion de filtros y mascara


	canDriverBitModify(RXBXCTRL(0),RECEIVE_MODE_MASK|BUKT_MASK,RECEIVE_MODE_DATA(0)|BUKT_DATA(0));//seteo mascara en buffers

	canDriverBitModify(TXBXCTRL(0),T_PRIORITY_MASK,T_PRIORITY_DATA(3));

	canDriverBitModify(CANINTE,RX0IE_MASK|TX0IE_MASK,RX0IE_DATA(1)|TX0IE_DATA(0));//interrupcion mensage valido llego

	//canDriverBitModify(CANCTRL,REQOP_MASK|OSM_MASK,REQOP_DATA(NORMAL)|OSM_DATA(0));//modo normal

	canDriverBitModify(CANCTRL,REQOP_MASK|OSM_MASK,REQOP_DATA(LOOPBACK)|OSM_DATA(0));



}

void canDriverSendCanMsg(int msg2SendId,uint8_t * data2send,uint8_t size,uint8_t rxoBuff){
	bool status=false;//si se pudo hacer la trasmicion
	uint8_t temp[3];//buffer auxiliar para cargar id en registro
	if(size>CAN_DRIVER_MAX_DATA_2_SEND){
		size=CAN_DRIVER_MAX_DATA_2_SEND; //no se enbian mas de 8 bytes
	}
	canDriverRead(TXBXCTRL(rxoBuff),temp,1);

	temp[0]=(uint8_t)(0xFF &(msg2SendId>>CAN_DRIVER_SHIFT_HIGH_ID));
	temp[1]=(uint8_t)((msg2SendId&0x07)<<CAN_DRIVER_SHIFT_LOW_ID);
	canDriverWrite(TXBXSIDH(rxoBuff), temp,2);//cargo id
	temp[0]=size;
	canDriverWrite(TXBXDLC(rxoBuff), temp,1);//cargo longitud del msg
	canDriverWrite(TXBXD0(rxoBuff), data2send,size);//cargo data a enviar
	switch (rxoBuff) {
	case 1:
		canDriverRts(2);
		break;
	case 2:
		canDriverRts(4);
		break;
	default:
		canDriverRts(1);
		break;
	}
}

bool canDriverIsData(void){
	return (!canCircBuff_empty(rxCircBuff));
}

void canDriverPopData(canDataStruct * recived){

	canCircBuff_get(rxCircBuff, recived);

}

