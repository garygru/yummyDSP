/*
 *  DSP Helpers
 *
 *  Author: Gary Grutzek
 *  gary@ib-gru.de
*/

#ifndef DSP_HELPERS_H
#define DSP_HELPERS_H


// Taken from https://github.com/ctag-fh-kiel/ctag-straempler/blob/master/components/audio/dsp_lib.c
inline float fasttanh(float x){
    float x2 = x*x;
    return x * ( 27.f + x2 ) / ( 27.f + 9.f * x2 );
}

#endif

