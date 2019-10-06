/*
 * coordBuffCirc.h
 *
 *  Created on: Oct 6, 2019
 *      Author: Matias Pierdominici
 */

#ifndef COORDBUFFCIRC_H_
#define COORDBUFFCIRC_H_


#include <stdbool.h>
#include <stdlib.h>

#include <stdint.h>
#include"coordStruct.h"
#include <stddef.h>
//fuente:https://github.com/embeddedartistry/embedded-resources/blob/master/examples/c/circular_buffer/circular_buffer.h


/// Opaque circular buffer structure

typedef struct coordBuffCirc_t coordBuffCirc_t;



/// Handle type, the way users interact with the API

typedef coordBuffCirc_t* coordBuf_handle_t;



/// Pass in a storage buffer and size, returns a circular buffer handle

/// Requires: buffer is not NULL, size > 0

/// Ensures: cbuf has been created and is returned in an empty state

coordBuf_handle_t coordBuffCirc_init(coordenadasCan * buffer, size_t size);



/// Free a circular buffer structure

/// Requires: cbuf is valid and created by coordBuffCirc_init

/// Does not free data buffer; owner is responsible for that

void coordBuffCirc_free(coordBuf_handle_t cbuf);



/// Reset the circular buffer to empty, head == tail. Data not cleared

/// Requires: cbuf is valid and created by coordBuffCirc_init

void coordBuffCirc_reset(coordBuf_handle_t cbuf);



/// Put version 1 continues to add data if the buffer is full

/// Old data is overwritten

/// Requires: cbuf is valid and created by coordBuffCirc_init

void coordBuffCirc_put(coordBuf_handle_t cbuf, coordenadasCan data);



/// Put Version 2 rejects new data if the buffer is full

/// Requires: cbuf is valid and created by coordBuffCirc_init

/// Returns 0 on success, -1 if buffer is full

int coordBuffCirc_put2(coordBuf_handle_t cbuf, coordenadasCan data);



/// Retrieve a value from the buffer

/// Requires: cbuf is valid and created by coordBuffCirc_init

/// Returns 0 on success, -1 if the buffer is empty

int coordBuffCirc_get(coordBuf_handle_t cbuf, coordenadasCan * data);



/// CHecks if the buffer is empty

/// Requires: cbuf is valid and created by coordBuffCirc_init

/// Returns true if the buffer is empty

bool coordBuffCirc_empty(coordBuf_handle_t cbuf);



/// Checks if the buffer is full

/// Requires: cbuf is valid and created by coordBuffCirc_init

/// Returns true if the buffer is full

bool coordBuffCirc_full(coordBuf_handle_t cbuf);



/// Check the capacity of the buffer

/// Requires: cbuf is valid and created by coordBuffCirc_init

/// Returns the maximum capacity of the buffer

size_t coordBuffCirc_capacity(coordBuf_handle_t cbuf);



/// Check the number of elements stored in the buffer

/// Requires: cbuf is valid and created by coordBuffCirc_init

/// Returns the current number of elements in the buffer

size_t coordBuffCirc_size(coordBuf_handle_t cbuf);



#endif /* COORDBUFFCIRC_H_ */
