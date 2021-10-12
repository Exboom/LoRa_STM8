/*
 * lora.c
 *
 *  Created on: Sep 23, 2021
 *      Author: Andrew
 */

#include "lora.h"


/* ----------------------------------------------------------------------------- *\
		name        : newLoRa
 description : it's a constructor for LoRa structure that assign default values
 and pass created object (LoRa struct instanse)
 arguments   : Nothing
 returns     : A LoRa object whit these default values:
 ----------------------------------------
 |   carrier frequency = 433 MHz        |
 |    spreading factor = 7				       |
 |           bandwidth = 125 KHz        |
 | 		    coding rate = 4/5            |
 ----------------------------------------
 \* ----------------------------------------------------------------------------- */
LoRa newLoRa() {
	LoRa newobj;

	newobj.frequency = 433;
	newobj.spredingFactor = SF_7;
	newobj.bandWidth = BW_125KHz;
	newobj.crcRate = CR_4_5;
	newobj.power = POWER_20db;
	newobj.overCurrentProtection = 100;
	newobj.preamble = 8;

	return newobj;
}
/* ----------------------------------------------------------------------------- *\
		name        : LoRa_reset
 description : reset module
 arguments   :
 LoRa* LoRa --> LoRa object handler
 returns     : Nothing
 \* ----------------------------------------------------------------------------- */
