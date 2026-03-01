#include "apa102.hpp"
#include "apa102.h"
#include <iostream>
#include <cstring>
#include <algorithm>

#if ENABLE_PRU_UIO
#include "apa102_bin.h"
#endif

enum {
	GPIO0 = 0,
	GPIO1 = 1,
	GPIO2 = 2,
	GPIO3 = 3,
};

APA102_BELA::APA102_BELA(uint16_t pixel_count)
    : pru0(pru_init(0)), num_pixels(pixel_count), buffer_size(pixel_count * NUM_BYTES) {
    	
	current_buffer_num = 0;
	data_len = (uint32_t)pixel_count * NUM_BYTES;
	if (2 * buffer_size > pru0->data_ram_size-DATA_OFFSET) {
		die("Pixel data needs at least 2 * %zu, only %zu available in data ram\n", buffer_size, pru0->data_ram_size-DATA_OFFSET);
	}
	
	apa102 = (apa102_command_t *)pru0->data_ram;
	*(apa102) = apa102_command_t(data_len);

	pru_gpio(GPIO_BANK, GPIO_CLOCK_PIN, 1, 0);
	pru_gpio(GPIO_BANK, GPIO_DATA_PIN, 1, 0);

#if ENABLE_PRU_UIO
	pru_exec_code(pru0, PRUcode, sizeof(PRUcode));
#endif
#if ENABLE_PRU_RPROC
	pru_exec_file(pru0, "apa102.out");
#endif

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

	apa102->data_address = DATA_OFFSET + current_buffer_num * data_len;

	// Send the start command
	apa102->command = 1;
	
	moveToNextBuffer();
}

void APA102_BELA::setPixel(int n, uint8_t brightness, uint8_t red, uint8_t green, uint8_t blue) {
  if (n < num_pixels) {
  	uint8_t preamble = static_cast<uint8_t>(224) + brightness;
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