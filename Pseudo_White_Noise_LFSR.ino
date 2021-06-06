// Generates digital pseudo "white" noise using a linear feedback shift register
// Output noise pin:  GPIO 4,  ATTiny85 pin 3
// Based on the "NoiseChip" project:  https://github.com/aleh/NoiseChip
//
// Requirements for Arduino IDE:   
// ATTiny85 board file support https://github.com/damellis/attiny
//
// A21 library for fast GPIO pin writes: https://github.com/aleh/a21
//                                       -Click the Code download button on page to download a zip and
//                                        install the zip as an Arduino library, then this sketch should
//                                        compile
// 
// Arduino IDE settings:   Board/Processor:  ATTiny85         
//                         Clock:            Internal 1 MHz
//                         Programmer:       Arduino as ISP (Using an Uno to program ATTiny85 over SPI)
//                                           General usage: https://youtu.be/7Rz9A9VbDx4?t=300

#include <a21.hpp>

using namespace a21;

class LFSR {
private:
  uint16_t reg;
public:

//  xor tap points for 16 bit LFSR:  16 15 13 4  
//  feedback polynomial x^16 + x^15 + x^13 + x^4 + 1
  LFSR(uint16_t seed) : reg(seed) {}  
  bool next() {
    uint8_t b = ((reg >> 0) ^ (reg >> 1) ^ (reg >> 3) ^ (reg >> 12)) & 1;  // tap points 16 15 13 4 correspond to bits 0 1 3 12 in 16 bit integer
    reg = (reg >> 1) | (b << 15);
    return b;
  }
};

LFSR lsfr1(0xA1e);            // start linear feedback shift register with a non zero seed number

typedef FastPin<4> NoisePin;  // pin 4 is noise output, using fast pin writes

inline void setup() {
  NoisePin::setOutput(); 
}

inline void loop() {
  NoisePin::write(lsfr1.next());  // step through the LFSR sequence and send the output bit to the noise pin
}

// Don't want Arduino timer routines since we are using the timer already. 
// Luckily we can override main().
int main(void) {

  setup();
  sei();
  
  while (true) {
    loop();
  }
}
