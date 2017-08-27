#include<MIDI.h>
#include"synth.h"

#define CHANEL 0b00000000






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
}

enum 
{
  MIDI, MANUAL 
} mode;

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
  if(!digitalRead(2))return;
 
  /*if(out > 255){out = 0;b = !b;digitalWrite(13,b);}
  analogWrite(6, out);
  int Dummy = analogRead(A0);
  out++;*/
  
  
  float amplitude = analogRead(A3) / 1024.0f;
 
  int dt = analogRead(A1);
  dt /= 5;
  int delay_ms = 20 + dt;
  delay_ms *= 57;

  int rp = analogRead(A2);
  int ramp_step = 5 + (rp/12);
  ramp_step /= 3;
  
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
    Serial.begin(9600);
    //Serial.print("test");
  }
  mode = MIDI;


  byte command_byte;
  byte note_byte;
  byte velocity_byte;

  bool note_available= Serial.available() > 2;

  do{
    if (Serial.available()){
      command_byte = Serial.read();//read first byte
      note_byte = Serial.read();//read next byte
      velocity_byte = Serial.read();//read final byte
    }
  }
  while (Serial.available() > 2);

  if(note_available)
  {
    if((command_byte & 0b00001111) != CHANEL)
      return;

    if((command_byte & 0b11110000) != NOTE_ON)
      return;
  }
  
}

void loop() 
{
  //if(digitalRead(8))
  //  manual_mode();
  //else
  mode = MANUAL;
    midi_mode();
  delay(1000);
}
