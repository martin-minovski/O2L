#include "apa102.hpp"
#include "apa102_bin.h"
#include <iostream>
#include <cstring>
#include <algorithm>
#define DATA_OFFSET 64

APA102_BELA::APA102_BELA(uint16_t pixel_count)
    : pru0(pru_init(0)), num_pixels(pixel_count), buffer_size(pixel_count * NUM_BYTES) {
    	
	current_buffer_num = 0;
	data_len = (unsigned) pixel_count*NUM_BYTES;
	if (2 * buffer_size > pru0->data_ram_size-DATA_OFFSET) {
		die("Pixel data needs at least 2 * %zu, only %zu available in data ram\n", buffer_size, pru0->data_ram_size-DATA_OFFSET);
	}
	
	apa102 = (apa102_command_t *)pru0->data_ram;
	*(apa102) = apa102_command_t(data_len);
	
	// Initiate the PRU0 program
	pru_exec_code(pru0, PRUcode, sizeof(PRUcode));
  	
	// Watch for a done response that indicates a proper startup
	std::cout << "Waiting for initial response from PRU0... ";
	while (!apa102->response);
	std::cout << "OK" << std::endl;
};

APA102_BELA::~APA102_BELA() {
	apa102->command = 0xFF;
	pru_close(pru0);
}

void APA102_BELA::show(void) {
	// Wait for any current command to have been acknowledged
	while (apa102->command);

	*(uint32_t *)(pru0->data_ram) = DATA_OFFSET + current_buffer_num*data_len;
	
	// Send the start command
	apa102->command = 1;
	
	moveToNextBuffer();
}

void APA102_BELA::setPixel(int n, uint8_t brightness, uint8_t red, uint8_t green, uint8_t blue) {
  if (n < num_pixels) {
  	uint8_t preamble = static_cast<uint8_t>(224) + brightness; // first 3 bytes are always "111"
    uint8_t values[NUM_BYTES] = {preamble, blue, green, red};
    setRamBytes(n, values);
  }
}

void APA102_BELA::moveToNextBuffer() {
    ++current_buffer_num %= 2;
};

void APA102_BELA::setRamBytes(int n, uint8_t* values) {
	setPruRamBytes(
		(uint8_t *)pru0->data_ram, 
		DATA_OFFSET + current_buffer_num*data_len + n*NUM_BYTES, 
		values, 
		NUM_BYTES
	);
}

void APA102_BELA::setPruRamBytes(uint8_t *pru_ram, size_t offset, const uint8_t *values, size_t size) {
    memcpy(&pru_ram[offset], values, size);
}

uint32_t APA102_BELA::wait() {
  while (1) {
    uint32_t response = apa102->response;
    if (response) {
      apa102->response = 0;
      return response;
    }
  }
}

