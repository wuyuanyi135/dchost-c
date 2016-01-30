#include "nrf24l01.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "config.h"

const uint8_t nrf24l01_initial_regs[] = 	{0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x1c, 0x1d};
const uint8_t nrf24l01_initial_state[] = 	{0x08, 0x3f, 0x03, 0x03, 0x03, 0x02, 0x0f, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void nrf24l01_set_autoack(uint8_t channel, uint8_t* payload, uint8_t size)
{
	uint8_t buf[33];
	buf[0] = W_ACK_PAYLOAD | channel;
	memcpy (buf+1,payload,size);
	SPIXFER(buf,size+1);
}

void nrf24l01_reuse_tx (void)
{
	uint8_t buf;
	buf = REUSE_TX_PL;
	SPIXFER((unsigned char*)&buf,1);
}

void nrf24l01_set_feature (uint8_t feature, uint8_t state)
{
		_nrf24l01_mod_reg (REG_FEATURE, feature,state);
}

void nrf24l01_rfchannel(uint8_t channel)
{
    _nrf24l01_set_reg(REG_RF_CH, channel);
}

void nrf24l01_printall(void)
{
	#ifndef FLAG_EMBED
   uint8_t regs[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x11, 0x12, 0x13, 0x14,0x15,0x16,0x17,0x1c,0x1d};
   printf ("=========================\n");
   for (int i = 0; i < sizeof(regs); i ++ )
   {
      printf ("Reg %x: %d\n",regs[i], _nrf24l01_get_reg(regs[i]));
   }
   printf ("TX ADDR: %llx\n", (long long) nrf24l01_get_txaddr());
   
   for (int i = 0; i <=5; i ++ )
   {
      printf ("RX ADDR%d: %llx\n",i,(long long) nrf24l01_get_rxaddr(i));
   }
   printf ("-------------------------\n");
	 #endif
}

void nrf24l01_reset(void)
{
	_nrf24l01_flush_rx();
	_nrf24l01_flush_tx();
	
	for (uint32_t i = 0; i<sizeof(nrf24l01_initial_regs); i++)
		_nrf24l01_set_reg ( nrf24l01_initial_regs[i], nrf24l01_initial_state[i]);
}

void nrf24l01_set_enable_rx (uint8_t channel, uint8_t state)
{
	_nrf24l01_mod_reg(REG_EN_RXADDR, channel, state);
}

void nrf24l01_set_retry_count (uint8_t count)
{
	uint8_t old = _nrf24l01_get_reg(REG_SETUP_RETR);
	old = (old & ~(0x0f) ) | count;
	_nrf24l01_set_reg(REG_SETUP_RETR, old);
}
void nrf24l01_set_retry_delay	(uint8_t	ARD)
{
	uint8_t old = _nrf24l01_get_reg(REG_SETUP_RETR);
	old = (old & ~(0xf0) ) | ARD;
	_nrf24l01_set_reg(REG_SETUP_RETR, old);	
}

void nrf24l01_set_en_aa (uint8_t channel, uint8_t state)
{
   _nrf24l01_mod_reg (REG_EN_AA, channel, state);
}

void nrf24l01_set_rx_payload_length( uint8_t channel, uint8_t length)
{
   _nrf24l01_set_reg (REG_RX_PW_P0 + channel, length);
}
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

   if (mode == MODE_TX)
   {
      __delayus(10);
      CE(LOW);
   }
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

uint8_t _nrf24l01_read_rx_payload_width (void)
{
    uint8_t buf[2];
    buf[0] = R_RX_PL_WID;
    buf[1] = 0xff;
    SPIXFER((unsigned char*)&buf,2);
    return buf[1];    
}
