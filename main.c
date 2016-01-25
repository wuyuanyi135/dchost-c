#include "app.h"
#include <stdio.h>
#include <wiringPi.h>
#include "nrf24l01.h"
void rq (void)
{
   printf("IRQ\n");
}
int main (int argc, char** argv)
{

   setup();
//   wiringPiISR(24, INT_EDGE_FALLING, rq);
   while (1)
   {
      printf ("%d %d %llx\n",
            nrf24l01_read_status(),
            _nrf24l01_get_reg(0),
            (unsigned long long)nrf24l01_get_rxaddr(0)
            );
      delay(1000);
   }
   return 0;
}
