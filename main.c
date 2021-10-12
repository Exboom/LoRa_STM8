#include "stm8l15x.h"
#include "lora.h"
#include "math.h"

//Joystick defines
//----------------  
#define Joystick_ADC ADC1
#define Joystick_Y_Left ADC_Channel_5
#define Joystick_X_Left ADC_Channel_10
#define Joystick_Y_Right ADC_Channel_18
#define Joystick_X_Right ADC_Channel_23
#define Joystick_Right_Button GPIO_Pin_5
#define Joystick_Left_Button GPIO_Pin_0

//Joystick variables
//----------------  
uint16_t axisY_Left;
uint16_t axisX_Left;
uint16_t axisY_Right;
uint16_t axisX_Right;
uint16_t axis_buff[4];
volatile uint8_t push_flag;

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
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_2); // 
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
  ITC_SetSoftwarePriority(EXTI0_IRQn, ITC_PriorityLevel_3);  
  ITC_SetSoftwarePriority(EXTI5_IRQn, ITC_PriorityLevel_3);
  
  
  //LoRa/SPI GPIO Conf
  //----------------
  GPIO_Init(LORA_PORT, LORA_NSS_pin | LORA_RESET_pin, GPIO_Mode_Out_PP_High_Slow); //NSS and RESET LoRa pin's  
  GPIO_ExternalPullUpConfig(LORA_PORT, LORA_SCK_pin | LORA_MISO_pin | LORA_MOSI_pin, ENABLE);
  
  //SPI/ADC/DMA CLK Conf
  //----------------
  CLK_PeripheralClockConfig(CLK_Peripheral_SPI1, ENABLE);  
  CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);
  CLK_PeripheralClockConfig(CLK_Peripheral_DMA1, ENABLE);
  
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
           ADC_Prescaler_2);

  //SPI/ADC start
  //----------------
  SPI_Cmd(LORA_SPI, ENABLE);
  ADC_Cmd(Joystick_ADC, ENABLE);
  ADC_ChannelCmd(Joystick_ADC, Joystick_Y_Left, ENABLE);
  ADC_ChannelCmd(Joystick_ADC, Joystick_X_Left, ENABLE);
  ADC_ChannelCmd(Joystick_ADC, Joystick_Y_Right, ENABLE);
  ADC_ChannelCmd(Joystick_ADC, Joystick_X_Right, ENABLE);
  ADC_SamplingTimeConfig(Joystick_ADC, ADC_Group_SlowChannels, ADC_SamplingTime_384Cycles);
    
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
  
  ADC_SoftwareStartConv(Joystick_ADC);
  
  while(1) {
    
    ADC_ChannelCmd(Joystick_ADC, Joystick_Y_Left, ENABLE);
    ADC_SoftwareStartConv(Joystick_ADC);
    while(ADC_GetFlagStatus(Joystick_ADC, ADC_FLAG_EOC)!= SET);
    axisY_Left = ADC_GetConversionValue(Joystick_ADC);
    ADC_ChannelCmd(Joystick_ADC, Joystick_Y_Left, DISABLE);
    
    ADC_ChannelCmd(Joystick_ADC, Joystick_X_Left, ENABLE);
    ADC_SoftwareStartConv(Joystick_ADC);
    while(ADC_GetFlagStatus(Joystick_ADC, ADC_FLAG_EOC)!= SET);
    axisX_Left = ADC_GetConversionValue(Joystick_ADC);
    ADC_ChannelCmd(Joystick_ADC, Joystick_X_Left, DISABLE);
    
    ADC_ChannelCmd(Joystick_ADC, Joystick_Y_Right, ENABLE);
    ADC_SoftwareStartConv(Joystick_ADC);
    while(ADC_GetFlagStatus(Joystick_ADC, ADC_FLAG_EOC)!= SET);
    axisY_Right = ADC_GetConversionValue(Joystick_ADC);
    ADC_ChannelCmd(Joystick_ADC, Joystick_Y_Right, DISABLE);
    
    ADC_ChannelCmd(Joystick_ADC, Joystick_X_Right, ENABLE);
    ADC_SoftwareStartConv(Joystick_ADC);
    while(ADC_GetFlagStatus(Joystick_ADC, ADC_FLAG_EOC)!= SET);
    axisX_Right = ADC_GetConversionValue(Joystick_ADC);
    ADC_ChannelCmd(Joystick_ADC, Joystick_X_Right, DISABLE);
    
    joystick_cmd(&LoRa_transmitter);
  }
}

