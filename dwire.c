#include <stdio.h>
#include "dwire.h"
#include "queue.h"
#include "config.h"
#include "stdlib.h"
#include <stdint.h>
#include <string.h>
#include "nrf24l01.h"
volatile uint8_t dwire_tx_buffer[DW_TX_SIZE];
volatile uint8_t dwire_rx_buffer[DW_RX_SIZE];
volatile uint32_t dwire_machine_flags;
volatile uint32_t dwire_tx_fail_counter;
volatile uint32_t dwire_tx_fail_max = 0;
volatile uint32_t dwire_late_send_time = 0;
volatile uint32_t dwire_notify_send_time = 0;

extern uint32_t msTick;
void (*dwire_rpc_call_callback)(uint8_t*, uint32_t);			/* TODO: check if NULL */

QueueParameter dwire_tx_queue, dwire_rx_queue;

__DWEVT int32_t dwire_rpc_handler(uint8_t from, uint8_t to, uint8_t rpc_code, uint8_t* rpc_args, uint32_t rpc_args_len, uint8_t* ret_buf)
{
	return -1;		/* let auto handler to do everything */
	
}

// real handler called internally
int32_t _dwire_rpc_handler(uint8_t from, uint8_t to, uint8_t rpc_code, uint8_t* rpc_args, uint32_t rpc_args_len, uint8_t* ret_buf)
{
//	uint8_t packet;
	int32_t len = dwire_rpc_handler(from, to, rpc_code, rpc_args, rpc_args_len, ret_buf);		/* go for external callback */
	
	if (len >= 0)				/* if negative, need to use default handler */
		return len;				
	
	switch (rpc_code)
	{
		case RPC_CMD_REPLY:				/* replying to previous transmitted packet, no more response to this packet. */
			
			/* finally, the call back is useful */
			dwire_rpc_call_callback(rpc_args,rpc_args_len);
			return	(-1);
		
		case RPC_CMD_ALIVE:				/* alive handler */
			ret_buf[0] = DW_LOCAL_ADDR;
			ret_buf[1] = from;
			ret_buf[2] = RPC_CMD_REPLY;
			ret_buf[3] = RPC_ACK;
			return 4;
		
		case RPC_GET_VAR:
			len = __dw_rpc_get_var(*(int32_t*)rpc_args,ret_buf);
			return len;
			
		default:
			ret_buf[0] = DW_LOCAL_ADDR;
			ret_buf[1] = from;
			ret_buf[2] = RPC_CMD_REPLY;
			ret_buf[3] = RPC_NACK;
			ret_buf[4] = RPC_EXCEPTION_NOT_FOUND;
			return 5;	
	}
}

void dwire_init (void)
{
	QueueInit(&dwire_tx_queue, (void*)dwire_tx_buffer, DW_TX_SIZE, sizeof(uint8_t));
	QueueInit(&dwire_rx_queue, (void*)dwire_rx_buffer, DW_RX_SIZE, sizeof(uint8_t));
	dwire_machine_flags = 0x00;
	dwire_tx_fail_counter = 0;
}

void dwire_rpc_machine(void)
{

	if (GETBIT(dwire_machine_flags, DW_FLAGS_RX_READY))
	{
		/* received packet */
		/*uint32_t packet_length = dwire_rx_queue.Count;*/
		uint8_t packet_from;
		
		Dequeue(&dwire_rx_queue, &packet_from, 1);
		
		uint8_t packet_to;
		Dequeue(&dwire_rx_queue, &packet_to,1);
		
		uint8_t _rpc;
		uint32_t args_length = dwire_rx_queue.Count;
		uint8_t* packet_args = malloc (args_length);
		
		if (!packet_args)
		{
			/* send error message (heap overflow) */
			while (1);
		}
		
		Dequeue(&dwire_rx_queue, &_rpc, args_length);
		int32_t send_length = _dwire_rpc_handler(packet_from, packet_to, _rpc, packet_args, args_length, (uint8_t*)dwire_tx_buffer);	/* dangerous operation */
		
		free (packet_args);
		if (send_length > DW_TX_SIZE)
		{
			/* send error message (tx buffer overflow ) */
			while(1);
		}
		
		if (send_length < 0 )
		{
			/* Do not send response */
			SETLOW(dwire_machine_flags, DW_FLAGS_RX_READY);
			QueueVacuate(&dwire_tx_queue);
		}
		else
		{
			dwire_tx_queue.Count = send_length;
			dwire_tx_queue.Offset = 0;
			
			
			SETHIGH(dwire_machine_flags, DW_FLAGS_TX_READY);
			SETLOW(dwire_machine_flags, DW_FLAGS_RX_READY);
			#ifdef FLAG_EMBED
				extern uint32_t msTick;
				dwire_late_send_time = msTick + 1;
				SETHIGH(dwire_machine_flags, DW_FLAGS_LATE_SEND);
			#else
				delay(1);				/* delay 1 ms	*/
				_dwire_notify_send();
			#endif
			
		}
	}
	
}
#ifdef FLAG_EMBED
void dwire_rpc_timed_machine(void)
{
	if (GETBIT(dwire_machine_flags, DW_FLAGS_LATE_SEND) && dwire_late_send_time < msTick && msTick - dwire_late_send_time < 100000)
	{
		_dwire_notify_send();
		SETLOW(dwire_machine_flags, DW_FLAGS_LATE_SEND);
	}
}
#endif
int32_t dwire_rpc_call(uint8_t remote_address, uint8_t rpc_code, uint8_t * rpc_args,  uint32_t rpc_args_len, void (*callback)(uint8_t*, uint32_t))
{
	/*if ((dwire_machine_flags >> DW_FLAGS_TXDS) & 0x01)
		return DW_RPC_FAILED;
	*/
	if (dwire_tx_queue.Count > 0)	/* tx not finished */
		return DW_RPC_FAILED;
	
	if (remote_address & 0x01)
		return DW_RPC_FAILED;				/* first bit should not be one */
	
	/* assert succeed, push packet*/
	
	uint8_t self_addr = DW_LOCAL_ADDR;
	
	while (1)
	{
		if ( rpc_args_len /*+ 3 self_addr + remote_addr + cmd*/ > 29/*32*/)				/* impossible to send in one packet */
			self_addr |= 1;				/* set first bit */
		else
			self_addr &= ~(1);
		
		Enqueue(&dwire_tx_queue, (void*) &self_addr, 1);
		Enqueue(&dwire_tx_queue, (void*) &remote_address, 1);
		Enqueue(&dwire_tx_queue, (void*) &rpc_code, 1);
		
		if (rpc_args_len > 29)
		{
			Enqueue(&dwire_tx_queue, (void*) &rpc_args, 29);
			rpc_args_len -= 29;
		}
		else
		{
			Enqueue(&dwire_tx_queue, (void*) &rpc_args, rpc_args_len);
			break;
		}
	}
    SETHIGH(dwire_machine_flags, DW_FLAGS_TX_READY);
	dwire_rpc_call_callback = callback;
	_dwire_notify_send();
    return dwire_tx_queue.Count;
}

