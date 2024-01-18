/*
 *  DSP Helpers
 *
 *  Author: Gary Grutzek
 *  gary@ib-gru.de
*/
#ifndef DSP_HELPERS_H
#define DSP_HELPERS_H

#include <Arduino.h>

// lookup tables
#define TABLE_BIT  		      5UL				    // bits per index of lookup tables. 10 bit means 2^10 = 1024 samples. Values from 5 to 11 are OK. Choose the most appropriate.
#define TABLE_SIZE            (1<<TABLE_BIT)        // samples used for lookup tables (it works pretty well down to 32 samples due to linear approximation, so listen and free some memory)
#define TABLE_MASK  	      (TABLE_SIZE-1)        // strip MSB's and remain within our desired range of TABLE_SIZE
#define DIV_TABLE_SIZE		  (1.0f/(float)TABLE_SIZE)
#define CYCLE_INDEX(i)        (((int32_t)(i)) & TABLE_MASK ) // this way we can operate with periodic functions or waveforms with auto-phase-reset ("if's" are pretty CPU-costly)
#define SHAPER_LOOKUP_MAX     5.0f                  // maximum X argument value for tanh(X) lookup table, tanh(X)~=1 if X>4 
#define SHAPER_LOOKUP_COEF	  ((float)TABLE_SIZE / SHAPER_LOOKUP_MAX)
#define TWOPI	6.283185307179586476925286766559f
#define ONE_DIV_TWOPI	0.15915494309189533576888376337251f	

static const float sin_tbl[TABLE_SIZE+1] = {
	0.000000000f, 0.195090322f, 0.382683432f, 0.555570233f, 0.707106781f, 0.831469612f, 0.923879533f, 0.980785280f,
	1.000000000f, 0.980785280f, 0.923879533f, 0.831469612f, 0.707106781f, 0.555570233f, 0.382683432f, 0.195090322f, 
	0.000000000f, -0.195090322f, -0.382683432f, -0.555570233f, -0.707106781f, -0.831469612f, -0.923879533f, -0.980785280f, 
	-1.000000000f, -0.980785280f, -0.923879533f, -0.831469612f, -0.707106781f, -0.555570233f, -0.382683432f, -0.195090322f, 0.000000000f };

static const float shaper_tbl[TABLE_SIZE+1] {
	0.000000000f, 0.154990730f, 0.302709729f, 0.437188785f, 0.554599722f, 0.653423588f, 0.734071520f, 0.798242755f, 
	0.848283640f, 0.886695149f, 0.915824544f, 0.937712339f, 0.954045260f, 0.966170173f, 0.975136698f, 0.981748725f, 
	0.986614298f, 0.990189189f, 0.992812795f, 0.994736652f, 0.996146531f, 0.997179283f, 0.997935538f, 0.998489189f, 
	0.998894443f, 0.999191037f, 0.999408086f, 0.999566912f, 0.999683128f, 0.999768161f, 0.999830378f, 0.999875899f , 0.999909204f };


inline float lookupTable(const float (&table)[TABLE_SIZE+1], float index ) { // lookup value in a table by float index, using linear interpolation
  static float v1, v2, res;
  static int32_t i;
  static float f;
  i = (int32_t)index;
  f = (float)index - i;
  v1 = (table)[i];
  v2 = (table)[i+1];
  res = (float)f * (float)(v2-v1) + v1;
  return res;
}

inline float fclamp(float in, float min, float max){
    return fmin(fmax(in, min), max);
}

inline float fast_shape(float x) {
    float sign = 1.0f;
    if (x < 0) {
      x = -x;
      sign = -1.0f;
    }
    if (x >= 4.95f) {
      return sign; // tanh(x) ~= 1, when |x| > 4
    }
	return  sign * lookupTable(shaper_tbl, (x*SHAPER_LOOKUP_COEF)); // lookup table contains tanh(x), 0 <= x <= 5
}

inline float fast_sin(const float x) {
	const float argument = ((x * ONE_DIV_TWOPI) * TABLE_SIZE);
	const float res = lookupTable(sin_tbl, CYCLE_INDEX(argument)+((float)argument-(int32_t)argument));
	return res;
}

inline float fast_cos(const float x) {
	const float argument = ((x * ONE_DIV_TWOPI + 0.25f) * TABLE_SIZE);
	const float res = lookupTable(sin_tbl, CYCLE_INDEX(argument)+((float)argument-(int32_t)argument));
	return res;
}

inline void fast_sincos(const float x, float* sinRes, float* cosRes){
	*sinRes = fast_sin(x);
	*cosRes = fast_cos(x);
}

