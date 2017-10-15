# Microwave Transmitter

30 - 6000 MHz 10 dBm Transmitter based on MAX2871 synthesizer. 
Controlled by Teensy 3.2. 

![MW-Tx][3d]

Some parts of the circuit design based on H. Forsten's home made VNA. The microwave transmitter consists of following blocks:

 * **Synthesizer** is based on MAX2871 chip (PLL with integrated VCO) and its loop filter. This guy is able to synthesize frequencies from 25 MHz to 6 GHz.
 * **Filter bank** that makes sure no harmonics are generated in GHz range.
 * **Amplification** block that uses programmable attenuation for the gain control and 12 dB power amplifier TRF37A75.
 * **Coupler and RSSI detectors** that measure emitted and reflected power, of course. 
 * **Teensy 3.2** that controls the synthesizer, selects filter switches, controls the attenuation and AD converts measured power from the RSSI detectors and sends all of data to PC. 
 * **Power supply** that scales 12V input to 5V, 3.6V, 3.3V and 1.8V. 


User's guide of Microwave Transmitter is available at [Github repo's wiki](https://github.com/PeraZver/Microwave-Transmitter/wiki).

Development log of the Microwave Transmitter is documented at [Hackaday.io](https://hackaday.io/project/20350-microwave-transmitter).




Pero, February 2017
(https://www.helentronica.com)

[3d]: https://github.com/PeraZver/Microwave-Transmitter/blob/master/HW/Transmitter.jpg