#include "drivers/nixie_driver.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/* Ports configuration:
 * PC[0..3] - demultiplexor's address (drive cathodes, 0..9)
 * PB[0..7] - enable anodes (8 lamps)
 * PD0 - UART RxD
 * PD1 - UART TxD
 */

void USARTx_Init(void)
{
  UBRRL = (F_CPU/9600/16-1);
  UBRRH = (F_CPU/9600/16-1) >> 8;

  UCSRB |= (1 << RXCIE)|(1 << RXEN)|(1 << TXEN);
  UCSRC |= (1 << URSEL)|(1 << UCSZ1)|(1 << UCSZ0);
}

int main()
{
  // Prepare USART
  PORTC = 0x00;
  DDRC = 0x0F;  // [-- -- -- -- A3 A2 A1 A0]
  PORTB = 0x00;
  DDRB = 0xFF;  // [E7 E6 E5 E4 E3 E2 E1 E0]
  NIXIE_Init();

  // Prepare NIXIE
  DDRD  = 0xFE; // [-- -- -- -- -- -- TX RX]
  USARTx_Init();

  // Enable interrupts
  sei();

  // Update NIXIE display
  for (;;) {
    _delay_ms(1);
    NIXIE_NextTick();
  }

  return 0;
}

// Read new data from UART
ISR(USART_RXC_vect)
{
    uint8_t uartData = UDR;
    UDR = uartData; // echo

    uint8_t pos   = (uartData & 0xF0) >> 4;
    uint8_t digit = (uartData & 0x0F);

    NIXIE_WriteDigit(digit, pos);
}
