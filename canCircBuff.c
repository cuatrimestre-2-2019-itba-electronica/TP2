#include"canCircBuff.h"



#include <assert.h>




// The definition of our circular buffer structure is hidden from the user

struct canCircBuff_t {

	canDataStruct * buffer;

	size_t head;

	size_t tail;

	size_t max; //of the buffer

	bool full;

};



#pragma mark - Private Functions -



static void advance_pointer(cbuf_handle_t cbuf)

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



static void retreat_pointer(cbuf_handle_t cbuf)

{

	//assert(cbuf);



	cbuf->full = false;

	cbuf->tail = (cbuf->tail + 1) % cbuf->max;

}



#pragma mark - APIs -



cbuf_handle_t canCircBuff_init(canDataStruct * buffer, size_t size)

{

	//assert(buffer && size);



	cbuf_handle_t cbuf = malloc(sizeof(canCircBuff_t));

	//assert(cbuf);



	cbuf->buffer = buffer;

	cbuf->max = size;

	canCircBuff_reset(cbuf);



	//assert(canCircBuff_empty(cbuf));



	return cbuf;

}



void canCircBuff_free(cbuf_handle_t cbuf)

{

	//assert(cbuf);

	free(cbuf);

}



void canCircBuff_reset(cbuf_handle_t cbuf)

{

  //  assert(cbuf);



    cbuf->head = 0;

    cbuf->tail = 0;

    cbuf->full = false;

}



size_t canCircBuff_size(cbuf_handle_t cbuf)

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



size_t canCircBuff_capacity(cbuf_handle_t cbuf)

{

//	assert(cbuf);



	return cbuf->max;

}



void canCircBuff_put(cbuf_handle_t cbuf, canDataStruct data)

{

//	assert(cbuf && cbuf->buffer);



    cbuf->buffer[cbuf->head] = data;



    advance_pointer(cbuf);

}



int canCircBuff_put2(cbuf_handle_t cbuf, canDataStruct data)

{

    int r = -1;



//    assert(cbuf && cbuf->buffer);



    if(!canCircBuff_full(cbuf))

    {

        cbuf->buffer[cbuf->head] = data;

        advance_pointer(cbuf);

        r = 0;

    }



    return r;

}



int canCircBuff_get(cbuf_handle_t cbuf, canDataStruct * data)

{

 //   assert(cbuf && data && cbuf->buffer);



    int r = -1;



    if(!canCircBuff_empty(cbuf))

    {

        *data = cbuf->buffer[cbuf->tail];

        retreat_pointer(cbuf);



        r = 0;

    }



    return r;

}



bool canCircBuff_empty(cbuf_handle_t cbuf)

{

//	assert(cbuf);



    return (!cbuf->full && (cbuf->head == cbuf->tail));

}



bool canCircBuff_full(cbuf_handle_t cbuf)

{

//	assert(cbuf);



    return cbuf->full;

}
