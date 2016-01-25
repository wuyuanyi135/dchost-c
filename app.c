#include "app.h"
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <math.h>
#include "config.h"
#include <stdlib.h>
#include <stdio.h>

void irq_handler(void)
{

   uint8_t status = nrf24l01_read_clear_interrupt();
   
   if (GETBIT(status, REG_STATUS_RX_DR))
   {
      /* Data Ready */
      uint16_t temperature[4];
      _nrf24l01_read_rx((uint8_t *) temperature, 8);
      
      double tmp;
      for (uint8_t i; i<4;i++)
      {
         if (GETBIT(temperature[i],11) == ENABLE)
            temperature[i] = ~ temperature[i];

         tmp = 0;
         for (uint8_t j=0 ; j<11; j++)
         {
            tmp += pow (2.0d, (double)j - 4.0d);  
            printf ("%f\t",tmp);
         }
         printf("\n");
      }
   }
         
}

void configure_address (void)
{
   nrf24l01_set_rxaddr (0,RX_ADDR_P0);
   nrf24l01_set_txaddr (TX_ADDR);
}

void configure_isr(void)
{
   int status = wiringPiISR(IRQ_PIN, INT_EDGE_FALLING,irq_handler); 
   if (digitalRead (IRQ_PIN)==LOW)
   {
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
}
void setup (void)
{
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
   nrf24l01_power_up(ENABLE);
   nrf24l01_mode(MODE_RX);

   printf ("Configured\n");
}
