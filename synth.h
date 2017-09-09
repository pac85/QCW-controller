#include <Arduino.h>
//#define PI 3.14159265f

inline void f_analogWrite(float f)
{
  analogWrite(6, f*255);
}

//unmessed version of delayMicroseconds
inline void u_delayMicroseconds(unsigned long int us)
{
  delayMicroseconds(us);
}

//unmessed version of delay
inline void u_delay(unsigned long int us)
{
  delay(us*64);
}

//unmessed version of micros
inline unsigned long u_micros()
{
  return micros()/64;
}

/****************************math********************************/
inline float fract(float a)
{
  int i_part = a;   //extract integer part
  return a-i_part;  //decimnal part is simply the float part minus the integer part
}

float rand(float n){return fract(sin(n) * 43758.5453123);}

inline float m_to_u(float a){return a/1000.0f;}//from milli to unit
/****************************math********************************/

/*inline float decaying_sine(float frequency, float decay_rate, unsigned long time) //frequenzy in Hz and time sould start from 0 us
{
  float ftime = float(time)/1000.0f;
  return sin(ftime/1000.0f*2.0*PI) / exp(-10.0*ftime*1000);
}*/

struct f_out
{
  f_out(){}

  f_out(float a, float b)
  {
    wave = a;
    amplitude = b;
  }
  float wave, amplitude;
};

/***************************drums*******************************/

inline f_out kick(float ftime)
{
  float amplitude = exp(-15.0*ftime);
  return f_out(sin(6.2831*45.0*ftime)*amplitude, amplitude);
}

inline f_out snare(float ftime)
{
  float amplitude = exp(-45.0*ftime)+exp(-55.0*ftime);
  return f_out((sin(6.2831*230.0*ftime))*exp(-55.0*ftime)*0.8 + rand(ftime*100.0)*exp(-45.0*ftime), amplitude);
}

inline f_out ltom(float ftime)
{
  float amplitude = exp(-35.0*ftime);
  return f_out(sin(6.2831*130.0*ftime)*amplitude, amplitude);
}

inline f_out lmtom(float ftime)
{
  float amplitude = exp(-35.0*ftime);
  return f_out(sin(6.2831*150.0*ftime)*amplitude, amplitude);
}

inline f_out htom(float ftime)
{
  float amplitude = exp(-35.0*ftime);
  return f_out(sin(6.2831*190.0*ftime)*amplitude, amplitude);
}
/***************************drums*******************************/

inline float ramp(float tup, float td, float ampl, float ftime)
{
  return max(ftime < tup ? ftime/tup : 1.0f - ((ftime-tup)/td), 0);
}

enum  drum_types
{
  /*SNARE = 38,
  KICK = 35,
  KICK1 = 36,*/
  KICK = 36,
  SNARE = 38,
  //C_HIHAT = 42,
  //O_HIHAT = 46,
  LTOM = 45,
  LMTOM = 47,
  HTOM = 50
  //C_CYMB = 49,
  //R_CYMB = 51,
  //R_BELL =53

};

