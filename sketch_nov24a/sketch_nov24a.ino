#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>
int msg[1];
RF24 radio(8,7);
const uint64_t pipe = 0xE8E8F0F0E1LL;
/*
this is the most basic sketch I can think of to transmit data from an nrf24l01.
It loops over the numbers 0-255 continuously and sends each number to the receiving 
unit.  I have used this pattern as a test to check for drops in the signal by 
checking the receiving end for gaps between numbers.  It's not sophisticated,
but it seems to work.
*/
 
void setup(void){
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(pipe);
}
 
void loop(void){
  radio.printDetails();
  for (int x=0;x<255;x++){
    delay(10);
  msg[0] = x;
  radio.write(msg, 1);
  
  //Serial.println("Radio message");
  Serial.println(x);
  radio.printDetails();
  }
}
