/*
 WaveSynth – Monophonic wavetable synthesizer

 16 Bit Fixed Point Wavetable synthesis
 Linear interpolated table lookup
 Linear smoothed volume
 Linear smoothed frequency

 Author: Gary Grutzek
 gary.grutzek@ib-gru.de
 */

#include "WaveSynth.h"

WaveSynth::WaveSynth() {
	this->begin(48000, 1024);
}


WaveSynth::~WaveSynth() {
	if (waveTable != NULL) {
		delete waveTable;
	}
	delete(interpolator);
	delete(envelope);
}


void WaveSynth::begin(int sampleRate, int waveTableSize) {
	tableSize = waveTableSize;
	fs = sampleRate;

	glide = 1;
	interpolator = new Interpolator(fs, glide); // glide
	envelope = new Envelope(fs);

	offset = 0; // no DC offset
	setFrequency(440.f);

	waveformType = SINE;
	waveTable = new float[tableSize + 1];
	generateWavetable();
	//randomSeed(analogRead(0));
}


void WaveSynth::setWaveform(int type) {
	waveformType = type;
	generateWavetable();
}


float WaveSynth::getSample() {
	float sample = 0;
	switch (waveformType) {
	case SINE:
		sample = getWaveSample();
		break;
	case SAW:
		sample = getWaveSample();
		break;
	case NOISE:
		sample = getNoiseSample();
		break;
	}
	return sample * envelope->process();
}

/*
 * precalculate waveforms
 */
void WaveSynth::generateWavetable() {
	float f0 = fs / (float)tableSize; // fundamental frequency of wave table
	float phase = 0.f;

	switch (waveformType) {
	case SINE: {  // sine table
		for (int k = 0; k < tableSize; k++) {
			waveTable[k] = sin(phase) + offset;
			phase += 2 * PI * f0 / (float)fs;
		}
		waveTable[tableSize] = waveTable[0];
	} break;

	case SAW: {  // saw table
		for (int k = 0; k < tableSize; k++) {
			waveTable[k] = phase + offset;
			phase += 1.f / tableSize;
		}
		waveTable[tableSize] = waveTable[0];
	} break;

	case NOISE: // no table for noise
		break;
	}
	// setFrequency(440.f, false);
}


float WaveSynth::getWaveSample() {
	// linear interpolation
	float frac = phase - int(phase);
	float sample = (1 - frac) * waveTable[int(phase)] + frac * waveTable[int(phase) + 1];
	phase += delta;
	// wrap around
	if (phase >= tableSize) {
		phase -= tableSize;
	}

	// frequency smoothing
	interpolator->process();
	delta = tableSize / (fs / freq[kCurrent]);

	return constrain(sample, -1.0, 1.0);
}


float WaveSynth::getNoiseSample() {
	static const int scale = pow(2, 15); // scale samples to 16 Bit signed values
	float sample = (float)random(-scale, scale) / (float)scale;

	return constrain(sample, -1.0, 1.0);
}


void WaveSynth::setFrequency(float frequency) {
	freq[kTarget] = constrain(frequency, 0, fs / 2);
	if (glide < 1) { // no smoothing
		freq[kCurrent] = freq[kTarget];
	} else {
		interpolator->add(&freq[kCurrent], freq[kTarget]);
	}
	delta = tableSize / (fs / freq[kCurrent]);
}

void WaveSynth::note(float frequency) { // TODO: velocity
	setFrequency(frequency);
	envelope->noteOn();
}

void WaveSynth::note(int midinote) { // TODO: velocity
	int num = constrain(midinote, 1 , 127);
	float f = powf(2.f, (num-69)/12.f)*440;
	//Serial.println(f);
	setFrequency(f);
	envelope->noteOn();
}


