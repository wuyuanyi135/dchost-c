#ifndef	__NRF24L01_CONFIG__
#define __NRF24L01_CONFIG__
#include "param.h"              /* volatile parameters, do not git */
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


/* COMMON.h*/
#define GETBIT(BYTE, BIT)  	((BYTE >> BIT) & 0x01)
#define SETBIT(BYTE,BIT,VAL)		((BYTE & ~(0x01<<BIT)) | (0x01<<VAL))
#define SETHIGH(BYTE,BIT)		(BYTE = (BYTE | (0x01<<BIT)))
#define SETLOW(BYTE,BIT)		(BYTE = (BYTE & ~(0x01<<BIT)))
#define __delayus(t)            //delayMicroseconds(t)
#endif