void joystick_cmd(LoRa *obj) {
  uint8_t data[11] = {0};
     
  delay_ms(50);
  
  data[0] = 43; //start bit
  
  if (push_flag == 25) {
    data[1] = 5;
    data[6] = 5; 
    LoRa_transmit(obj, data, 11, 100);
    push_flag = 0;
    return;
  }
    
  if ((axisX_Left<=150) || (axisX_Right<=150)) {  
    data[1] = 4; //left turn
    data[6] = 4; //left turn
    LoRa_transmit(obj, data, 11, 100);
    return;
  }
  if ((axisX_Left>=900) || (axisX_Right>=900)){
    data[1] = 6; //right turn
    data[6] = 6; //right turn
    LoRa_transmit(obj, data, 11, 100);
    return;
  }
  

  
  //left
  if ((axisY_Left>=400)&&(axisY_Left<=600)) {
    data[1] = 8; //stop motor
  } else if (axisY_Left<400) {
    data[1] = 7; //back move
  } else if (axisY_Left>600) {
    data[1] = 3; //forward move
  }
  if ((axisY_Left<1000)&&(axisY_Left>=100)) {
    data[2] = 0;
    data[3] = (uint8_t) floor(axisY_Left/100);
    data[4] = (uint8_t) (floor(axisY_Left/10))%10;
    data[5] = (uint8_t) axisY_Left%10;
  } else if ((axisY_Left<100)&&(axisY_Left>=10)) {
    data[2] = 0;
    data[3] = 0;
    data[4] = (uint8_t) floor(axisY_Left/10);
    data[5] = (uint8_t) axisY_Left%10;
  } else if (axisY_Left<10) {
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = (uint8_t) axisY_Left;
  } else if (axisY_Left>=1000){
    data[2] = 1;
    data[3] = 0;
    data[4] = (uint8_t) floor(axisY_Left%1000)/10;
    data[5] = (uint8_t) axisY_Left%10;
  }
  
  //right
  if ((axisY_Right>=400)&&(axisY_Right<=600)) {
    data[6] = 8; //stop motor
  } else if (axisY_Right<400) {
    data[6] = 7; //back move
  } else if (axisY_Right>600) {
    data[6] = 3; //forward move
  }
  if ((axisY_Right<1000)&&(axisY_Right>=100)) {
    data[7] = 0;
    data[8] = (uint8_t) floor(axisY_Right/100);
    data[9] = (uint8_t) floor(axisY_Right/10)%10;
    data[10] = (uint8_t) axisY_Right%10;
  } else if ((axisY_Right<100)&&(axisY_Right>=10)) {
    data[7] = 0;
    data[8] = 0;
    data[9] = (uint8_t) floor(axisY_Left/10);
    data[10] = (uint8_t) axisY_Right%10;
  } else if (axisY_Right<10) {
    data[7] = 0;
    data[8] = 0;
    data[9] = 0;
    data[10] = (uint8_t) axisY_Right;
  } else if (axisY_Right>=1000){
    data[7] = 1;
    data[8] = 0;
    data[9] = (uint8_t) floor(axisY_Left%1000)/10;
    data[10] = (uint8_t) axisY_Right%10;
  }
  LoRa_transmit(obj, data, 11, 50);
  data[0] = 0;
}

