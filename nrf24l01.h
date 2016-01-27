#ifndef  __NRF24L01_H__
#define  __NRF24L01_H__

#include <stdint.h>
#include "config.h"

#define R_REGISTER   (0x00)
#define W_REGISTER   (0x20)
#define R_RX_PAYLOAD (0x61)
#define W_TX_PAYLOAD (0xA0)
#define FLUSH_TX  (0xE1)
#define FLUSH_RX  (0xE2)

#define REG_CONFIG      (0x00)
#define REG_CONFIG_MASK_RX_DR    (6)
#define REG_CONFIG_MASK_TX_DS    (5)
#define REG_CONFIG_MAX_RT        (4)
#define REG_CONFIG_EN_CRC        (3)
#define REG_CONFIG_CRCO          (2)
#define REG_CONFIG_PWR_UP        (1)
#define REG_CONFIG_PRIM_RX       (0)

#define REG_EN_AA       (0x01)
#define REG_EN_RXADDR   (0x02)
#define REG_SETUP_RETR	(0x04)
#define REG_STATUS      (0x07)
#define REG_STATUS_RX_DR   (6)
#define REG_STATUS_TX_DS   (5)
#define REG_STATUS_MAX_RT  (4)
//#define REG_STATUS_PX_P_NO (3)
#define REG_STATUS_TX_FULL (0)

#define REG_RX_ADDR_P0  (0x0A)
#define REG_RX_ADDR_P1  (0x0B) 
#define REG_TX_ADDR     (0x10)
#define REG_RX_PW_P0    (0x11)
#define REG_DYNPD       (0x1C)
#define REG_FEATURE     (0x1D)
#define REG_FEATURE_EN_DPL       (2)
#define REG_FEATURE_ACK_PAY      (1)
#define REG_FEATURE_EN_DYN_ACK   (0)


#define MODE_RX   (1)
#define MODE_TX   (0)
#define MODE_STANDBY (2)

 

extern const uint8_t nrf24l01_initial_regs[]; 
extern const uint8_t nrf24l01_initial_state[];

void nrf24l01_printall(void);

void nrf24l01_reset(void);
void nrf24l01_set_enable_rx (uint8_t channel, uint8_t state);

void nrf24l01_set_retry_count (uint8_t count);
void nrf24l01_set_retry_delay	(uint8_t	ARD);

void nrf24l01_set_en_aa (uint8_t channel, uint8_t state);
/* TODO getter*/
void nrf24l01_set_rx_payload_length ( uint8_t channel, uint8_t length);

/* TODO close DPL */
void nrf24l01_set_dynamic_payload_length ( uint8_t channel, uint8_t state);

uint8_t nrf24l01_get_config (void);
void nrf24l01_set_config (uint8_t config);

uint64_t nrf24l01_get_rxaddr(uint8_t channel);
/* first 40 bits in address is used for hannel 0 and 1. For the rest only 8 bits is selected */
void nrf24l01_set_rxaddr(uint8_t channel, uint64_t address);

uint64_t nrf24l01_get_txaddr(void);
void nrf24l01_set_txaddr(uint64_t address);

uint8_t nrf24l01_read_clear_interrupt (void);
uint8_t nrf24l01_read_status(void);
void nrf24l01_clear_interrupt(void);

void nrf24l01_power_up (uint8_t state);
void nrf24l01_mode (uint8_t mode);
uint8_t _nrf24l01_get_reg(uint8_t reg);
void    _nrf24l01_set_reg(uint8_t reg, uint8_t val);
uint8_t _nrf24l01_mod_reg(uint8_t reg, uint8_t bit, uint8_t value);

void _nrf24l01_write_tx (uint8_t* buffer, uint8_t size);
void _nrf24l01_read_rx (uint8_t* buffer, uint8_t size);

void _nrf24l01_flush_tx (void);
void _nrf24l01_flush_rx (void);



#endif