void play_drum(float drum_ampl, float tup, float td, float base, float ramp_ampl, drum_types drumt)
{
  tup = min(m_to_u(5), tup);

  const float stop_threshold = 0.01f;    //once the function amplitude reaches this value it will stop being played
  const float drum_delay = m_to_u(5.0f);//after how many ms the drum sound as to start
  //const float tup= m_to_u(10.0f), td=m_to_u(1.0f), base = 40.0f / 310.0f, ampl = ;
  float f_amplitude = 1.0f;             //the amplitude of the drum function

  //writes initial value and waits
  f_analogWrite(base);
  u_delay(1);
  digitalWrite(13, HIGH);

  unsigned long start_time;
  switch(drumt)
  {
    case KICK:
      start_time = u_micros();
      while(f_amplitude > stop_threshold)
      {
        //calculates time making shure that it starts from zero
        float ftime = (u_micros()-start_time)/1000000.0f;

        //drum sound
        f_out drum_r;
        if(ftime > drum_delay)
          drum_r = kick(ftime - drum_delay);
        else
          drum_r = f_out(0.0f, 10.0f);

        //ramp
        float ramp_r = ramp(tup, td, ramp_ampl, ftime);

        //combines the two
        float combined = drum_ampl*drum_r.wave + ramp_ampl*ramp_r;
        //adds bias(half the drum sound amplitude unless it is less than the base voltage)
        combined += max(base, drum_ampl*drum_r.amplitude/2.0f);

        f_analogWrite(combined);

        f_amplitude = drum_r.amplitude;
      }
      break;
    case SNARE:
      start_time = u_micros();
      while(f_amplitude > stop_threshold)
      {
        //calculates time making shure that it starts from zero
        float ftime = (u_micros()-start_time)/1000000.0f;

        //drum sound
        f_out drum_r;
        if(ftime > drum_delay)
          drum_r = snare(ftime - drum_delay);
        else
          drum_r = f_out(0.0f, 10.0f);

        //ramp
        float ramp_r = ramp(tup, td, ramp_ampl, ftime);

        //combines the two
        float combined = drum_ampl*drum_r.wave + ramp_ampl*ramp_r;
        //adds bias(half the drum sound amplitude unless it is less than the base voltage)
        combined += max(base, drum_ampl*drum_r.amplitude/2.0f);

        f_analogWrite(combined);

        f_amplitude = drum_r.amplitude;
      }
      break;
    case LTOM:
      start_time = u_micros();
      while(f_amplitude > stop_threshold)
      {
        //calculates time making shure that it starts from zero
        float ftime = (u_micros()-start_time)/1000000.0f;

        //drum sound
        f_out drum_r;
        if(ftime > drum_delay)
          drum_r = ltom(ftime - drum_delay);
        else
          drum_r = f_out(0.0f, 10.0f);

        //ramp
        float ramp_r = ramp(tup, td, ramp_ampl, ftime);

        //combines the two
        float combined = drum_ampl*drum_r.wave + ramp_ampl*ramp_r;
        //adds bias(half the drum sound amplitude unless it is less than the base voltage)
        combined += max(base, drum_ampl*drum_r.amplitude/2.0f);

        f_analogWrite(combined);

        f_amplitude = drum_r.amplitude;
      }
      break;
    case LMTOM:
      start_time = u_micros();
      while(f_amplitude > stop_threshold)
      {
        //calculates time making shure that it starts from zero
        float ftime = (u_micros()-start_time)/1000000.0f;

        //drum sound
        f_out drum_r;
        if(ftime > drum_delay)
          drum_r = lmtom(ftime - drum_delay);
        else
          drum_r = f_out(0.0f, 10.0f);

        //ramp
        float ramp_r = ramp(tup, td, ramp_ampl, ftime);

        //combines the two
        float combined = drum_ampl*drum_r.wave + ramp_ampl*ramp_r;
        //adds bias(half the drum sound amplitude unless it is less than the base voltage)
        combined += max(base, drum_ampl*drum_r.amplitude/2.0f);

        f_analogWrite(combined);

        f_amplitude = drum_r.amplitude;
      }
      break;
    case HTOM:
      start_time = u_micros();
      while(f_amplitude > stop_threshold)
      {
        //calculates time making shure that it starts from zero
        float ftime = (u_micros()-start_time)/1000000.0f;

        //drum sound
        f_out drum_r;
        if(ftime > drum_delay)
          drum_r = htom(ftime - drum_delay);
        else
          drum_r = f_out(0.0f, 10.0f);

        //ramp
        float ramp_r = ramp(tup, td, ramp_ampl, ftime);

        //combines the two
        float combined = drum_ampl*drum_r.wave + ramp_ampl*ramp_r;
        //adds bias(half the drum sound amplitude unless it is less than the base voltage)
        combined += max(base, drum_ampl*drum_r.amplitude/2.0f);

        f_analogWrite(combined);

        f_amplitude = drum_r.amplitude;
      }
      break;

     default:
      start_time = u_micros();
      while(f_amplitude > stop_threshold)
      {
        //calculates time making shure that it starts from zero
        float ftime = (u_micros()-start_time)/1000000.0f;

        //drum sound
        f_out drum_r;
        if(ftime > drum_delay)
          drum_r = htom(ftime - drum_delay);
        else
          drum_r = f_out(0.0f, 10.0f);

        //ramp
        float ramp_r = ramp(tup, td, ramp_ampl, ftime);

        //combines the two
        float combined = drum_ampl*drum_r.wave + ramp_ampl*ramp_r;
        //adds bias(half the drum sound amplitude unless it is less than the base voltage)
        combined += max(base, drum_ampl*drum_r.amplitude/2.0f);

        f_analogWrite(combined);

        f_amplitude = drum_r.amplitude;
      }
      break;


  }
  digitalWrite(13, LOW);
  u_delay(1);
  f_analogWrite(0);
}
