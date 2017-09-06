/*
 * Copyright 2017 Antonino Maniscalco 
 * 
 * QCW controller is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * QCW controller is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Nome-Programma.  If not, see http://www.gnu.org/licenses/ .
*/

//#include<MIDI.h>
#include"synth.h"

#define CHANEL 0b00001010






#define NOTE_ON 0b10000000

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// Define various ADC prescaler
const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x07; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}

enum 
{
  MIDI, MANUAL 
} mode;

void setup() 
{

  // set prescale to 16
  ADCSRA &= ~PS_128;

  // you can choose a prescaler from above.
  // PS_16, PS_32, PS_64 or PS_128
  ADCSRA |= PS_16;    // set our own prescaler to 64 
  // put your setup code here, to run once:
  
  
  setPwmFrequency(6, 1);
  pinMode(6, OUTPUT);
  pinMode(13, OUTPUT);
  
  pinMode(2,INPUT);
  pinMode(8,INPUT);

  mode = digitalRead(8) ? MANUAL : MIDI;
}

int out =0;
bool b;



#define SUPPLY_VOLTAGE 350.0f
#define BASE_VOLTAGE 40.0f

void manual_mode()
{
  if(mode != MANUAL)
  {
    //do once when mode is switched
    Serial.end();
  }
  mode = MANUAL;
  
  if(!digitalRead(2))return;      //does nothing if the trigger button is not pressd
 
  /*if(out > 255){out = 0;b = !b;digitalWrite(13,b);}
  analogWrite(6, out);
  int Dummy = analogRead(A0);
  out++;*/
  
  
  float amplitude = analogRead(A3) / 1024.0f; 
 
  int dt = analogRead(A1);    //it is actually the frequency
  dt /= 5;
  int delay_ms = 20 + dt;     //adds a minumum 
  delay_ms *= 57;             //compensates for messed up timing due to pwm frequency change

  int rp = analogRead(A2);    //ton
  int ramp_step = 5 + (rp/12);
  ramp_step /= 3;

  //this calculates the necessary value to add in order to get the initial voltage that starts the oscillation
  int base_value = BASE_VOLTAGE / SUPPLY_VOLTAGE * 255;
  
  for(int i = 0;i < 5;i++)
    delay(delay_ms);
  
  analogWrite(6, base_value);
  delay(57);
  digitalWrite(13,HIGH);
  delay(57);
  for(int i = 0;i <= 255;i++)
  {
     analogWrite(6, i * amplitude + base_value);
     delayMicroseconds(ramp_step);
  }
 
  for(int i = 255;i >=0;i-=2)
  {
     analogWrite(6, i * amplitude + base_value);
     delayMicroseconds(1);
  }
  delay(57);
  digitalWrite(13,LOW);
  delay(57);
  analogWrite(6, 0);
}

void midi_mode()
{
  if(mode != MIDI)
  {
    //do once when mode is switched
    Serial.begin(31250);
    //Serial.print("test");
  }
  mode = MIDI;


  byte command_byte;
  byte note_byte;
  byte velocity_byte;

  //bool note_available= Serial.available() > 2;

  do
  {
    if (Serial.available())
    {
      command_byte = Serial.read();//read first byte
      note_byte = Serial.read();//read next byte
      velocity_byte = Serial.read();//read final byte

      if((command_byte & 0b00001111) != CHANEL || (command_byte & 0b11110000) != NOTE_ON || velocity_byte <= 0)
        return;
      
      float tup = 5.0f + float(analogRead(A2)) / 102.4f;    //ton in ms
      tup = m_to_u(tup);      //converts to unit
      float td = tup / 10.0f; //ramp down is 1/10 of ramp up
    
      float ramp_amplitude = float(analogRead(A3)) / 1462.0f; //maximum of 0.7
      float drum_amplitude = ramp_amplitude * 0.3f;
    
      //float drum_ampl, float tup, float td, float base, float ramp_ampl, drum_types drumt
      play_drum(drum_amplitude, tup, td, BASE_VOLTAGE / SUPPLY_VOLTAGE, ramp_amplitude, (drum_types)note_byte);


      
    }
  }
  while (Serial.available() > 2);

  //if(!note_available) return;

  /*if(note_available)
  {
    if((command_byte & 0b00001111) != CHANEL)
      return;

    if((command_byte & 0b11110000) != NOTE_ON)
      return;
  }*/

  //if(!digitalRead(2))return;

  /**/
  
}

void loop() 
{
  if(digitalRead(8))
    midi_mode();
  else
    manual_mode();  
}
