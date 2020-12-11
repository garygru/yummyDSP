# yummyDSP
yummyDSP is an Arduino audio DSP library for the [Espressif ESP32](https://www.espressif.com/en/products/hardware/esp32/overview). 
Main focus is **realtime** processing of audio signals for e.g. a guitar stomp box.  
Although there is some basic synth stuff inside, there are probably better audio synthesis libraries out there. 
For web radios, player, recorder or similar have a look at the [Espressif Systems Audio Development Framework](https://github.com/espressif/esp-adf).

The Library is in an early state, still lot's of known issues! Anyone is invited to help, extend and bring in his expertise. 

The idea is to have modular blocks which are simply arranged in a processing tree to form the signal chain. 
These processing blocks are called *Nodes* inspired by Apple's *Audio Unit* framework. They are processed one after another in the sequence in which they are added to the tree:

```
YummyDSP dsp;
FilterNode lp; // a low pass filter
WaveShaperNode sat; // some saturation
...
dsp.add(&lp);
dsp.add(&sat);
...
sample = dsp.process(sample);

```
This allows for a pretty high level view on signal processing but also to dig deeper and write your own nodes or directly add dsp code in the sample loop. 

Currently there a just a few nodes implemented (more to come)

- Hipass / Lopass / Bandpass filter
- Waveshaper / Saturation
- Delay (still dirty and experimental :)

## Getting Started

First you'll need some sort of an ESP32 e.g. [ESP32 DevKitC](https://www.espressif.com/en/products/hardware/esp32-devkitc/overview) or a derivate.  
You also need an ADC or audio codec connected via I2S. The AI-Thinker ESP32 Audio Kit is cheap and has everything on board to start. 
 

### Installation
- Install the [Arduino ESP32 Core](https://github.com/espressif/arduino-esp32) as described. 
- Download this library into your Arduino library folder
- Run the example code and fiddle around 


## ESP32
The ESP32 has two cores at 240 MHz and features a Bluetooth and Wifi module (hello OSC). The WROOM32 module has just 520 kB of RAM, the WROVER32 module at least additional 4MB (you'll want that for many lookup tables, delay line, etc.).  
The ESP32 is running a realtime OS, namely freeRTOS managing tasks, threading and all that. 
Audio processing is running on one core, leaving the other for control and housekeeping stuff. 

## Fun Facts
- Processing is sample wise 
- I2S buffers are handled by DMA
- I2S master clock is provided on GPIO0. This pin needs to be high on boot up!
- Latency is low, depends on the I2S buffer size (currently a few ms)
- Tested with AKM AK4552 and AK4556 audio codec and AC101 / AI-Thinker Audio Development Kit


## License

This project is licensed under the GPLv3 License - see the [LICENSE](LICENSE) file for details

