#ifndef __DWIRE_H__
#define __DWIRE_H__

#include "config.h"
#include "queue.h"

#include <stdint.h>

#define RPC_CMD_ALIVE	(0x0f)			/* test if this device is alive */
#define RPC_GET_VAR		(0x0a)
#define RPC_SET_VAR		(0x0b)

#define RPC_ACK				(0xff)
#define RPC_NACK			(0x00)

#define RPC_EXCEPTION_NOT_FOUND		(0x44);

#define DW_TX_CLEAR		(0x01)
#define DW_TX_WAIT		(0x00)

#define DW_FLAGS_TX_READY		(0)
#define DW_FLAGS_RX_READY		(1)

#define DW_RPC_FAILED				(0)
#define DW_RPC_OK						(1)
#define __DWEVT __attribute__((weak)) 



extern volatile uint8_t dwire_tx_buffer[];
extern volatile uint8_t dwire_rx_buffer[];
extern volatile uint32_t dwire_machine_flags;
extern volatile uint32_t dwire_tx_fail_counter;
extern volatile uint32_t dwire_tx_fail_max;
extern void (*dwire_rpc_call_callback)(uint8_t*, uint32_t);
extern QueueParameter dwire_tx_queue, dwire_rx_queue;
// return ret_buf len, return negative number to enable auto handle
__DWEVT int32_t dwire_rpc_handler(uint8_t rpc_code, uint8_t* rpc_args, uint32_t rpc_args_len, uint8_t* ret_buf);
int32_t _dwire_rpc_handler(uint8_t rpc_code, uint8_t* rpc_args, uint32_t rpc_args_len, uint8_t* ret_buf);

void dwire_init (void);
void dwire_rpc_machine(void);

//remote call on the same physical channel
int32_t dwire_rpc_call(uint8_t remote_address, uint8_t rpc_code, uint8_t * rpc_args,  uint32_t rpc_args_len, void (*callback)(uint8_t*, uint32_t));

void _dwire_txds_handler(void);
void _dwire_maxrt_handler(void);
void _dwire_rxdr_handler(void);

void _dwire_notify_send(void);				/* tell dwire tx packet is ready */

/* export nrf24l01p code */
void _dwire_send(uint8_t* buffer, uint8_t len);
void _dwire_abort_tx (void);
void _dwire_abort_rx (void);
uint8_t _dwire_get_dynamic_length(void);
void _dwire_read_rx (uint8_t* buffer, uint8_t length);

// double score is standard handler (clean packet, no need for post modification)
int32_t __dw_rpc_get_var(uint32_t reg_code, uint8_t *ret_buf);
#endif
