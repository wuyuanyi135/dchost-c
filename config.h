#ifndef	__NRF24L01_CONFIG__
#define __NRF24L01_CONFIG__
#ifndef	 FLAG_EMBED
	#include <wiringPi.h>
	#include <wiringPiSPI.h>
	
	#define SPIXFER(BUF,LEN) wiringPiSPIDataRW(CHANNEL,BUF,LEN)
	#define CE(HIGHLOW)    digitalWrite (CE_PIN, HIGHLOW)
	
	#define  ENABLE   (1)
	#define  DISABLE  (0)
	
#else
	#include "spi.h"
	#include "stm32f0xx.h"
	
	#define SPIXFER(BUF,LEN)	SPI_xfer(BUF,LEN)
	#define CE(HIGHLOW)		 GPIO_WriteBit(GPIOA, GPIO_Pin_0,HIGHLOW==HIGH?Bit_SET:Bit_RESET);
	
	#define HIGH		(1)
	#define LOW			(0)
#endif

#define CHANNEL   (0)
#define IRQ_PIN   (24)
#define CE_PIN    (25)
#define SPEED     (1000000)

#ifndef FLAG_EMBED

#define RX_ADDR_P0   (TX_ADDR)      /* used for auto ack */
#define RX_ADDR_P1   (0xE0E1E2E3E4)
#define TX_ADDR      (0xA0A1A2A3A4)

#else

#define RX_ADDR_P0   (TX_ADDR)      /* used for auto ack */
#define RX_ADDR_P1   (0xA0A1A2A3A4)
#define TX_ADDR      (0xE0E1E2E3E4)

#endif 

//DWIRE CONFIG
#define DW_TX_SIZE		(64)
#define DW_RX_SIZE		(64)
#define	DW_LOCAL_ADDR		(0x10)					/* only used in slave mode */

#define DW_VAR_MAP_SIZE	(4)						/* auto var get/set handler map size */


/* COMMON.h*/
#define GETBIT(BYTE, BIT)  	((BYTE >> BIT) & 0x01)
#define SETBIT(BYTE,BIT,VAL)		((BYTE & ~(0x01<<BIT)) | (0x01<<VAL))
#define SETHIGH(BYTE,BIT)		(BYTE | (0x01<<BIT))
#define SETLOW(BYTE,BIT)		(BYTE & ~(0x01<<BIT))
#endif