// reciprocal asm injection for xtensa LX6 FPU
// https://blog.llandsmeer.com/tech/2021/04/08/esp32-s2-fpu.html
inline float one_div(float a) {
    float result;
    asm volatile (
        "wfr f1, %1"          "\n\t"
        "recip0.s f0, f1"     "\n\t"
        "const.s f2, 1"       "\n\t"
        "msub.s f2, f1, f0"   "\n\t"
        "maddn.s f0, f0, f2"  "\n\t"
        "const.s f2, 1"       "\n\t"
        "msub.s f2, f1, f0"   "\n\t"
        "maddn.s f0, f0, f2"  "\n\t"
        "rfr %0, f0"          "\n\t"
        : "=r" (result)
        : "r" (a)
        : "f0","f1","f2"
    );
    return result;
}

// taken from here: https://www.esp32.com/viewtopic.php?t=10540#p43343
// probably it's fixed already, so keep it here just in case
inline float divsf(float a, float b) {
    float result;
    asm volatile (
        "wfr f0, %1\n"
        "wfr f1, %2\n"
        "div0.s f3, f1 \n"
        "nexp01.s f4, f1 \n"
        "const.s f5, 1 \n"
        "maddn.s f5, f4, f3 \n"
        "mov.s f6, f3 \n"
        "mov.s f7, f1 \n"
        "nexp01.s f8, f0 \n"
        "maddn.s f6, f5, f3 \n"
        "const.s f5, 1 \n"
        "const.s f2, 0 \n"
        "neg.s f9, f8 \n"
        "maddn.s f5,f4,f6 \n"
        "maddn.s f2, f9, f3 \n" /* Original was "maddn.s f2, f0, f3 \n" */
        "mkdadj.s f7, f0 \n"
        "maddn.s f6,f5,f6 \n"
        "maddn.s f9,f4,f2 \n"
        "const.s f5, 1 \n"
        "maddn.s f5,f4,f6 \n"
        "maddn.s f2,f9,f6 \n"
        "neg.s f9, f8 \n"
        "maddn.s f6,f5,f6 \n"
        "maddn.s f9,f4,f2 \n"
        "addexpm.s f2, f7 \n"
        "addexp.s f6, f7 \n"
        "divn.s f2,f9,f6\n"
        "rfr %0, f2\n"
        :"=r"(result):"r"(a), "r"(b)
    );
    return result;
}
/*
inline float dB2amp(float dB){
  return expf(dB * 0.11512925464970228420089957273422f);
  //return pow(10.0, (0.05*dB)); // naive, inefficient version
}

inline float amp2dB(float amp){
  return 8.6858896380650365530225783783321f * logf(amp);
  //return 20*log10(amp); // naive version
}

inline float linToLin(float in, float inMin, float inMax, float outMin, float outMax){
  // map input to the range 0.0...1.0:
  float tmp = (in-inMin) * one_div(inMax-inMin);

  // map the tmp-value to the range outMin...outMax:
  tmp *= (outMax-outMin);
  tmp += outMin;

  return tmp;
}

inline float linToExp(float in, float inMin, float inMax, float outMin, float outMax){
  // map input to the range 0.0...1.0:
  float tmp = (in-inMin) * one_div(inMax-inMin);

  // map the tmp-value exponentially to the range outMin...outMax:
  //tmp = outMin * exp( tmp*(log(outMax)-log(outMin)) );
  return outMin * expf( tmp*(logf(outMax * one_div(outMin))) );
}

inline float expToLin(float in, float inMin, float inMax, float outMin, float outMax){
  float tmp = logf(in * one_div(inMin)) * one_div( logf(inMax * one_div(inMin)));
  return outMin + tmp * (outMax-outMin);
}

inline float knobMap(float in, float outMin, float outMax) {
  return outMin + lookupTable(knob_tbl, (int)(in * TABLE_SIZE)) * (outMax - outMin);
}

inline float knob_fill(uint16_t i) { // f(x) = (exp(k*x)-1)/b, 0 <= x <= 1, 0 <= f(x) <= 1, x mapped to [0 .. TABLE_SIZE]
  float x = (float)i * (float)DIV_TABLE_SIZE;
  float res = ( expf((float)(x * 2.71f))-1.0f) * 0.071279495455219f;
  return res;
}

inline float freqToPhaseInc(float freq, uint16_t sampleSize, uint16_t sampleRate) {
  return freq * (float)sampleSize / (float)sampleRate;
}
*/

#endif

