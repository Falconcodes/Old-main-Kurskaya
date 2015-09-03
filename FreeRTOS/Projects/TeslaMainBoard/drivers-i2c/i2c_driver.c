/* Scheduler includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

/* Hardware specification. */
#include "stm32f0_discovery.h"
#include "stm32f0_tesla_board.h"

/* Demo application includes. */
#include "i2c_driver.h"

/*-----------------------------------------------------------*/

/* Misc defines. */
#define i2cINVALID_QUEUE        ( ( QueueHandle_t ) 0 )
#define i2cINVALID_SEMAPHORE    ( ( SemaphoreHandle_t ) 0 )
#define i2cNO_BLOCK             ( ( TickType_t ) 0 )
#define i2cTX_BLOCK_TIME        ( 40 / portTICK_PERIOD_MS )

/*-----------------------------------------------------------*/

/* The queue used to hold received characters. */
static QueueHandle_t xRxedChars;
static QueueHandle_t xCharsForTx;

/* I2C semaphores */
SemaphoreHandle_t xSemaphore_I2C_READY = NULL;
SemaphoreHandle_t xSemaphore_FLAG_TC = NULL;
SemaphoreHandle_t xSemaphore_FLAG_RXNE = NULL;

/*-----------------------------------------------------------*/

void vI2CPortInitMinimal()
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  NVIC_InitTypeDef  NVIC_InitStructure;
  I2C_InitTypeDef   I2C_InitStructure;

  /* Create the queues used to hold Rx/Tx characters. */
  xRxedChars = xQueueCreate( I2Cx_RX_LEN, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
  xCharsForTx = xQueueCreate( I2Cx_TX_LEN, ( unsigned portBASE_TYPE ) sizeof( signed char ) );

  /* Create the semaphores used to detect I2C transfer state. */
  xSemaphore_I2C_READY = xSemaphoreCreateBinary();
  xSemaphore_FLAG_TC = xSemaphoreCreateBinary();
  xSemaphore_FLAG_RXNE = xSemaphoreCreateBinary();

  /* If the queue/semaphore was not created correctly then return. */
  if( ( xRxedChars == i2cINVALID_QUEUE ) ||
      ( xCharsForTx == i2cINVALID_QUEUE ) ||
      ( xSemaphore_I2C_READY == i2cINVALID_SEMAPHORE ) ||
      ( xSemaphore_FLAG_TC == i2cINVALID_SEMAPHORE ) ||
      ( xSemaphore_FLAG_RXNE == i2cINVALID_SEMAPHORE ) )
    return;

  /* I2C Periph clock enable */
  RCC_APB1PeriphClockCmd(I2Cx_CLK, ENABLE);

  /* SDA GPIO clock enable */
  RCC_AHBPeriphClockCmd(I2Cx_SDA_GPIO_CLK, ENABLE);

  /* SCL GPIO clock enable */
  RCC_AHBPeriphClockCmd(I2Cx_SCL_GPIO_CLK, ENABLE);

  /* Connect PXx to I2C_SCL */
  GPIO_PinAFConfig(I2Cx_SCL_GPIO_PORT, I2Cx_SCL_SOURCE, I2Cx_SCL_AF);

  /* Connect PXx to I2C_SDA */
  GPIO_PinAFConfig(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_SOURCE, I2Cx_SDA_AF);

  /* Configure I2C SCL pin */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin   = I2Cx_SCL_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(I2Cx_SCL_GPIO_PORT, &GPIO_InitStructure);

  /* Configure I2C SDA pin */
  GPIO_InitStructure.GPIO_Pin   = I2Cx_SDA_PIN;
  GPIO_Init(I2Cx_SDA_GPIO_PORT, &GPIO_InitStructure);

  /* Configure the I2C event priority */
  NVIC_InitStructure.NVIC_IRQChannel         = I2Cx_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd      = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Configure I2C error interrupt to have the higher priority */
  NVIC_InitStructure.NVIC_IRQChannel         = I2Cx_IRQn;
  NVIC_Init(&NVIC_InitStructure);

  /* I2C De-initialize */
  I2C_DeInit(I2Cx);

  /* I2C Struct Initialize */
  I2C_StructInit(&I2C_InitStructure);
  I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C;
  I2C_InitStructure.I2C_OwnAddress1         = I2Cx_OWN_ADDRESS;
  I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;
  I2C_InitStructure.I2C_Timing              = I2Cx_TIMING;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

  /* I2C Initialize */
  I2C_Init(I2Cx, &I2C_InitStructure);

  /* Enable Error Interrupt */
  I2C_ITConfig(I2Cx, I2C_IT_ERRI, ENABLE);

  /* Enable transfer interrupt */
  I2C_ITConfig(I2Cx,
               I2C_IT_STOPI | I2C_IT_TXI | I2C_IT_RXI,
               ENABLE);

  /* I2C ENABLE */
  I2C_Cmd(I2Cx, ENABLE);

  /* Ensure that I2C is not busy and give semaphore */
  if (I2C_GetFlagStatus(I2Cx, I2C_ISR_BUSY) == RESET)
    xSemaphoreGive( xSemaphore_I2C_READY );
}

