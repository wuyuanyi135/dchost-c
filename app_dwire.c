#include "app.h"
#include "dwire.h"
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <math.h>
#include "config.h"
#include <stdlib.h>
#include <stdio.h>

void irq_handler(void)
{

   uint8_t status = nrf24l01_read_clear_interrupt();

    printf ("IRQ ENTER!!!!! status: %d\n", status);
   
   if (GETBIT(status, REG_STATUS_RX_DR))
   {
      /* Data Ready */
      _dwire_rxdr_handler();
      printf ("DATA RECV: %d\n", dwire_rx_queue.Count);
   }

   if (GETBIT(status, REG_STATUS_TX_DS))
   {
       _dwire_txds_handler();
   }

   if (GETBIT(status, REG_STATUS_MAX_RT))
   {
       _dwire_maxrt_handler();
       printf ("maxrt! %d\n", dwire_tx_fail_counter);
   } 
         
//   dwire_rpc_machine();         /* dont do this in embed platform */
}

void configure_address (void)
{
   nrf24l01_set_rxaddr (0,RX_ADDR_P0);
   nrf24l01_set_rxaddr (1,RX_ADDR_P1);
   nrf24l01_set_txaddr (TX_ADDR);
}

void configure_isr(void)
{
   int status = wiringPiISR(IRQ_PIN, INT_EDGE_FALLING,irq_handler); 
   /* Clear status first*/
   if (status < 0)
   {
      perror ("Failed to register ISR");
      exit (1);
   }
   nrf24l01_clear_interrupt();
   _nrf24l01_flush_rx();
   _nrf24l01_flush_tx();
   
   
}
void setup (void)
{
   dwire_init();
   dwire_tx_fail_max = 30;              /* max retransmission times */

   int status = wiringPiSetupGpio();
   if(status < 0)
   {
      perror ("Failed to configure GPIO");
      exit (1);
   }

   pinMode (IRQ_PIN, INPUT);
   pinMode (CE_PIN, OUTPUT);

   status = wiringPiSPISetup (CHANNEL,SPEED);    
   if (status < 0)
   {
      perror ("Failed to configure SPI");
      exit(1);
   }

   configure_address();
   //TODO dynamic length nrf24l01_set_dynamic_payload_length(0, ENABLE);
   configure_isr();
      
   nrf24l01_set_retry_count(15);
   nrf24l01_set_retry_delay(3);
   nrf24l01_set_rx_payload_length(1,32);
   nrf24l01_set_en_aa(0,ENABLE);
   nrf24l01_set_en_aa(1,ENABLE);
   nrf24l01_set_dynamic_payload_length(0, ENABLE);
   nrf24l01_set_dynamic_payload_length(1, ENABLE);
   nrf24l01_power_up(ENABLE);
   nrf24l01_mode(MODE_RX);
   printf ("Configured\n");

   dwire_rpc_call(0x10, RPC_CMD_ALIVE, NULL,0,  NULL);
   
}
