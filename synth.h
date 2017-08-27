//#define PI 3.14159265f
/****************************math********************************/
inline float fract(float a)
{
  int i_part = a;   //extract integer part
  return a-i_part;  //decimnal part is simply the float part minus the integer part
}

float rand(float n){return fract(sin(n) * 43758.5453123);}
/****************************math********************************/

/*inline float decaying_sine(float frequency, float decay_rate, unsigned long time) //frequenzy in Hz and time sould start from 0 us
{
  float ftime = float(time)/1000.0f;
  return sin(ftime/1000.0f*2.0*PI) / exp(-10.0*ftime*1000);
}*/

struct f_out
{
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

inline float ramp(float tup, float td, float base, float ampl, float ftime)   
{
  //float ftime = float(time)/1000.0f;
  float rawramp = ftime < tup ? ftime/tup : 1.0f - ((ftime-tup)/td);
  return base + ampl*rawramp;
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

void play_drum(drum_types drumt)
{
  float amplitude = 1.0f;
  switch(drumt)
  {
    case KICK:
      break;
    case SNARE:
      break;
    case LTOM:
      break;
    case LMTOM:
      break;
    case HTOM:
      break;
  }
}

