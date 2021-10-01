/*
 * lora.h
 *
 *  Created on: Sep 23, 2021
 *      Author: Andrew
 */

#include "stm8l15x_gpio.h"
#include "stm8l15x_spi.h"
#include "delay.h"

#ifndef LORA_LORA_H_
#define LORA_LORA_H_

#define TRANSMIT_TIMEOUT	2000
#define RECEIVE_TIMEOUT		2000

//--------HARDWARE---------//
#define LORA_MISO_pin GPIO_Pin_7
#define LORA_MOSI_pin GPIO_Pin_6
#define LORA_SCK_pin GPIO_Pin_5
#define LORA_NSS_pin GPIO_Pin_4
#define LORA_RESET_pin GPIO_Pin_3
#define LORA_DIO0_pin GPIO_Pin_2
#define LORA_PORT GPIOB
#define LORA_SPI SPI1

//--------- MODES ---------//
#define SLEEP_MODE				0
#define	STNBY_MODE				1
#define TRANSMIT_MODE			3
#define RXCONTIN_MODE			5
#define RXSINGLE_MODE			6

//------- BANDWIDTH -------//
#define BW_7_8KHz					0
#define BW_10_4KHz				1
#define BW_15_6KHz				2
#define BW_20_8KHz				3
#define BW_31_25KHz				4
#define BW_41_7KHz				5
#define BW_62_5KHz				6
#define BW_125KHz					7
#define BW_250KHz					8
#define BW_500KHz					9

//------ CODING RATE ------//
#define CR_4_5						1
#define CR_4_6						2
#define CR_4_7						3
#define CR_4_8						4

//--- SPREADING FACTORS ---//
#define SF_7							7
#define SF_8							8
#define SF_9							9
#define SF_10							10
#define SF_11  						11
#define SF_12							12

//------ POWER GAIN ------//
#define POWER_11db				0xF6
#define POWER_14db				0xF9
#define POWER_17db				0xFC
#define POWER_20db				0xFF

//------- REGISTERS -------//
#define RegFiFo								0x00
#define RegOpMode							0x01
#define RegFrMsb							0x06
#define RegFrMid							0x07
#define RegFrLsb							0x08
#define RegPaConfig						0x09
#define RegOcp								0x0B
#define RegLna								0x0C
#define RegFiFoAddPtr					0x0D
#define RegFiFoTxBaseAddr			0x0E
#define RegFiFoRxBaseAddr			0x0F
#define RegFiFoRxCurrentAddr	0x10
#define RegIrqFlags						0x12
#define RegRxNbBytes					0x13
#define RegPktRssiValue				0x1A
#define	RegModemConfig1				0x1D
#define RegModemConfig2				0x1E
#define RegSymbTimeoutL				0x1F
#define RegPreambleMsb				0x20
#define RegPreambleLsb				0x21
#define RegPayloadLength			0x22
#define RegDioMapping1				0x40
#define RegDioMapping2				0x41
#define RegVersion						0x42

//------ LORA STATUS ------//
#define LORA_OK							200
#define LORA_NOT_FOUND			404
#define LORA_LARGE_PAYLOAD	413
#define LORA_UNAVAILABLE		503

typedef struct LoRa_setting {

	// Hardware setings:
//	GPIO_TypeDef *CS_port;
//	uint16_t CS_pin;
//	GPIO_TypeDef *reset_port;
//	uint16_t reset_pin;
//	GPIO_TypeDef *DIO0_port;
//	uint16_t DIO0_pin;
//	SPI_HandleTypeDef *hSPIx;
        
//        GPIO_TypeDef *CS_port;
//	uint8_t CS_pin;
//	GPIO_TypeDef *reset_port;
//	uint8_t reset_pin;
//	GPIO_TypeDef *DIO0_port;
//	uint8_t DIO0_pin;
//	SPI_TypeDef *hSPIx;

	// Module settings:
	int current_mode;
	int frequency;
	uint8_t spredingFactor;
	uint8_t bandWidth;
	uint8_t crcRate;
	uint8_t preamble;
	uint8_t power;
	uint8_t overCurrentProtection;

} LoRa;

LoRa newLoRa(void);
void LoRa_reset(void);
//void LoRa_readReg(LoRa *obj, uint8_t *address, uint8_t *output);
//void LoRa_writeReg(LoRa *obj, uint8_t *address, uint8_t *values);
void LoRa_gotoMode(LoRa *obj, int mode);
uint8_t LoRa_read(uint8_t address);
void LoRa_write(uint8_t address, uint8_t value);
void LoRa_BurstWrite(uint8_t address, uint8_t *value);
uint8_t LoRa_isvalid(void);

void LoRa_setFrequency(int freq);
void LoRa_setSpreadingFactor(int SF);
void LoRa_setPower(uint8_t power);
void LoRa_setOCP(uint8_t current);
void LoRa_setTOMsb_setCRCon(void);
uint8_t LoRa_transmit(LoRa *obj, uint8_t *data, uint8_t length, uint8_t timeout);
void LoRa_startReceiving(LoRa *obj);
uint8_t LoRa_receive(LoRa *obj, uint8_t *data, uint8_t length);
void LoRa_receive_IT(LoRa *obj, uint8_t *data, uint8_t length);
int LoRa_getRSSI(void);

uint8_t LoRa_init(LoRa *obj);

#endif /* LORA_LORA_H_ */
