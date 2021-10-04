#include "stm8l15x.h"
#include "lora.h"
#include "math.h"

//Joystick defines
//----------------  
#define Joystick_ADC ADC1
#define Joystick_Y_Left ADC_Channel_5
#define Joystick_X_Left ADC_Channel_6
#define Joystick_X_Right ADC_Channel_7
#define Joystick_Y_Right ADC_Channel_8
#define Joystick_Right_Button GPIO_Pin_5
#define Joystick_Left_Button GPIO_Pin_0

//Joystick variables
//----------------  
uint16_t axisY_Left;
uint16_t axisX_Left;
uint16_t axisY_Right;
uint16_t axisX_Right;

//Joystick proto
//---------------- 
static void joystick_cmd(LoRa *obj);

//LoRa structure instance
//---------------- 
LoRa LoRa_transmitter;

int main( void )
{
  //CLK Conf
  //----------------
  CLK_SYSCLKSourceSwitchCmd(ENABLE); // 
  CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI); //
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1); // 
  CLK_SYSCLKSourceSwitchCmd(DISABLE);
  CLK_RTCClockConfig(CLK_RTCCLKSource_LSI, CLK_RTCCLKDiv_1);
  
  //Joystick GPIO Conf
  //----------------
  GPIO_Init(GPIOA, GPIO_Pin_4, GPIO_Mode_Out_PP_High_Fast); //to pull up for joystic
  GPIO_ExternalPullUpConfig(GPIOA, GPIO_Pin_4, ENABLE); //to pull up for joystick
  GPIO_Init(GPIOC, Joystick_Left_Button, GPIO_Mode_In_FL_IT); //button left
  GPIO_Init(GPIOD, Joystick_Right_Button, GPIO_Mode_In_FL_IT); //button right
  EXTI_SetPinSensitivity(EXTI_Pin_0, EXTI_Trigger_Rising); //Interrupt on the time push button
  EXTI_SetPinSensitivity(EXTI_Pin_5, EXTI_Trigger_Rising); //Interrupt on the time push button
  
  //LoRa GPIO Conf
  //----------------
  GPIO_Init(LORA_PORT, LORA_NSS_pin | LORA_RESET_pin, GPIO_Mode_Out_PP_High_Slow); //NSS and RESET LoRa pin's  
  
  //SPI CLK Conf
  //----------------
  CLK_PeripheralClockConfig(CLK_Peripheral_SPI1, ENABLE);  
  
  //ADC CLK Conf
  //----------------
  CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);

  //SPI GPIO
  //----------------
  GPIO_ExternalPullUpConfig(LORA_PORT, LORA_SCK_pin | LORA_MISO_pin | LORA_MOSI_pin, ENABLE);
  
  //SPI Conf
  //----------------
  SPI_DeInit(LORA_SPI);
  SPI_Init(LORA_SPI, 
           SPI_FirstBit_MSB, 
           SPI_BaudRatePrescaler_2, 
           SPI_Mode_Master, 
           SPI_CPOL_Low, 
           SPI_CPHA_1Edge, 
           SPI_Direction_2Lines_FullDuplex, 
           SPI_NSS_Soft, 
           0x07);
  
  //ADC Conf
  //----------------
  ADC_DeInit(ADC1);
  ADC_Init(Joystick_ADC, 
           ADC_ConversionMode_Single, 
           ADC_Resolution_10Bit,
           ADC_Prescaler_1);

  //SPI start
  //----------------
  SPI_Cmd(LORA_SPI, ENABLE);
  
  //ADC start
  //----------------
  ADC_Cmd(Joystick_ADC, ENABLE);
    
  //LoRa start init
  //----------------
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
    return 0;
  }
  
  enableInterrupts();
  
  while(1) {
    //Y axis left
    ADC_ChannelCmd(Joystick_ADC, Joystick_Y_Left, ENABLE);
    ADC_SoftwareStartConv(Joystick_ADC);
    axisY_Left = ADC_GetConversionValue(Joystick_ADC);
    ADC_ChannelCmd(Joystick_ADC, Joystick_Y_Left, DISABLE);
    
    //Y axis right
    ADC_ChannelCmd(Joystick_ADC, Joystick_Y_Right, ENABLE);
    ADC_SoftwareStartConv(Joystick_ADC);
    axisY_Right = ADC_GetConversionValue(Joystick_ADC);
    ADC_ChannelCmd(Joystick_ADC, Joystick_Y_Right, DISABLE);
    
    //X axis left
    ADC_ChannelCmd(Joystick_ADC, Joystick_X_Left, ENABLE);
    ADC_SoftwareStartConv(Joystick_ADC);    
    axisX_Left = ADC_GetConversionValue(Joystick_ADC);
    ADC_ChannelCmd(Joystick_ADC, Joystick_X_Left, DISABLE);
    
    //X axis right
    ADC_ChannelCmd(Joystick_ADC, Joystick_X_Right, ENABLE);
    ADC_SoftwareStartConv(Joystick_ADC);    
    axisX_Right = ADC_GetConversionValue(Joystick_ADC);
    ADC_ChannelCmd(Joystick_ADC, Joystick_X_Right, DISABLE);
    
    joystick_cmd(&LoRa_transmitter);
  }
}

void joystick_cmd(LoRa *obj) {
  uint8_t data[12] = {0};
  //left
  data[0] = 1; //left start bit
  data[1] = 3; //longitudinal flag
  if ((axisY_Left<1000)&&(axisY_Left>=100)) {
    data[2] = 0;
    data[3] = (uint8_t) floor(axisY_Left/100);
    data[4] = (uint8_t) floor(axisY_Left/10)%10;
    data[5] = (uint8_t) axisY_Left%10;
  } else if ((axisY_Left<100)&&(axisY_Left>=10)) {
    data[2] = 0;
    data[3] = 0;
    data[4] = (uint8_t) floor(axisY_Left/10)%10;
    data[5] = (uint8_t) axisY_Left%10;
  } else if (axisY_Left<10) {
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = (uint8_t) axisY_Left;
  } else {
    data[2] = 1;
    data[3] = (uint8_t) floor(axisY_Left/100);
    data[4] = (uint8_t) floor(axisY_Left/10)%10;
    data[5] = (uint8_t) axisY_Left%10;
  }
  
  //right
  data[6] = 2; //right start bit
  data[7] = 3; //longitudinal flag
  if ((axisY_Right<1000)&&(axisY_Right>=100)) {
    data[8] = 0;
    data[9] = (uint8_t) floor(axisY_Right/100);
    data[10] = (uint8_t) floor(axisY_Right/10)%10;
    data[11] = (uint8_t) axisY_Right%10;
  } else if ((axisY_Right<100)&&(axisY_Right>=10)) {
    data[8] = 0;
    data[9] = 0;
    data[10] = (uint8_t) floor(axisY_Right/10)%10;
    data[11] = (uint8_t) axisY_Right%10;
  } else if (axisY_Right<10) {
    data[8] = 0;
    data[9] = 0;
    data[10] = 0;
    data[11] = (uint8_t) axisY_Right;
  } else {
    data[8] = 1;
    data[9] = (uint8_t) floor(axisY_Right/100);
    data[10] = (uint8_t) floor(axisY_Right/10)%10;
    data[11] = (uint8_t) axisY_Right%10;
  }
  LoRa_transmit(obj, data, 12, 100);

}

