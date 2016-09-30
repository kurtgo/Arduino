/*
 * BitBangSerial.h
 *
 *  Created on: Sep 14, 2016
 *      Author: kghome
 */

#ifndef LIBRARIES_BITBANGSERIAL_BITBANGSERIAL_H_
#define LIBRARIES_BITBANGSERIAL_BITBANGSERIAL_H_


#include <arduino.h>

class BitBangSerial : public Print
{

    //http://www.dnatechindia.com/Tutorial/8051-Tutorial/BIT-BANGING.html

    public:
      BitBangSerial(int pin_init)
      {
    	  pin = pin_init;
    		pinMode(pin, OUTPUT);
    		digitalWrite(pin, HIGH);   // turn the LED on (HIGH is the voltage level)
      }
      virtual ~BitBangSerial()
      {

      }

      virtual size_t write(uint8_t c) {
         int periodMicroSecs = 102; //1/9600  interval
         //int periodMicroSecs = 26;  //1/38400 interval

         writePin(LOW);
         delayMicroseconds(periodMicroSecs);
         for(uint8_t b = 0; b < 8; b++)
         {
           writePin(c & 0x01);
           c >>= 1;
           delayMicroseconds(periodMicroSecs);
         }

         //stop bit
         writePin(HIGH);
         delayMicroseconds(periodMicroSecs*2);
         return 1;
      };

      void writePin(bool value)
      {
  		digitalWrite(pin, value);   // turn the LED on (HIGH is the voltage level)
      };
    private:
      	  int pin;
};
#endif /* LIBRARIES_BITBANGSERIAL_BITBANGSERIAL_H_ */