__DWEVT void _dwire_txds_handler(void)
{
	if ( GETBIT(dwire_machine_flags,DW_FLAGS_TX_READY) == 0 )				/* tx not ready, exit*/
    {
        /* possibly last packet has been transferred, and so enter here */
        _dwire_mode(MODE_RX);
        return;
    }
	uint8_t tx[32];
	int32_t count = dwire_tx_queue.Count;
	if (count > 32)
	{
		Dequeue(&dwire_tx_queue, tx, 32);
		_dwire_send((void*)tx, 32);
		_dwire_mode (MODE_TX);
        return ;
	}
	else if (count > 0) 
	{
		Dequeue(&dwire_tx_queue, tx, count);
		_dwire_send((void*)tx, count);	
		_dwire_mode (MODE_TX);		
	}
	else /* if (count ==0) */
	{
		/* tx completed */
		
	}
    SETLOW(dwire_machine_flags, DW_FLAGS_TX_READY);				/*dwire_machine_flags &= ~( 1 << DW_FLAGS_TX_READY);  remove tx ready flag */
    _dwire_mode(MODE_RX);
}

__DWEVT void _dwire_maxrt_handler(void)
{
	/* when enter here, the flag in 0x07 should be cleared first */
    //_dwire_mode(MODE_TX);	
    //_dwire_mode(MODE_RX);
	dwire_tx_fail_counter ++ ;				/* count fail transmition */
	
	if ( dwire_tx_fail_max == 0 )			/* infinite retransmit */
    {
        _dwire_mode(MODE_TX);
		return;
    }
	
	if ( dwire_tx_fail_max >= dwire_tx_fail_counter)
    {

        _dwire_mode(MODE_TX);
        return;
    }
    dwire_tx_fail_counter = 0;
    _dwire_abort_tx();
    _dwire_mode(MODE_RX);
}

__DWEVT void _dwire_rxdr_handler(void)
{
	uint8_t rx_len = _dwire_get_dynamic_length();
	uint8_t rx_buf[32];
	if (rx_len > 32)
	{
		/* weired packet received, TODO: boardcast I got a bad packet! */
		_dwire_abort_rx ();	
	}
	else
	{
		_dwire_read_rx (rx_buf, rx_len);				/* read buffer*/
		
		if (rx_buf[1] != DW_LOCAL_ADDR /* && rx_buf[1] != DW_BOARDCAST_ADDR */)
		{
				return;				/* address not match */
		}
		
		if ( (GETBIT( dwire_machine_flags,DW_FLAGS_RX_READY)))
		{
				//previous packet is not handled
				return;
		}
		
		if ((rx_buf[0] & 0x01) == 0)						/* no preceeding packet */
			SETHIGH (dwire_machine_flags, DW_FLAGS_RX_READY);
		
		Enqueue (&dwire_rx_queue, rx_buf, rx_len);
	}
}

void _dwire_notify_send(void)
{
	/* simply initiate the transmission */
	uint8_t buf[32];
	uint8_t count = dwire_tx_queue.Count > 32 ? 32: dwire_tx_queue.Count;
	Dequeue ( &dwire_tx_queue, buf, count);
	_dwire_send (buf,count);
    _dwire_mode(MODE_TX);
    //_dwire_mode(MODE_RX);
	
}

/* export nrf24l01p code */
void _dwire_send(uint8_t* buffer, uint8_t len)
{
	_nrf24l01_write_tx(buffer,len);
}
void _dwire_abort_tx (void)
{
	_nrf24l01_flush_tx();
}
void _dwire_abort_rx (void)
{
	_nrf24l01_flush_rx();
}
uint8_t _dwire_get_dynamic_length(void)
{
	return _nrf24l01_read_rx_payload_width();
}
void _dwire_read_rx (uint8_t* buffer, uint8_t length)
{
	_nrf24l01_read_rx(buffer,length);
}
void _dwire_mode (uint8_t mode)
{
    nrf24l01_mode (mode);
}

int32_t __dw_rpc_get_var(uint32_t reg_code, uint8_t *ret_buf)
{
	/* Not Implemented */
	return 0;
}

