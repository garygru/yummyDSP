# yummyDSP
is an Arduino audio DSP library for the [Espressif ESP32](https://en.wikipedia.org/wiki/ESP32). 
Main focus is realtime processing of audio signals like guitars (although there is some basic synth stuff inside).

The idea is to have modular blocks which are simply arranged in a processing tree to form the signal chain. 
These processing blocks are called Nodes inspired by the Apple Audiounit framework.

Currently there a just a few Nodes implemented (more to come)
- Hipass / Lopass / Bandpass filter
- Waveshaper
- Delay (still dirty and experimental :)

## ESP32
The ESP32 has two cores at 240 MHz and also a features Bluetooth and Wifi module (hello OSC). 
Audio processing is running on one core, leaving the other for control and housekeeping stuff. 

## Fun Facts
- Processing is sample wise 
- The I2S buffers are handled by DMA
- Latency depends on the I2S buffer size (currently a few ms)
- The audio driver is specific to the AKM AK4556 audio codec for now but will be generalized soon 
(didn't manage to get the built in ADC / DAC to run with I2S DMA)

