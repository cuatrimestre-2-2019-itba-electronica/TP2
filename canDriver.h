#ifndef _CANDRIVER_H_
#define _CANDRIVER_H_

//#include <stdbool.h>
//#include<stdint.h>

#include"canStruct.h"

 void canDriverReset(void);

 void canDriverWrite(uint8_t address2write, uint8_t * buffer2send, uint8_t size);

 void canDriverBitModify(uint8_t address2modify, uint8_t masc, uint8_t data);

 void canDriverRts(uint8_t internalBuffer2Send);

 void canDriverRead(uint8_t address2read, uint8_t * recepctionBuffer, uint8_t bufSize);

 void canDriverInit(int msk, int fltr);

 void canDriverSendCanMsg(int msg2SendId,uint8_t * data2send,uint8_t size,uint8_t rxoBuff);

 bool canDriverIsData(void);
 void canDriverPopData(canDataStruct * recived);

























#endif // _CANDRIVER_H_
