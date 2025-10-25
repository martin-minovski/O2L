
#include <Bela.h>
#include <cmath>
#include <libraries/OscReceiver/OscReceiver.h>
#include <signal.h>
#include "apa102.hpp"

const int gLocalPort = 7562; // port for incoming OSC messages
const short kNumLeds = 300;  // number of LEDs on the strip

bool DEBUG = false;

const int CLOCK_PIN = 30;  // BBB P9.11
const int DATA_PIN = 31;   // BBB P9.13

APA102_BELA * gLeds;
static OscReceiver oscReceiver;
static constexpr uint8_t kNumbersPerRgb = 4;        // brightness + rgb
static constexpr uint8_t kNumbersPerMonochrome = 2; // brightness + color
const int MAX_BRIGHTNESS = 31;
const int MAX_RGB = 255;

uint8_t brightness[kNumLeds];
uint8_t red[kNumLeds];
uint8_t green[kNumLeds];
uint8_t blue[kNumLeds];

static bool gStop;
// Handle Ctrl-C by requesting that the audio rendering stop
static void interrupt_handler(int var)
{
	gStop = true;
}

template <typename T>
static uint8_t clipForLed(T val)
{
	return val < 0 ? 0 : val > 255 ? 255 : val;
}

template <typename T>
static uint8_t clipForBrightness(T val)
{
	return val < 0 ? 0 : val > 31 ? 31 : val;
}

int parseMessage(oscpkt::Message msg, const char* address, void*)
{
	oscpkt::Message::ArgReader args = msg.arg();
	enum {
		kOk = 0,
		kWrongArguments,
		kUnmatchedPattern,
	} error = kOk;
	if (DEBUG) printf("Message from %s; %s\n", address, msg.addressPattern().c_str());
	// check state (non-display) messages first
	std::string baseAddr = "/leds/setRaw";
	if (msg.partialMatch(baseAddr)) {
		enum {
			kR,
			kG,
			kB,
			kRgb,
		};
		size_t color;
		if(msg.match(baseAddr + "/r"))
			color = kR;
		else if(msg.match(baseAddr + "/g"))
			color = kG;
		else if(msg.match(baseAddr + "/b"))
			color = kB;
		else if(msg.match(baseAddr + "/rgb"))
			color = kRgb;
		else
			error = kUnmatchedPattern;
		if(kOk == error)
		{
			int startPixel;
			args.popInt32(startPixel);
			float gain;
			args.popNumber(gain);
			size_t numArgs = args.nbArgRemaining();
			if(
				startPixel < 0
				|| startPixel >= kNumLeds
				|| (kRgb == color && numArgs % kNumbersPerRgb) 
				|| (kRgb != color && numArgs % kNumbersPerMonochrome) 
				|| !args.isOk()
			) error = kWrongArguments;
			else {
				int n = startPixel;
				while(args.nbArgRemaining() && n < kNumLeds)
				{
					float brightnessFloat;
					args.popNumber(brightnessFloat);
					brightness[n] = clipForBrightness(std::round(brightnessFloat * MAX_BRIGHTNESS));
					float val;
					args.popNumber(val);
					if(!args.isOk()) {
						error = kWrongArguments;
						break;
					}
					uint8_t ledValue = clipForLed(std::round(val * gain * MAX_RGB));
					
					// in monochrome mode, set the corresponding color
					// and zero out the rest
					switch (color) {
						case kR:
							red[n] = ledValue;
							green[n] = 0;
							blue[n] = 0;
							break;
						case kG:
							red[n] = 0;
							green[n] = ledValue;
							blue[n] = 0;
							break;
						case kB:
							red[n] = 0;
							green[n] = 0;
							blue[n] = ledValue;
							break;
						break;
						default:
							// in kRgb mode, set each color per each LED in order
							red[n] = ledValue;
							if (args.nbArgRemaining()) {
								args.popNumber(val);
								ledValue = clipForLed(std::round(val * gain * MAX_RGB));
								green[n] = ledValue;
							}
							if (args.nbArgRemaining()) {
								args.popNumber(val);
								ledValue = clipForLed(std::round(val * gain * MAX_RGB));
								blue[n] = ledValue;
							}
						break;
					}
					++n;
				}
				if(kOk == error) {
					for (n = 0; n < kNumLeds; n++) {
						gLeds->setPixel(n, brightness[n], red[n], green[n], blue[n]);
					}
					gLeds->show();
				}
			}
		}
	} else
		error = kUnmatchedPattern;
	int ret = 0;
	if(error)
	{
		std::string str;
		switch(error){
			case kUnmatchedPattern:
				str = "no matching pattern available\n";
				break;
			case kWrongArguments:
				str = "unexpected types and/or length\n";
				break;
			case kOk:
				str = "";
				break;
		}
		fprintf(stderr, "An error occurred with message to: %s: %s\n", msg.addressPattern().c_str(), str.c_str());
		ret = 1;
	}
	return ret;
}

int main(int argc, char* argv[])
{
	if (gpio_export(CLOCK_PIN)) printf("Warning: couldn't export clock pin\n");
	if (gpio_set_dir(CLOCK_PIN, OUTPUT_PIN)) printf("Warning: couldn't set direction of clock pin\n");
	if (gpio_export(DATA_PIN)) printf("Warning: couldn't export data pin\n");
	if (gpio_set_dir(DATA_PIN, OUTPUT_PIN)) printf("Warning: couldn't set direction of data pin\n");
	
	for (int i = 0; i < kNumLeds; i++) {
		brightness[i] = 0;
		red[i] = 0;
		green[i] = 0;
		blue[i] = 0;
	}
	
	gLeds = new APA102_BELA(kNumLeds);
	oscReceiver.setup(gLocalPort, parseMessage);

	gStop = false;
	// Set up interrupt handler to catch Control-C and SIGTERM
	signal(SIGINT, interrupt_handler);
	signal(SIGTERM, interrupt_handler);
	while(!gStop)
		usleep(200000);
	return 0;
}
