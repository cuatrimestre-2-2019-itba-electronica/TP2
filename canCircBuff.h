#ifndef CAN_CIRCBUFF_H_
#define CAN_CIRCBUFF_H_

#include"canStruct.h"

#include <stdbool.h>
#include <stdlib.h>

#include <stdint.h>

#include <stddef.h>
//fuente:https://github.com/embeddedartistry/embedded-resources/blob/master/examples/c/circular_buffer/circular_buffer.h


/// Opaque circular buffer structure

typedef struct canCircBuff_t canCircBuff_t;



/// Handle type, the way users interact with the API

typedef canCircBuff_t* cbuf_handle_t;



/// Pass in a storage buffer and size, returns a circular buffer handle

/// Requires: buffer is not NULL, size > 0

/// Ensures: cbuf has been created and is returned in an empty state

cbuf_handle_t canCircBuff_init(canDataStruct * buffer, size_t size);



/// Free a circular buffer structure

/// Requires: cbuf is valid and created by canCircBuff_init

/// Does not free data buffer; owner is responsible for that

void canCircBuff_free(cbuf_handle_t cbuf);



/// Reset the circular buffer to empty, head == tail. Data not cleared

/// Requires: cbuf is valid and created by canCircBuff_init

void canCircBuff_reset(cbuf_handle_t cbuf);



/// Put version 1 continues to add data if the buffer is full

/// Old data is overwritten

/// Requires: cbuf is valid and created by canCircBuff_init

void canCircBuff_put(cbuf_handle_t cbuf, canDataStruct data);



/// Put Version 2 rejects new data if the buffer is full

/// Requires: cbuf is valid and created by canCircBuff_init

/// Returns 0 on success, -1 if buffer is full

int canCircBuff_put2(cbuf_handle_t cbuf, canDataStruct data);



/// Retrieve a value from the buffer

/// Requires: cbuf is valid and created by canCircBuff_init

/// Returns 0 on success, -1 if the buffer is empty

int canCircBuff_get(cbuf_handle_t cbuf, canDataStruct * data);



/// CHecks if the buffer is empty

/// Requires: cbuf is valid and created by canCircBuff_init

/// Returns true if the buffer is empty

bool canCircBuff_empty(cbuf_handle_t cbuf);



/// Checks if the buffer is full

/// Requires: cbuf is valid and created by canCircBuff_init

/// Returns true if the buffer is full

bool canCircBuff_full(cbuf_handle_t cbuf);



/// Check the capacity of the buffer

/// Requires: cbuf is valid and created by canCircBuff_init

/// Returns the maximum capacity of the buffer

size_t canCircBuff_capacity(cbuf_handle_t cbuf);



/// Check the number of elements stored in the buffer

/// Requires: cbuf is valid and created by canCircBuff_init

/// Returns the current number of elements in the buffer

size_t canCircBuff_size(cbuf_handle_t cbuf);







#endif /* SPI_DRIVER_H_  */
