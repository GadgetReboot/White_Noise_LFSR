// Generates digital pseudo "white" noise
// Uses a 31 bit maximum length linear feedback shift register
//   feedback taps:  3, 31
//   can also try:   6, 31   7, 31   13, 31
//
// LFSR cycles through (2^31 -1) states before repeating
// All possible register values except all 0's are generated
// (all zeros would lock up the lfsr and never change state)
//
// Output noise pin:  GPIO 4,  ATTiny85 pin 3
//
// Requirements for Arduino IDE:
// Install ATTiny85 board file support https://github.com/damellis/attiny
//
// Tested with Arduino IDE 1.8.13
// Arduino IDE settings:   Board/Processor:  ATTiny85
//                         Clock:            Internal 8 or 16 MHz (burn bootloader to ensure fuses set)
//                         Programmer:       Arduino as ISP (Using an Uno to program ATTiny85 over SPI)
//                                           General usage: https://youtu.be/7Rz9A9VbDx4?t=300
// Gadget Reboot
// https://www.youtube.com/c/GadgetReboot/videos

const int noisePin = 4;  // noise output pin

uint32_t lfsr = 0xACE1FFFF;  // linear feedback shift register with non-zero seed number

void setup() {

  pinMode(noisePin, OUTPUT); // noise pin

}

void loop() {

  // 31 bit LFSR with taps at 3, 31 where position 1 is at MSB and 31 at LSB position
  // input to LFSR is position 1 and output is taken from position 31
  //    (bit 1)  _ _ x _   _ _ _ _   _ _ _ _   _ _ _ _   _ _ _ _   _ _ _ _   _ _ _ _   _ _ x   (bit 31)
  //               tap 3                                                               tap 31

  // using 32 bit lfsr register (uint32_t) where MSB is 31 and LSB is 0
  //   LFSR tap 3 is found at bit position 29 in register
  //   LFSR tap 31 is found at bit position 1 in register (bit 0 is not used)
  //   Output is taken at bit position 1 for 31 stage LFSR
  //   (bit 31)  _ _ x _   _ _ _ _   _ _ _ _   _ _ _ _   _ _ _ _   _ _ _ _   _ _ _ _   _ _ x _  (bit 0)
  //               tap 3                                                               tap 31

  uint32_t reg = ((lfsr >> 1) ^ (lfsr >> 29)) & 1;  // perform xor of tap points with result in reg LSB (bit 0)
  lfsr = (lfsr >> 1) | (reg << 31);                 // perform LFSR shift for this clock cycle and feed output back to input

  digitalWrite(noisePin, bitRead(lfsr, 1));         // send the lfsr output to the noise pin
  //delayMicroseconds(3);  // optionally experiment with slowing down the lfsr "clock"
}
