#ifndef _apa102_hpp_
#define _apa102_hpp_
#include "pru.h"

struct apa102_command_t {
  // Offset into PRU data RAM where pixel data begins.
  // Must be uint32_t (not uintptr_t) to match the 32-bit PRU's view of this struct.
  volatile uint32_t data_address;

  // Total bytes of LED frame data
  volatile uint32_t num_bytes;

  // Write 1 to start, 0xFF to abort. Cleared by PRU when acknowledged.
  volatile uint32_t command;

  // Non-zero response written by PRU when done
  volatile uint32_t response;
  apa102_command_t(uint32_t _num_bytes)
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
  static const int DATA_OFFSET = 64;

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