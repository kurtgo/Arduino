#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>
/*
This is the corresponding sketch to the 'basicSend' sketch.
the nrf24l01 will listen for numbers 0-255, and light the red LED
whenever a number in the sequence is missed.  Otherwise,
it lights the green LED
*/
int msg[1];
RF24 radio(8,7);
const uint64_t pipe = 0xE8E8F0F0E1LL;
int red = 6;
int green = 4;
int redNeg = 3;
int greenNeg = 5;
int lastmsg = 1;
 
void setup(void){
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(1,pipe);
  radio.printDetails();
  radio.startListening();
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(redNeg, OUTPUT);
  pinMode(greenNeg, OUTPUT);
  digitalWrite(greenNeg, LOW);
  digitalWrite(redNeg, LOW);
}
 
void loop(void){
  if (radio.available()){
    bool done = false;  
    

    while (!done){
      done = radio.read(msg, 2); 
      if (msg[0] != lastmsg +1){
        digitalWrite(red, HIGH);
        digitalWrite(green,LOW);
        }    
      else {
       digitalWrite(red,LOW);
       digitalWrite(green,HIGH); 
        }
      lastmsg = msg[0];
      //Serial.println(msg[0]);
     }
    }
    else {
      digitalWrite(red, HIGH);
      digitalWrite(green,LOW);
    }
}



