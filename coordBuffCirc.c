/*
 * coordBuffCirc.c
 *
 *  Created on: Oct 6, 2019
 *      Author: Matias Pierdominici
 */

#include"coordBuffCirc.h"

#include <assert.h>




// The definition of our circular buffer structure is hidden from the user

struct coordBuffCirc_t {

	coordenadasCan * buffer;

	size_t head;

	size_t tail;

	size_t max; //of the buffer

	bool full;

};



#pragma mark - Private Functions -



static void advance_pointer(coordBuf_handle_t cbuf)

{

	//assert(cbuf);



	if(cbuf->full)

    {

        cbuf->tail = (cbuf->tail + 1) % cbuf->max;

    }



	cbuf->head = (cbuf->head + 1) % cbuf->max;



	// We mark full because we will advance tail on the next time around

	cbuf->full = (cbuf->head == cbuf->tail);

}



static void retreat_pointer(coordBuf_handle_t cbuf)

{

	//assert(cbuf);



	cbuf->full = false;

	cbuf->tail = (cbuf->tail + 1) % cbuf->max;

}



#pragma mark - APIs -



coordBuf_handle_t coordBuffCirc_init(coordenadasCan * buffer, size_t size)

{

	//assert(buffer && size);



	coordBuf_handle_t cbuf = malloc(sizeof(coordBuffCirc_t));

	//assert(cbuf);



	cbuf->buffer = buffer;

	cbuf->max = size;

	coordBuffCirc_reset(cbuf);



	//assert(coordBuffCirc_empty(cbuf));



	return cbuf;

}



void coordBuffCirc_free(coordBuf_handle_t cbuf)

{

	//assert(cbuf);

	free(cbuf);

}



void coordBuffCirc_reset(coordBuf_handle_t cbuf)

{

  //  assert(cbuf);



    cbuf->head = 0;

    cbuf->tail = 0;

    cbuf->full = false;

}



size_t coordBuffCirc_size(coordBuf_handle_t cbuf)

{

//	assert(cbuf);



	size_t size = cbuf->max;



	if(!cbuf->full)

	{

		if(cbuf->head >= cbuf->tail)

		{

			size = (cbuf->head - cbuf->tail);

		}

		else

		{

			size = (cbuf->max + cbuf->head - cbuf->tail);

		}



	}



	return size;

}



size_t coordBuffCirc_capacity(coordBuf_handle_t cbuf)

{

//	assert(cbuf);



	return cbuf->max;

}



void coordBuffCirc_put(coordBuf_handle_t cbuf, coordenadasCan data)

{

//	assert(cbuf && cbuf->buffer);



    cbuf->buffer[cbuf->head] = data;



    advance_pointer(cbuf);

}



int coordBuffCirc_put2(coordBuf_handle_t cbuf, coordenadasCan data)

{

    int r = -1;



//    assert(cbuf && cbuf->buffer);



    if(!coordBuffCirc_full(cbuf))

    {

        cbuf->buffer[cbuf->head] = data;

        advance_pointer(cbuf);

        r = 0;

    }



    return r;

}



int coordBuffCirc_get(coordBuf_handle_t cbuf, coordenadasCan * data)

{

 //   assert(cbuf && data && cbuf->buffer);



    int r = -1;



    if(!coordBuffCirc_empty(cbuf))

    {

        *data = cbuf->buffer[cbuf->tail];

        retreat_pointer(cbuf);



        r = 0;

    }



    return r;

}



bool coordBuffCirc_empty(coordBuf_handle_t cbuf)

{

//	assert(cbuf);



    return (!cbuf->full && (cbuf->head == cbuf->tail));

}



bool coordBuffCirc_full(coordBuf_handle_t cbuf)

{

//	assert(cbuf);



    return cbuf->full;

}
