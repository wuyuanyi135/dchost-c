#include "app.h"
#include <stdio.h>
#include <wiringPi.h>
#include "nrf24l01.h"

int main (int argc, char** argv)
{

   setup();


   while (1)
   {

      nrf24l01_printall();
      delay(5000);
   }
   return 0;
}
