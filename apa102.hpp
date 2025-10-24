
#ifndef _apa102_hpp_
#define _apa102_hpp_
#include "pru.h"

struct apa102_command_t {
  // DDR shared with the PRU
  uintptr_t data_address;

  // Length in bytes of each LED frame
  unsigned num_bytes;

  // Write 1 to start, 0xFF to abort. will be cleared when started
  volatile unsigned command;

  // This will have a non-zero response written when done
  volatile unsigned response;
  apa102_command_t(unsigned _num_bytes)
      : data_address(0), num_bytes(_num_bytes), command(0), response(0) {};

} __attribute__((__packed__));

class APA102_BELA {
  pru_t *pru0;
  uint32_t num_pixels;
  uint8_t current_buffer_num;
  apa102_command_t *apa102;
  size_t buffer_size;
  uint32_t data_len;
  static const int NUM_BYTES = 4;
  

public:
  APA102_BELA(uint16_t pixel_count);
  ~APA102_BELA();
  void show(void);
  void setPixel(int n, uint8_t brightness, uint8_t red, uint8_t green, uint8_t blue);
  void setRamBytes(int n, uint8_t* values);
  uint32_t wait();
  
private:
  void moveToNextBuffer();
  void setPruRamBytes(uint8_t *pru_ram, size_t offset, const uint8_t *values, size_t size);  
};

#endif
