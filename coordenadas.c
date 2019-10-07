/*
 * coordenadas.c
 *
 *  Created on: Oct 6, 2019
 *      Author: Matias Pierdominici
 */

#include"coordenadas.h"
#include "canDriver.h"
#include "coordBuffCirc.h"
#define COORDENADAS_NUM_GRUP 0X103
#define COORDENADAS_POS_TYPE 0
#define COORDENADAS_BUFF_SIZE 30

static coordenadasCan buf[COORDENADAS_BUFF_SIZE];
static coordBuf_handle_t circRes;
static void coordenadasArray2Struct(coordenadasCan * coord,char * data,int size,int id);

void coordenadasInit(void){
	canDriverInit(0x7F8, 0x100);
	circRes=coordBuffCirc_init(buf, COORDENADAS_BUFF_SIZE);
}

void coordenadasSend(coordenadasCan coord){

	char temp[8]; //arrreglo temporal donde se almacenan los datos
	temp[COORDENADAS_POS_TYPE]=coord.type;
	for(int i=0;i<coord.size;i++){//copio los datos
		temp[i+1]=coord.data[i];
	}
	canDriverSendCanMsg(COORDENADAS_NUM_GRUP,temp,coord.size + 1,0);
	coordBuffCirc_put(circRes, coord);
}

bool coordenadasRecived(void){
	canDataStruct tempdataCan;
	coordenadasCan tempdataCoord;
	if(canDriverIsData()){
		canDriverPopData(&tempdataCan);
		coordenadasArray2Struct(& tempdataCoord,tempdataCan.reciveData,tempdataCan.size,tempdataCan.msgId);\
		coordBuffCirc_put(circRes, tempdataCoord);
	}


	return(!coordBuffCirc_empty(circRes));
}

void coordenadasPop(coordenadasCan * coord){
	coordBuffCirc_get(circRes, coord);
}

static void coordenadasArray2Struct(coordenadasCan * coord,char * data,int size,int id){
	coord->Grupo[0]='G';
	coord->Grupo[1]=(id & 0X0FF)+'0';
	coord->size=size-1;
	coord->type=data[0];

	for(int i=0;i<coord->size;i++){
		coord->data[i]=data[i+1];
	}

}





