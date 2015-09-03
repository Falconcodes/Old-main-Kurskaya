#include "usart.h"

DMA_InitTypeDef DMA_InitStructure;

/* ========== Initialize ========== */

void USARTx_Init(void)
{
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(USARTx_TX_GPIO_CLK | USARTx_RX_GPIO_CLK, ENABLE);

  /* Enable USART clock */
  USARTx_APBPERIPHCLOCK(USARTx_CLK, ENABLE);

  /* Enable the DMA periph */
  RCC_AHBPeriphClockCmd(DMAx_CLK, ENABLE);

  /* Connect PXx to USARTx_Tx */
  GPIO_PinAFConfig(USARTx_TX_GPIO_PORT, USARTx_TX_SOURCE, USARTx_TX_AF);

  /* Connect PXx to USARTx_Rx */
  GPIO_PinAFConfig(USARTx_RX_GPIO_PORT, USARTx_RX_SOURCE, USARTx_RX_AF);

  /* Configure USART Tx and Rx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

  GPIO_InitStructure.GPIO_Pin = USARTx_TX_PIN;
  GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = USARTx_RX_PIN;
  GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStructure);

  /* DMA Configuration: write to USART  */
  DMA_StructInit(&DMA_InitStructure);
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
  DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
  DMA_Init(USARTx_TX_DMA_CHANNEL, &DMA_InitStructure);

  /* Configure USART parameters */
  USART_InitStructure.USART_BaudRate            = USARTx_BAUD;
  USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits            = USART_StopBits_1;
  USART_InitStructure.USART_Parity              = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USARTx, &USART_InitStructure);

  /* Enable the USART */
  USART_Cmd(USARTx, ENABLE);

  /* Init the xprintf library */
  xdev_out(USARTx_Write_Byte);
}

/* ========== Transmit data ========== */

void USARTx_Write_Byte(uint8_t data)
{
  while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
  USART_SendData(USARTx, data);
}

void USARTx_Prepare_Transmitter(uint8_t *txBuffer, uint16_t size)
{
  /* DMA Configuration: write to USART  */
  DMA_DeInit(USARTx_TX_DMA_CHANNEL);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USARTx_TDR_ADDRESS;
  DMA_InitStructure.DMA_BufferSize         = (uint16_t)size;
  DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)txBuffer;
  DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
  DMA_Init(USARTx_TX_DMA_CHANNEL, &DMA_InitStructure);

  /* Enable DMA1 USART Tx Channel */
  DMA_Cmd(USARTx_TX_DMA_CHANNEL, ENABLE);

  /* Clear the TC bit in the SR register by writing 0 to it */
  USART_ClearFlag(USARTx, USART_FLAG_TC);

  /* Enable DMA for USART (Start DMA transmission) */
  USART_DMACmd(USARTx, USART_DMAReq_Tx, ENABLE);

  /* ***** SENDING: check state in USARTx_Is_Transmit_Complete() ***** */
}

void USARTx_Prepare_Receiver(uint8_t *rxBuffer, uint16_t size)
{
  /* DMA Configuration: read from USART  */
  DMA_DeInit(USARTx_RX_DMA_CHANNEL);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USARTx_RDR_ADDRESS;
  DMA_InitStructure.DMA_BufferSize         = (uint16_t)size;
  DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)rxBuffer;
  DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_Priority           = DMA_Priority_Low;
  DMA_Init(USARTx_RX_DMA_CHANNEL, &DMA_InitStructure);

  /* Enable the DMA channel */
  DMA_Cmd(USARTx_RX_DMA_CHANNEL, ENABLE);

  /* Clear the TC bit in the SR register by writing 0 to it */
  USART_ClearFlag(USARTx, USART_FLAG_TC);

  /* Enable the USART Rx DMA request */
  USART_DMACmd(USARTx, USART_DMAReq_Rx, ENABLE);

  /* ***** RECEIVING: check state in USARTx_Is_Receive_Complete() ***** */
}

int USARTx_Is_Transmit_Complete()
{
  /* Wait the USART DMA Tx transfer complete */
  if (DMA_GetFlagStatus(USARTx_TX_DMA_FLAG_TC) == RESET)
    return 0;

  /* The software must wait until TC=1. The TC flag remains cleared during all data
     transfers and it is set by hardware at the last frame's end of transmission */
  while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);

  /* Clear DMA global flags */
  DMA_ClearFlag(USARTx_TX_DMA_FLAG_GL);

  /* Disable DMA1 USART Tx Channel */
  DMA_Cmd(USARTx_TX_DMA_CHANNEL, DISABLE);

  /* Disable the USART Tx DMA requests (Stop DMA transmission) */
  USART_DMACmd(USARTx, USART_DMAReq_Tx, DISABLE);

  return 1;
}

int USARTx_Is_Receive_Complete()
{
  /* Wait the USART DMA Rx transfer complete */
  if (DMA_GetFlagStatus(USARTx_RX_DMA_FLAG_TC) == RESET)
    return 0;

  /* Clear DMA1 global flags */
  DMA_ClearFlag(USARTx_RX_DMA_FLAG_GL);

  /* Disable the DMA channels */
  DMA_Cmd(USARTx_RX_DMA_CHANNEL, DISABLE);

  /* Disable the USART Rx DMA requests */
  USART_DMACmd(USARTx, USART_DMAReq_Rx, DISABLE);

  return 1;
}



void USARTx_Write_Text(const uint8_t *str)
{
  while(*str) {
    USARTx_Write_Byte(*str++);
  }
}

void USARTx_Write_Int(uint32_t data)
{
  USARTx_Write_Int_FixedWidth(data, 1);
}

void USARTx_Write_Int_FixedWidth(uint32_t data, uint8_t min_width)
{
  /* 2^32 = 4'294'967'296 (i.e. 10 digits max) */
#define N_DIGITS        10
#define MAX_DIVIDER     1000000000

  uint8_t foundNotNull = 0;
  uint8_t currentIteration = 0;
  uint32_t currentData = data;
  uint32_t currentDivider = MAX_DIVIDER;
  uint32_t currentDigit;

  /* Fix minimum width */
  while (min_width > N_DIGITS) {
    USARTx_Write_Byte('0');
    --min_width;
  }

  /* Fill digits */
  while (currentDivider > 0) {
    /* Get current digit */
    currentDigit = currentData / currentDivider;
    currentData -= currentDigit * currentDivider;
    currentDivider /= 10; // 10 is base of dec. system
    ++currentIteration;

    if (foundNotNull == 0 && currentDigit != 0)
      foundNotNull = 1;

    /* Transmit digits without leading zero */
    if (foundNotNull || currentIteration > N_DIGITS - min_width)
      USARTx_Write_Byte((currentDigit & 0x0F) + '0');
  }
}

/* ========== Receive data ========== */

uint8_t USARTx_Read_Byte(void)
{
  while(USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET);
  return (USART_ReceiveData(USARTx) & 0xFF);
}
