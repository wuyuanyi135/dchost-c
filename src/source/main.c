#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>

void
irq_handler (void)
{
   printf ("irq occured\n");

}
int
main (int argc, char** argv)
{

   int status = wiringPiSetupGpio();
   if (status < 0)
   {
      printf ("Fail to setup wiringPi. Are you root?");
      exit(1);
   }

   pinMode (24,INPUT);

   wiringPiISR(24, INT_EDGE_FALLING, irq_handler);

   status = wiringPiSPISetup(0,500000);
   if (status < 0)
   {
      printf("Failed to configure SPI");
      exit (1);
   }   
   
   unsigned char buf [256];
   pause();
   while (1)
   {
      buf[0] = 0x07;
      buf[1] = 0xff;
      wiringPiSPIDataRW(0, buf, 2);
      printf ("Output %d \n",buf[1]);
      delay (1000);
      
   }
}
