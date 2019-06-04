/*
 * 	WaveSynth – monophonic wavetable synthesizer
 *
 * 	floating point wavetable synthesis
 *	linear interpolated table lookup
 *	exponential ADSR envelope
 *
 *  Author: Gary Grutzek
 * 	gary@ib-gru.de
 */

#include "Arduino.h"
#include "Interpolator.h"

#ifndef WAVELIB
#define WAVELIB

// ADSR
class Envelope {
	enum states {
		ATTACK,
		DECAY,
		SUSTAIN,
		RELEASE,
		IDLE,
		kADSRCount
	};

	enum {
		OFF = 0,
		ON = 1
	};

public:
	// Envelope();
	//	~Envelope();
	Envelope(int fs) {
		begin(fs);
	};

	void begin(int samplerate) {
		fs = samplerate;
		overshoot = 0.05;
		state = IDLE;
		gain = 0;
		setADSR(50, 10, 0.8, 400);
	}

	void setADSR(float attack, float decay, float sustainLevel, float release) {
		setAttack(attack);
		setDecay(decay);
		setSustain(sustainLevel);
		setRelease(release);
	}

	void setAttack(float attackms) {
		adsr[ATTACK] = 1.f-expf(-1/(fs*attackms/3000.f));
	}

	void setDecay(float decayms) {
		adsr[DECAY] = 1.f-expf(-1/(fs*decayms/2000.f));
	}

	void setSustain(float sustainLevel) {
		adsr[SUSTAIN] = constrain(sustainLevel, 0.00001f, 1.f);
	}

	void setRelease(float releasems) {
		adsr[RELEASE] = 1.f-expf(-1/(fs*releasems/3000.f));
	}

	void noteOn() {
		state = ATTACK;
		gate = ON;
	}

	void noteOff() {
		gate = OFF;
	}

	float volume() {
		return gain;
	}

	inline float process() {
		switch (state) {
		case IDLE: gain = 0; break;
		case ATTACK: {
			gain = gain * (1.f-adsr[ATTACK]) + (1.f+overshoot) * adsr[ATTACK];
			if (gain >= 1.f) {
				gain = 1.f;
				state = DECAY;
			}
		} break;
		case DECAY: {
			gain = gain * (1.f-adsr[DECAY]) + (adsr[SUSTAIN]-overshoot) * adsr[DECAY];
			if (gain <= adsr[SUSTAIN]) {
				gain = adsr[SUSTAIN];
				state = SUSTAIN;
			}
		} break;
		case SUSTAIN: {
			if (!gate) {
				state = RELEASE;
			}
		} break;
		case RELEASE: {
			gain = gain * (1.f-adsr[RELEASE]) + (0.f-overshoot/5) * adsr[RELEASE];
			if (gain <= 0) {
				gain = 0.f;
				state = IDLE;
			}
		} break;
		}
		return gain;
	}

protected:
	int fs;
	bool gate;
	float gain;
	int state;
	float adsr[4];
	float overshoot;
};


enum type {
	SINE = 0,
	SAW,
	NOISE
};

class WaveSynth {

public:
	WaveSynth();
	~WaveSynth();

	void begin(int fs, int waveTableSize = 1024);

	void setWaveform(int type);

	// returns sample at current phase with envelope applied
	float getSample();

	int getSampleRate() {
		return fs;
	}

	float getPhase() {
		return phase;
	}

	float getFrequency() {
		return freq[kCurrent];
	}
	void setFrequency(float freq);

	void note(float frequency);
	void note(int note);

	void noteOn() { // retrigger
		envelope->noteOn();
	}

	void noteOff() {
		envelope->noteOff();
	}

	void setGlide(int ms) {
		int glide = constrain(ms, 0, 1000);
		if (interpolator) {
			interpolator->begin(fs, glide);
		}
	}

protected:
	Interpolator *interpolator;
	Envelope *envelope;

	void generateWavetable();
	float getWaveSample();
	float getNoiseSample();

	int fs; // sample rate
	int tableSize; // size of the wave table
	float *waveTable; // pointer to waveTable
	int waveformType; // SINE, SAW or NOISE

	float freq[2]; // current frequency and target frequency
	float phase; // phase accumulator
	float delta; // phase delta to next sample
	float offset; // DC offset
	int glide; // in ms
};

#endif