void LoRa_reset( ) {
  
	GPIO_ResetBits(LORA_PORT, LORA_RESET_pin);
	delay_ms(1);
        GPIO_SetBits(LORA_PORT, LORA_RESET_pin);
	delay_ms(100);
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_gotoMode
 description : set LoRa Op mode
 arguments   :
 LoRa* LoRa    --> LoRa object handler
 mode	        --> select from defined modes
 returns     : Nothing
 \* ----------------------------------------------------------------------------- */
void LoRa_gotoMode(LoRa *obj, int mode) {
	uint8_t read;
	uint8_t data;

	read = LoRa_read(RegOpMode);
	data = read;

	switch (mode) {
	case SLEEP_MODE:
		data = (read & 0xF8) | 0x00;
		obj->current_mode = SLEEP_MODE;
		break;
	case STNBY_MODE:
		data = (read & 0xF8) | 0x01;
		obj->current_mode = STNBY_MODE;
		break;
	case TRANSMIT_MODE:
		data = (read & 0xF8) | 0x03;
		obj->current_mode = TRANSMIT_MODE;
		break;
	case RXCONTIN_MODE:
		data = (read & 0xF8) | 0x05;
		obj->current_mode = RXCONTIN_MODE;
		break;
	case RXSINGLE_MODE:
		data = (read & 0xF8) | 0x06;
		obj->current_mode = RXSINGLE_MODE;
		break;
	default:
		break;
	}

	LoRa_write(RegOpMode, data);
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_readReg
 description : read a register(s) by an address and a length,
 then store value(s) at outpur array.
 arguments   :
 LoRa* LoRa        --> LoRa object handler
 uint8_t* address  -->	pointer to the beginning of address array
 uint16_t r_length -->	detemines number of addresse bytes that
 you want to send
 uint8_t* output		--> pointer to the beginning of output array
 uint16_t w_length	--> detemines number of bytes that you want to read
 returns     : Nothing
 \* ----------------------------------------------------------------------------- */
void LoRa_readReg(LoRa *obj, uint8_t *address, uint8_t *output) {
//        GPIO_ResetBits(obj->CS_port, obj->CS_pin);
//        SPI_SendData(obj->hSPIx, *address);
//        while (SPI_GetFlagStatus(obj->hSPIx, SPI_FLAG_RXNE) != SET);
//        output = (uint8_t *) SPI_ReceiveData(obj->hSPIx);
//        while ((SPI1->SR & SPI_FLAG_TXE) == RESET)
//              ;
//        SPI1->DR = *address;
//        while ((SPI1->SR & SPI_FLAG_RXNE) == RESET)
//              ;
//        *output = SPI1->DR;
//        GPIO_SetBits(obj->CS_port, obj->CS_pin);
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_writeReg
 description : write a value(s) in a register(s) by an address
 arguments   :
 LoRa* LoRa        --> LoRa object handler
 uint8_t* address  -->	pointer to the beginning of address array
 uint16_t r_length -->	detemines number of addresse bytes that
 you want to send
 uint8_t* output		--> pointer to the beginning of values array
 uint16_t w_length	--> detemines number of bytes that you want to send
 returns     : Nothing
 \* ----------------------------------------------------------------------------- */
void LoRa_writeReg(LoRa *obj, uint8_t *address, uint8_t *values) {
//        GPIO_ResetBits(obj->CS_port, obj->CS_pin);
//        uint8_t resp;
//	SPI_SendData(obj->hSPIx, *address);
//	while (SPI_GetFlagStatus(obj->hSPIx, SPI_FLAG_TXE) != SET);
//	SPI_SendData(obj->hSPIx, *values);
//	while (SPI_GetFlagStatus(obj->hSPIx, SPI_FLAG_TXE) != SET);
//        while ((SPI1->SR & SPI_FLAG_TXE) == RESET)
//              ;
//        SPI1->DR = *address;
//        while ((SPI1->SR & SPI_FLAG_RXNE) == RESET)
//              ;
//        resp = SPI1->DR;
//        while ((SPI1->SR & SPI_FLAG_TXE) == RESET)
//              ;
//        SPI1->DR = *values;
//        while ((SPI1->SR & SPI_FLAG_RXNE) == RESET)
//              ;
//        resp = SPI1->DR;
//	GPIO_SetBits(obj->CS_port, obj->CS_pin);
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_setFrequency
 description : set carrier frequency e.g 433 MHz
 arguments   :
 LoRa* LoRa        --> LoRa object handler
 int   freq        --> desired frequency in MHz unit, e.g 434
 returns     : Nothing
 \* ----------------------------------------------------------------------------- */
void LoRa_setFrequency(int freq) {
	uint8_t data;
	uint32_t F;
	F = (freq * 524288) >> 5;

	// write Msb:
	data = F >> 16;
	LoRa_write(RegFrMsb, data);
	delay_ms(5);

	// write Mid:
	data = F >> 8;
	LoRa_write(RegFrMid, data);
	delay_ms(5);

	// write Lsb:
	data = F >> 0;
	LoRa_write(RegFrLsb, data);
	delay_ms(5);
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_setSpreadingFactor
 description : set spreading factor, from 7 to 12.
 arguments   :
 LoRa* LoRa        --> LoRa object handler
 int   SP          --> desired spreading factor e.g 7
 returns     : Nothing
 \* ----------------------------------------------------------------------------- */
void LoRa_setSpreadingFactor(int SF) {
	uint8_t data;
	uint8_t read;

	if (SF > 12)
		SF = 12;
	if (SF < 7)
		SF = 7;

	read = LoRa_read(RegModemConfig2);
	delay_ms(10);

	data = (SF << 4) + (read & 0x0F);
	LoRa_write(RegModemConfig2, data);
	delay_ms(10);
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_setPower
 description : set power gain.
 arguments   :
 LoRa* LoRa        --> LoRa object handler
 int   power       --> desired power e.g POWER_17db
 returns     : Nothing
 \* ----------------------------------------------------------------------------- */
void LoRa_setPower(uint8_t power) {
	LoRa_write(RegPaConfig, power);
	delay_ms(10);
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_setOCP
 description : set maximum allowed current.
 arguments   :
 LoRa* LoRa        --> LoRa object handler
 int   current     --> desired max currnet in mA, e.g 120
 returns     : Nothing
 \* ----------------------------------------------------------------------------- */
void LoRa_setOCP(uint8_t current) {
	uint8_t OcpTrim = 0;

	if (current < 45)
		current = 45;
	if (current > 240)
		current = 240;

	if (current <= 120)
		OcpTrim = (current - 45) / 5;
	else if (current <= 240)
		OcpTrim = (current + 30) / 10;

	OcpTrim = OcpTrim + (1 << 5);
	LoRa_write(RegOcp, OcpTrim);
	delay_ms(10);
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_setTOMsb_setCRCon
 description : set timeout msb to 0xFF + set CRC enable.
 arguments   :
 LoRa* LoRa        --> LoRa object handler
 returns     : Nothing
 \* ----------------------------------------------------------------------------- */
void LoRa_setTOMsb_setCRCon(void) {
	uint8_t read, data;

	read = LoRa_read(RegModemConfig2);

	data = read | 0x07;
	LoRa_write(RegModemConfig2, data);\

	delay_ms(10);
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_read
 description : read a register by an address
 arguments   :
 LoRa*   LoRa        --> LoRa object handler
 uint8_t address     -->	address of the register e.g 0x1D
 returns     : register value
 \* ----------------------------------------------------------------------------- */
uint8_t LoRa_read(uint8_t address) {
	uint8_t read_data;
	uint8_t data_addr;
	data_addr = address & 0x7F;
        
        GPIO_ResetBits(LORA_PORT, LORA_NSS_pin);
        
        while ((SPI1->SR & SPI_FLAG_TXE) == RESET);
        SPI_SendData(LORA_SPI, data_addr);
        while ((SPI1->SR & SPI_FLAG_RXNE) == RESET);
        read_data = SPI_ReceiveData(LORA_SPI);
        while ((SPI1->SR & SPI_FLAG_TXE) == RESET);
        SPI_SendData(LORA_SPI, 0xFF);
        while ((SPI1->SR & SPI_FLAG_RXNE) == RESET);
        read_data = SPI_ReceiveData(LORA_SPI);
        
        GPIO_SetBits(LORA_PORT, LORA_NSS_pin);
        
	return read_data;
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_write
 description : write a value in a register by an address
 arguments   :
 LoRa*   LoRa        --> LoRa object handler
 uint8_t address     -->	address of the register e.g 0x1D
 uint8_t value       --> value that you want to write
 returns     : Nothing
 \* ----------------------------------------------------------------------------- */
void LoRa_write(uint8_t address, uint8_t value) {
	uint8_t data;
	uint8_t addr;
        uint8_t resp;
        
	addr = address | 0x80;
	data = value;
        
        GPIO_ResetBits(LORA_PORT, LORA_NSS_pin);
        
        while ((SPI1->SR & SPI_FLAG_TXE) == RESET);
	SPI_SendData(LORA_SPI, addr);
        while ((SPI1->SR & SPI_FLAG_RXNE) == RESET);
        resp = SPI_ReceiveData(LORA_SPI);
        while ((SPI1->SR & SPI_FLAG_TXE) == RESET);
        SPI_SendData(LORA_SPI, data);
        while ((SPI1->SR & SPI_FLAG_RXNE) == RESET);
        resp = SPI_ReceiveData(LORA_SPI);
        
	GPIO_SetBits(LORA_PORT, LORA_NSS_pin);
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_BurstWrite
 description : write a set of values in a register by an address respectively
 arguments   :
 LoRa*   LoRa        --> LoRa object handler
 uint8_t address     -->	address of the register e.g 0x1D
 uint8_t *value      --> address of values that you want to write
 returns     : Nothing
 \* ----------------------------------------------------------------------------- */
void LoRa_BurstWrite(uint8_t address, uint8_t *value) {
	uint8_t addr;
        uint8_t resp;
	addr = address | 0x80;

	//NSS = 1
        GPIO_ResetBits(LORA_PORT, LORA_NSS_pin);
        //say module thai I want to write in RegFiFo
        while ((SPI1->SR & SPI_FLAG_TXE) == RESET);
        SPI_SendData(LORA_SPI, addr);
        while ((SPI1->SR & SPI_FLAG_RXNE) == RESET);
        resp = SPI_ReceiveData(LORA_SPI);
        //Write data in FiFo
        while ((SPI1->SR & SPI_FLAG_TXE) == RESET);
        SPI_SendData(LORA_SPI, *value);
        while ((SPI1->SR & SPI_FLAG_RXNE) == RESET);
        resp = SPI_ReceiveData(LORA_SPI);
        
        //NSS = 0
	GPIO_SetBits(LORA_PORT, LORA_NSS_pin);
}
/* ----------------------------------------------------------------------------- *\
		name        : LoRa_isvalid
 description : check the LoRa instruct values
 arguments   :
 LoRa* LoRa --> LoRa object handler
 returns     : returns 1 if all of the values were given, otherwise returns 0
 \* ----------------------------------------------------------------------------- */
uint8_t LoRa_isvalid(void) {

	return 1;
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_transmit
 description : Transmit data
 arguments   :
 LoRa*    LoRa     --> LoRa object handler
 uint8_t  data			--> A pointer to the data you wanna send
 uint8_t	 length   --> Size of your data in Bytes
 uint16_t timeOut	--> Timeout in milliseconds
 returns     : 1 in case of success, 0 in case of timeout
 \* ----------------------------------------------------------------------------- */
uint8_t LoRa_transmit(LoRa *obj, uint8_t *data, uint8_t length, uint8_t timeout) {
	uint8_t read;

	int mode = obj->current_mode;
        //standby mode
	LoRa_gotoMode(obj, STNBY_MODE);
	read = LoRa_read(RegFiFoTxBaseAddr);
	LoRa_write(RegFiFoAddPtr, read);
	LoRa_write(RegPayloadLength, length);
	for (int i=0; i < length; i++) {
          LoRa_write(RegFiFo, data[i]);
        }
//        LoRa_BurstWrite(RegFiFo, data);
	LoRa_gotoMode(obj, TRANSMIT_MODE);
	while (1) {
		read = LoRa_read(RegIrqFlags);
		if ((read & 0x08) != 0) {
			LoRa_write(RegIrqFlags, 0xFF);
			LoRa_gotoMode(obj, mode);
			return 1;
		} else {
			if (--timeout == 0) {
				LoRa_gotoMode(obj, mode);
				return 0;
			}
		}
		delay_ms(1);
	}

}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_startReceiving
 description : Start receiving continuously
 arguments   :
 LoRa*    LoRa     --> LoRa object handler
 returns     : Nothing
 \* ----------------------------------------------------------------------------- */
void LoRa_startReceiving(LoRa *obj) {
	LoRa_gotoMode(obj, RXCONTIN_MODE);
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_Receive
 description : Read received data from module
 arguments   :
 LoRa*    LoRa     --> LoRa object handler
 uint8_t  data			--> A pointer to the array that you want to write bytes in it
 uint8_t	 length   --> Determines how many bytes you want to read
 returns     : The number of bytes received
 \* ----------------------------------------------------------------------------- */
uint8_t LoRa_receive(LoRa *obj, uint8_t *data, uint8_t length) {
	uint8_t read;
	uint8_t number_of_bytes;
	uint8_t min = 0;

	for (int i = 0; i < length; i++)
		data[i] = 0;

	LoRa_gotoMode(obj, STNBY_MODE);
	read = LoRa_read(RegIrqFlags);
	if ((read & 0x40) != 0) {
		LoRa_write(RegIrqFlags, 0xFF);
		number_of_bytes = LoRa_read(RegRxNbBytes);
		read = LoRa_read(RegFiFoRxCurrentAddr);
		LoRa_write(RegFiFoAddPtr, read);
		min = length >= number_of_bytes ? number_of_bytes : length;
		for (int i = 0; i < min; i++)
			data[i] = LoRa_read(RegFiFo);
	}
	LoRa_gotoMode(obj, RXCONTIN_MODE);
	return min;
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_getRSSI
 description : initialize and set the right setting according to LoRa sruct vars
 arguments   :
 LoRa* LoRa        --> LoRa object handler
 returns     : Returns the RSSI value of last received packet.
 \* ----------------------------------------------------------------------------- */
int LoRa_getRSSI(void) {
	uint8_t read;
	read = LoRa_read(RegPktRssiValue);
	return -164 + read;
}

/* ----------------------------------------------------------------------------- *\
		name        : LoRa_init
 description : initialize and set the right setting according to LoRa sruct vars
 arguments   :
 LoRa* LoRa        --> LoRa object handler
 returns     : Nothing
 \* ----------------------------------------------------------------------------- */
uint8_t LoRa_init(LoRa *obj) {
	uint8_t data;
	uint8_t read;

	if (LoRa_isvalid()) {
		// goto sleep mode:
		LoRa_gotoMode(obj, SLEEP_MODE);
		delay_ms(10);

		// turn on lora mode:
		read = LoRa_read(RegOpMode);
		delay_ms(10);
		data = read | 0x80;
		LoRa_write(RegOpMode, data);
		delay_ms(100);

		// set frequency:
		LoRa_setFrequency(obj->frequency);

		// set output power gain:
		LoRa_setPower(obj->power);

		// set over current protection:
		LoRa_setOCP(obj->overCurrentProtection);

		// set LNA gain:
		LoRa_write(RegLna, 0x23);

		// set spreading factor, CRC on, and Timeout Msb:
		LoRa_setTOMsb_setCRCon();
		LoRa_setSpreadingFactor(obj->spredingFactor);

		// set Timeout Lsb:
		LoRa_write(RegSymbTimeoutL, 0xFF);

		// set bandwidth, coding rate and expilicit mode:
		// 8 bit RegModemConfig --> | X | X | X | X | X | X | X | X |
		//       bits represent --> |   bandwidth   |     CR    |I/E|
		data = 0;
		data = (obj->bandWidth << 4) + (obj->crcRate << 1);
		LoRa_write(RegModemConfig1, data);

		// set preamble:
		LoRa_write(RegPreambleMsb, obj->preamble >> 8);
		LoRa_write(RegPreambleLsb, obj->preamble >> 0);

		// DIO mapping:   --> DIO: RxDone
		read = LoRa_read(RegDioMapping1);
		data = read | 0x3F;
		LoRa_write(RegDioMapping1, data);

		// goto standby mode:
		LoRa_gotoMode(obj, STNBY_MODE);
		obj->current_mode = STNBY_MODE;
		delay_ms(10);

		read = LoRa_read(RegVersion);
		if (read == 0x12)
			return LORA_OK;
		else
			return LORA_NOT_FOUND;
	} else {
		return LORA_UNAVAILABLE;
	}
}