signed portBASE_TYPE vI2CWriteRegister(uint8_t reg, uint8_t val)
{
  signed portBASE_TYPE xReturn = pdPASS;

  /* Wait for I2C ready */
  if( xSemaphoreTake( xSemaphore_I2C_READY, portMAX_DELAY ) == pdTRUE ) {

    /* Send register and value to queue (send both at one transaction) */
    if( xQueueSend( xCharsForTx, &reg, i2cTX_BLOCK_TIME ) != pdPASS )
      xReturn = pdFAIL;
    if( xReturn != pdPASS || xQueueSend( xCharsForTx, &val, i2cTX_BLOCK_TIME ) != pdPASS )
      xReturn = pdFAIL;

    /* Enable "Transfer Complete" interrupt */
    I2C_ITConfig(I2Cx, I2C_IT_TCI, ENABLE);

    /* Queue filled, ready to start transaction */
    I2C_TransferHandling(I2Cx,
                         I2Cx_SLAVE_ADDRESS,
                         2 /* = bytes to send */,
                         I2C_SoftEnd_Mode,
                         I2C_Generate_Start_Write);

    /* Wait for transmit complete */
    if( xSemaphoreTake( xSemaphore_FLAG_TC, portMAX_DELAY ) == pdTRUE )
      I2C_GenerateSTOP(I2Cx, ENABLE);

    /* DON'T give xSemaphore_I2C_READY here. It should be done in ISR. */
  }

  return xReturn;
}

signed portBASE_TYPE vI2CReadRegister(uint8_t reg, uint8_t *val)
{
  signed portBASE_TYPE xReturn = pdPASS;

  /* Wait for I2C ready */
  if( xSemaphoreTake( xSemaphore_I2C_READY, portMAX_DELAY ) == pdTRUE ) {

    /* Send register to queue */
    if( xQueueSend( xCharsForTx, &reg, i2cTX_BLOCK_TIME ) != pdPASS )
      xReturn = pdFAIL;

    /* Enable "Transfer Complete" interrupt */
    I2C_ITConfig(I2Cx, I2C_IT_TCI, ENABLE);

    /* Queue filled, ready to start transaction */
    I2C_TransferHandling(I2Cx,
                         I2Cx_SLAVE_ADDRESS,
                         1 /* = bytes to send */,
                         I2C_SoftEnd_Mode,
                         I2C_Generate_Start_Write);

    /* Wait for transmit complete */
    if( xSemaphoreTake( xSemaphore_FLAG_TC, portMAX_DELAY ) != pdTRUE )
      xReturn = pdFAIL;

    /* Read register value */
    I2C_TransferHandling(I2Cx,
                         I2Cx_SLAVE_ADDRESS,
                         1 /* = bytes to receive */,
                         I2C_AutoEnd_Mode,
                         I2C_Generate_Start_Read);

    /* Get character from the buffer */
    xQueueReceive( xRxedChars, val, portMAX_DELAY );

    /* DON'T give xSemaphore_I2C_READY here. It should be done in ISR. */
  }

  return xReturn;
}

void I2Cx_IRQHANDLER()
{
  static signed portBASE_TYPE xHigherPriorityTaskWoken;

  if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_TXIS) == SET) {
    char cTxChar;
    if ( xQueueReceiveFromISR( xCharsForTx, ( void * ) &cTxChar, i2cNO_BLOCK) )
      I2C_SendData(I2Cx, cTxChar);
  }

  if (I2C_GetFlagStatus(I2Cx, I2C_ISR_RXNE) == SET) {
    char cRxChar;
    cRxChar = I2C_ReceiveData(I2Cx);
    xQueueSendToBackFromISR( xRxedChars, ( void * ) &cRxChar, i2cNO_BLOCK);
  }

  if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_TC) == SET) {
    xSemaphoreGiveFromISR( xSemaphore_FLAG_TC, &xHigherPriorityTaskWoken );
    I2C_ITConfig(I2Cx, I2C_IT_TCI, DISABLE);
  }

  if (I2C_GetFlagStatus(I2Cx, I2C_FLAG_STOPF) == SET) {
    xSemaphoreGiveFromISR( xSemaphore_I2C_READY, &xHigherPriorityTaskWoken );
    I2C_ClearFlag(I2Cx, I2C_FLAG_STOPF);
  }

  portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}