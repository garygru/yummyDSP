# yummyDSP
is an Arduino audio DSP library for the [Espressif ESP32](https://www.espressif.com/en/products/hardware/esp32/overview). 
Main focus is **realtime** processing of audio signals for e.g. a guitar stomp box.  
Although there is some basic synth stuff inside, there are better audio synthesis libraries out there. 
For web radios or similar use the [Espressif Systems Audio Development Framework](https://github.com/espressif/esp-adf)

The Library is still in an early state!

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
This is a pretty high level view on signal processing but also allows to dig deeper and write your own nodes or just add the code in the sample loop. 

Currently there a just a few nodes implemented (more to come)

- Hipass / Lopass / Bandpass filter
- Waveshaper / Saturation
- Delay (still dirty and experimental :)

## Getting Started

First you'll need some sort of an ESP32 e.g. [ESP32 DevKitC](https://www.espressif.com/en/products/hardware/esp32-devkitc/overview) or a derivate.  
Currently you also need an ADC or audio codec connected via I2S.  
*I didn't manage yet to make the built-in ADC work with I2S DMA without breaking the DAC. If someone has an advice, please drop me a line*. 
 

###Installation
- Install the [Arduino ESP32 Core](https://github.com/espressif/arduino-esp32) as described. 
- Download this library into your Arduino library folder
- Run the example code and fiddle around 


## ESP32
The ESP32 has two cores at 240 MHz and features a Bluetooth and Wifi module (hello OSC). 
Audio processing is running on one core, leaving the other for control and housekeeping stuff. 

## Fun Facts
- Processing is sample wise 
- I2S buffers are handled by DMA
- Latency is low, depends on the I2S buffer size (currently a few ms)
- The audio driver is only tested with the AKM AK4556 audio codec. A proper hardware abstraction layer is still missing. 


## License

This project is licensed under the GPLv3 License - see the [LICENSE.md](LICENSE.md) file for details

