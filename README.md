An OSC to APA102 addressable LED bridge for Linux using Bela's OscReceiver and UdpServer classes.
Uses the second PRU of the BBB to generate I2C signal via P9.11 (clock) and P9.13 (data). May also work on Bela Mini with adjusted CLOCK_PIN and DATA_PIN.
Works fine on 3.3V in low brightness setting. Otherwise, a logic level shifter and an external power supply may be required.
Message format:

The first 2 numbers in the sequence are always startPixel (integer, less than kNumLeds defined in main.cpp) and gain (floating point, between 0 and 1).
The remaining numbers depend on the OSC endpoint:
/leds/setRaw/rgb - 4*kNumLeds floating point numbers between 0 and 1: brightness, red, green and blue subsequence for each LED.
/leds/setRaw/r - 2*kNumLeds floating point numbers between 0 and 1: brightness and color value in monochrome mode for each LED.
/leds/setRaw/g - ditto
/leds/setRaw/b - ditto
