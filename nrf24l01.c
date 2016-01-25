#include "nrf24l01.h"
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <string.h>

#include "config.h"

#define SPIXFER(BUF,LEN) wiringPiSPIDataRW(CHANNEL,BUF,LEN)
#define CE(HIGHLOW)    digitalWrite (CE_PIN, HIGHLOW)
void nrf24l01_set_dynamic_payload_length (uint8_t channel, uint8_t state)
{
   _nrf24l01_mod_reg (REG_FEATURE, REG_FEATURE_EN_DPL, ENABLE);
   _nrf24l01_mod_reg (REG_DYNPD, channel, state);
   _nrf24l01_mod_reg (REG_EN_AA, channel, state);
   

   /* activate */   
}

uint64_t nrf24l01_get_rxaddr(uint8_t channel)
{
   if (channel > 5)
   {
      return (uint64_t)(-1);
   }
   
   uint64_t tmp={0xffffffffff00};

   if (channel == 0)
   {
      tmp |= R_REGISTER | REG_RX_ADDR_P0;
      SPIXFER ((uint8_t *)&tmp,6);
      tmp = tmp >> 8;
      return tmp;
   }
   
   tmp = R_REGISTER | REG_RX_ADDR_P1;
   SPIXFER ((uint8_t *)&tmp,6);

   if (channel == 1)
   {
      return (tmp >> 8);
   }

   tmp = R_REGISTER | (REG_RX_ADDR_P0 + channel);
   SPIXFER ((uint8_t *)&tmp, 2);
   return (tmp >> 8);
}

void nrf24l01_set_rxaddr(uint8_t channel, uint64_t address)
{
   
   if (channel > 5)
   {
      return; 
   }
   
   if (channel < 2)
   {
      address = W_REGISTER | (REG_RX_ADDR_P0 + channel) | (address << 8);

      SPIXFER ((uint8_t *)&address,6);
   }
   

   address = R_REGISTER | (REG_RX_ADDR_P0 + channel) | (address << 8);
   SPIXFER ((uint8_t *)&address,2);
}

uint64_t nrf24l01_get_txaddr(void)
{
   uint64_t tmp = {0xffffffffff00};
   tmp =  R_REGISTER | REG_TX_ADDR;
   SPIXFER((uint8_t *)&tmp,6);
   return (tmp >> 8);
}

void nrf24l01_set_txaddr(uint64_t address)
{
   address = W_REGISTER | REG_TX_ADDR | (address << 8);
   SPIXFER((uint8_t *)&address,6);
}

uint8_t nrf24l01_read_clear_interrupt (void)
{
   uint8_t status = nrf24l01_read_status();
   nrf24l01_clear_interrupt();
   return status;
}
uint8_t nrf24l01_read_status (void)
{
   return _nrf24l01_get_reg(REG_STATUS);
}

void nrf24l01_power_up (uint8_t state)
{
   _nrf24l01_mod_reg( REG_CONFIG, REG_CONFIG_PWR_UP, state);
}

void nrf24l01_mode (uint8_t mode)
{
   if (mode == MODE_STANDBY)
   {
      CE(LOW);
      return;
   }
   
   CE(LOW);
   _nrf24l01_mod_reg( REG_CONFIG, REG_CONFIG_PRIM_RX, mode);
   CE(HIGH);
}

void nrf24l01_clear_interrupt (void)
{
   _nrf24l01_set_reg (REG_STATUS, 0x70);
}

uint8_t _nrf24l01_get_reg (uint8_t reg)
{
   uint8_t buffer [2];
   buffer[0] = reg;
   buffer[1] = 0xff;
   SPIXFER(buffer,2);

   return buffer[1];
}

void _nrf24l01_set_reg(uint8_t reg, uint8_t val)
{
   uint8_t buffer [2];
   buffer[0] = W_REGISTER | reg;
   buffer[1] = val;
   SPIXFER(buffer,2);
}

uint8_t _nrf24l01_mod_reg(uint8_t reg, uint8_t bit, uint8_t value)
{
   uint8_t old;
   old = _nrf24l01_get_reg (reg);

   if (((old >> bit) & 0x01) == value )
      return old;
   
   uint8_t new;
   new = (old & ~(1 << bit)) | (value << bit); 
   _nrf24l01_set_reg(reg, new);
   return old;
}
void _nrf24l01_write_tx (uint8_t* buffer, uint8_t size)
{
   uint8_t buf [33]; /* TODO: replace hard code */
   buf[0] = W_TX_PAYLOAD;
   memcpy (buf+1, buffer,size);
   SPIXFER(buf,size+1);   
}

void _nrf24l01_read_rx(uint8_t* buffer, uint8_t size)
{
   uint8_t buf [33] = {0xff}; /* TODO: replace hard code */
   buf[0] = R_RX_PAYLOAD;
   memcpy (buf+1, buffer,size);
   SPIXFER(buf,size+1);   
   memcpy (buffer,buf+1,size);
} 

void _nrf24l01_flush_tx (void)
{
   uint8_t buf;
   buf = FLUSH_TX;
   SPIXFER(&buf,1);
}

void _nrf24l01_flush_rx (void)
{
   uint8_t buf;
   buf = FLUSH_RX;
   SPIXFER(&buf,1);
}

