#include "stm8l15x.h"
#include "lora.h"

//#define LORA_MISO_pin GPIO_Pin_7
//#define LORA_MOSI_pin GPIO_Pin_6
//#define LORA_SCK_pin GPIO_Pin_5
//#define LORA_NSS_pin GPIO_Pin_4
//#define LORA_RESET_pin GPIO_Pin_3
//#define LORA_DIO0_pin GPIO_Pin_2
//#define LORA_PORT GPIOB
//#define LORA_SPI SPI1


int main( void )
{
  //CLK Conf
  //----------------
  CLK_SYSCLKSourceSwitchCmd(ENABLE); // 
  CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI); //
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1); // 
  CLK_SYSCLKSourceSwitchCmd(DISABLE);
  CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_1);
  
  //LoRa GPIO Conf
  //----------------
  GPIO_Init(LORA_PORT, LORA_NSS_pin | LORA_RESET_pin, GPIO_Mode_Out_PP_High_Slow); //NSS and RESET LoRa pin's  
  
  //SPI CLK Conf
  //----------------
  CLK_PeripheralClockConfig(CLK_Peripheral_SPI1, ENABLE);  

  //SPI GPIO
  //----------------
  GPIO_ExternalPullUpConfig(LORA_PORT, LORA_SCK_pin | LORA_MISO_pin | LORA_MOSI_pin, ENABLE);
  
  //SPI Conf
  //----------------
  SPI_Init(LORA_SPI, 
           SPI_FirstBit_MSB, 
           SPI_BaudRatePrescaler_2, 
           SPI_Mode_Master, 
           SPI_CPOL_High, 
           SPI_CPHA_1Edge, 
           SPI_Direction_2Lines_FullDuplex, 
           SPI_NSS_Soft, 
           0x07);

  //SPI start
  //----------------
  SPI_Cmd(LORA_SPI, ENABLE);
  
  LoRa LoRa_transmitter;
  LoRa_transmitter = newLoRa();
  LoRa_transmitter.overCurrentProtection = 120;
  LoRa_transmitter.preamble = 10;
  
  LoRa_reset();
  
  uint8_t lora_status;
  lora_status = LoRa_init(&LoRa_transmitter);
  if (lora_status == 200) {
    GPIO_Init(GPIOE, GPIO_Pin_7, GPIO_Mode_Out_PP_High_Fast);
  } else {
    GPIO_Init(GPIOC, GPIO_Pin_7, GPIO_Mode_Out_PP_High_Fast);
  }
  
  return 0;
}
