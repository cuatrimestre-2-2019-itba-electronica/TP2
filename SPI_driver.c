/*
 * SPI_driver.c
 *
 *  Created on: Sep 20, 2019
 *      Author: gonzalosilva
 */
#include "board.h"
#include "MK64F12.h"

#define SPI_DRIVER_MODE		2 //alternativa
#define SPI_DRIVER_INPUT	1


typedef uint8_t pin_t;

static bool trasmiting=false;

static SPI_Type *SPI_driver_SPI[] = {SPI0, SPI1, SPI2};
static void setup_pin (pin_t pin);

static PORT_Type * ports[] = PORT_BASE_PTRS;
static uint32_t sim_port[] = {SIM_SCGC5_PORTA_MASK, SIM_SCGC5_PORTB_MASK, SIM_SCGC5_PORTC_MASK, SIM_SCGC5_PORTD_MASK, SIM_SCGC5_PORTE_MASK};

static void startTrasmissionReception(void){
	SPI_driver_SPI[0]->MCR =(SPI_driver_SPI[0]->MCR & (~SPI_MCR_HALT_MASK))| SPI_MCR_HALT(0);
}
static void stopTrasmissionReception(void){
	SPI_driver_SPI[0]->MCR =(SPI_driver_SPI[0]->MCR & (~SPI_MCR_HALT_MASK))| SPI_MCR_HALT(1);
}


void SPI_driver_init (void){
	setup_pin(PIN_SCK);
	setup_pin(PIN_MOSI);
	setup_pin(PIN_MISO);

	SIM->SCGC6 |= SIM_SCGC6_SPI0(1); //habilite el clock del periferico de spi

	SPI_driver_SPI[0]->MCR=SPI_MCR_HALT_MASK|SPI_MCR_MSTR_MASK;

	SPI_driver_SPI[0]->CTAR[0] = SPI_CTAR_CPOL(0) |  \
								 SPI_CTAR_CPHA(0)| \
								 SPI_CTAR_PBR(2) | \
								 SPI_CTAR_BR(1) | \
								 SPI_CTAR_FMSZ(8-1);
			//configuracion del SPI 0

	SPI_driver_SPI[0]->MCR =(SPI_driver_SPI[0]->MCR & (~(SPI_MCR_MDIS_MASK | SPI_MCR_HALT_MASK | SPI_MCR_MSTR_MASK))) \
														| SPI_MCR_MDIS(0) |SPI_MCR_HALT(0) | SPI_MCR_MSTR(1);
	stopTrasmissionReception();

}

void setup_pin (pin_t pin){
	int pinPort = PIN2PORT(pin);
	int pinBit = PIN2NUM(pin);
	SIM->SCGC5 |= sim_port[pinPort]; //Habilito el clock al puerto correspondiente

	//configuro el pcr del pin
	ports[pinPort]->PCR[pinBit]= PORT_PCR_MUX(SPI_DRIVER_MODE) | \
			PORT_PCR_IRQC(0) | \
			PORT_PCR_PS(0)| \
			PORT_PCR_PE(0);


}


uint8_t SPI_driver_send(uint8_t * data2end, uint8_t size){

	if(!((SPI_driver_SPI[0]->SR & SPI_SR_TFFF_MASK)==0)){
		stopTrasmissionReception();
		SPI_driver_SPI[0]->SR=SPI_SR_TFFF_MASK;

		for(int i=0;i<size;i++){
			if((SPI_driver_SPI[0]->SR & SPI_SR_TFFF_MASK)==0){
				return i;
			}
			SPI_driver_SPI[0]->PUSHR=(SPI_driver_SPI[0]->PUSHR&(~SPI_PUSHR_TXDATA_MASK))| SPI_PUSHR_TXDATA(data2end[i]);
		}
		trasmiting=true;
		startTrasmissionReception();
		return size;
	}
	return 0;

}
bool SPI_driver_dataSended(void){
	if(trasmiting){
		if(SPI_driver_SPI[0]->SR & SPI_SR_EOQF_MASK){
			SPI_driver_SPI[0]->SR = SPI_SR_EOQF_MASK;//borro el flag
			trasmiting=false;
		}
	}
	return !trasmiting;
}
bool SPI_driver_availableDataRecived(void){
	return true;
}
uint8_t SPI_driver_getData(uint8_t * dataRecived){
	uint8_t amauntOfData=0;
	if(trasmiting){
		return 0;
	}

	amauntOfData=(SPI_driver_SPI[0]->SR & SPI_SR_RXCTR_MASK)>>SPI_SR_RXCTR_SHIFT;
	if(amauntOfData!=0){
		for(int i=0;i<amauntOfData;i++){
			dataRecived[i]=SPI_driver_SPI[0]->POPR;
		}
	}

	return amauntOfData;
}
//void SPI0_IRQHandler(void)
