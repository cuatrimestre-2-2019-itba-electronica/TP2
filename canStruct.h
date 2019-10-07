#ifndef CAN_STRUCT_H_
#define CAN_STRUCT_H_
#include<stdint.h>


#define CAN_STRUCT_DATA_SIZE 10


typedef struct canDataStruct {
   uint8_t reciveData[CAN_STRUCT_DATA_SIZE];
   int msgId;
   uint8_t size;
} canDataStruct;


#endif /* CAN_STRUCT_H_ */
