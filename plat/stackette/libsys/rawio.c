/* The console of the Stackette platform is the UART: DATA at 0xf000 (a write
 * sends a byte and stalls while the transmitter is busy, a read takes the
 * received byte), STATUS at 0xf002 (bit 1: a received byte is waiting). */

#include "libsys.h"

#define UART_DATA (*(volatile unsigned char*)0xf000)
#define UART_STATUS (*(volatile unsigned int*)0xf002)
#define RX_AVAIL 2

void _sys_rawwrite(unsigned char b)
{
	UART_DATA = b;
}

unsigned char _sys_rawread(void)
{
	while (!(UART_STATUS & RX_AVAIL))
		;
	return UART_DATA;
}
