#include "stm32f0xx_conf.h"

void SysTick_Handler(void) 
{
  static uint16_t tick = 0;

  switch (tick++) {
    case 100:
      tick = 0;
      //GPIOC->ODR ^= (1 << 8);
      break;
  }
}

int main(void)
{
  USART_InitTypeDef USART2_InitStructure;
  USART_InitTypeDef USART1_InitStructure;
  GPIO_InitTypeDef GPIOA_InitStructure;
  GPIO_InitTypeDef GPIOB_InitStructure;
  GPIO_InitTypeDef GPIOC_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);

  // Configure USART2, USART1 pins:  Rx Only, PA3, PA10
  GPIOA_InitStructure.GPIO_Pin =  GPIO_Pin_3 | GPIO_Pin_10;
  GPIOA_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIOA_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIOA_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIOA_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIOA_InitStructure);
  
  // Enable GPIOB to convert the Serial data to parallel for probing
  GPIOB_InitStructure.GPIO_Pin =  GPIO_Pin_All;
  GPIOB_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIOB_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIOB_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIOB_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOB, &GPIOB_InitStructure);
  
  // Enable GPIOC to output CLK whenever new data arrives
  GPIOC_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9;
  GPIOC_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIOC_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIOC_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIOC_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIOC_InitStructure);
  
  USART2_InitStructure.USART_BaudRate = 19200;
  USART2_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART2_InitStructure.USART_StopBits = USART_StopBits_1;
  USART2_InitStructure.USART_Parity = USART_Parity_No;
  USART2_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART2_InitStructure.USART_Mode = USART_Mode_Rx;
  
  USART1_InitStructure.USART_BaudRate = 19200;
  USART1_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART1_InitStructure.USART_StopBits = USART_StopBits_1;
  USART1_InitStructure.USART_Parity = USART_Parity_No;
  USART1_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART1_InitStructure.USART_Mode = USART_Mode_Rx;
  
  USART_Init(USART2, &USART2_InitStructure);
  USART_Init(USART1, &USART1_InitStructure);
  
  USART_Cmd(USART2, ENABLE);
  USART_Cmd(USART1, ENABLE);
  volatile uint32_t uart2_status, uart1_status;
  
  GPIO_Write(GPIOC, 0x0000);
  volatile BitAction pin9, pin8;
  volatile FlagStatus OutgoingFlag, IncomingFlag;
  pin9 = Bit_SET;
  pin8 = Bit_SET;
  OutgoingFlag = RESET;
  IncomingFlag = RESET;
  volatile uint16_t TX_data, RX_data;
  
  while(1)
  {
    uart2_status = USART2->ISR;
    uart1_status = USART1->ISR;
    
    // When RX data received on USART2, transfer to GPIOB, upper octet 
    IncomingFlag = USART_GetFlagStatus(USART2, USART_FLAG_RXNE);
    if (IncomingFlag == SET)
    {
      RX_data = USART_ReceiveData(USART2);
      GPIO_Write(GPIOB, (RX_data << 8) | (0x00FF & (GPIOB->ODR)) );
      // Toggle GPIOC_9
      pin9 = GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_9);
    }
    
    // When TX data received on USART1, transfer to GPIOB, lower octet
    OutgoingFlag = USART_GetFlagStatus(USART1, USART_FLAG_RXNE);
    if (OutgoingFlag == SET)
    {
      TX_data = USART_ReceiveData(USART1);
      GPIO_Write(GPIOB, TX_data | (0xFF00 & (GPIOB->ODR)));
      // Toggle GPIOC_8
      pin8 = GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_8);
      
    }
    if (pin9 == Bit_SET)
    {
      GPIO_WriteBit(GPIOC, GPIO_Pin_9, Bit_RESET);
    }
    else if (pin9 == Bit_RESET)
    {
      GPIO_WriteBit(GPIOC, GPIO_Pin_9, Bit_SET);
    }
    if (pin8 == Bit_SET)
    {
      GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_RESET);
    }
    else if (pin8 == Bit_RESET)
    {
      GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_SET);
    }
  }
}
