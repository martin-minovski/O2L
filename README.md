# OSC to APA102 Bridge for Bela Gem / PocketBeagle2

An OSC to APA102 addressable LED bridge for Linux, built using Bela's `OscReceiver` and `UdpServer` classes. 

### Hardware Setup
This version uses the PRU on the Bela Gem / PocketBeagle2 SoC to bit-bang the 2-wire signal (clock and data) required by APA102 strips. 

By default, it is configured for the following pins:
* **Clock Pin:** Linux GPIO 49 *(Mapped to PRU Bank 1, Bit 17)*
* **Data Pin:** Linux GPIO 50 *(Mapped to PRU Bank 1, Bit 18)*

> This setup works fine using the board's native 3.3V logic **only** if you keep the brightness settings very low. If you plan to run the LEDs at higher brightness levels or use a long strip, you will need a logic level shifter (to step up to 5V) and a dedicated external power supply to avoid drawing too much current and damaging your board.

### OSC Message Format
Every OSC message sequence must start with these two numbers:
1.  `startPixel` (Integer): Must be less than `kNumLeds` (defined in `main.cpp`).
2.  `gain` (Float): Global brightness scaling between `0.0` and `1.0`.

The remaining numbers in the payload depend on which OSC endpoint you are sending to:

* `/leds/setRaw/rgb` 
    Expects `4 * kNumLeds` floating-point numbers (between 0.0 and 1.0). The sequence for each LED is: `[brightness, red, green, blue]`.
* `/leds/setRaw/r` 
    Monochrome mode. Expects `2 * kNumLeds` floating-point numbers (between 0.0 and 1.0). The sequence for each LED is: `[brightness, red]`.
* `/leds/setRaw/g` 
    Ditto, but for green: `[brightness, green]`.
* `/leds/setRaw/b` 
    Ditto, but for blue: `[brightness, blue]`.
