

#ifndef SIMPLE_TM1637_H
#define SIMPLE_TM1637_H

#include <inttypes.h>
#include <Arduino.h>

#define SEG_A   0b00000001
#define SEG_B   0b00000010
#define SEG_C   0b00000100
#define SEG_D   0b00001000
#define SEG_E   0b00010000
#define SEG_F   0b00100000
#define SEG_G   0b01000000
#define SEG_CP  0b10000000 // colon

/*
      A            A                 A            A 
     ---          ---               ---          ---
  F |   | B    F |   | B    *    F |   | B    F |   | B
     -G-          -G-       CP      -G-          -G-
  E |   | C    E |   | C    *    E |   | C    E |   | C
     ---          ---               ---          ---
      D            D                 D            D

*/

const uint8_t digit2segments[] = {
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,          // 0
  SEG_B | SEG_C,                                          // 1
  SEG_A | SEG_B | SEG_G | SEG_E | SEG_D,                  // 2
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_G,                  // 3
  SEG_F | SEG_G | SEG_B | SEG_C,                          // 4
  SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,                  // 5
  SEG_A | SEG_F | SEG_E | SEG_D | SEG_C | SEG_G,          // 6
  SEG_A | SEG_B | SEG_C,                                  // 7
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,  // 8
  SEG_G | SEG_F | SEG_A | SEG_B | SEG_C | SEG_D,          // 9
  };

#define TM1637_CLK_LOW  pinMode(clk, OUTPUT)
#define TM1637_CLK_HIGH pinMode(clk, INPUT)
#define TM1637_DIO_LOW  pinMode(dio, OUTPUT)
#define TM1637_DIO_HIGH pinMode(dio, INPUT)

#define TM1637_DELAY 100

#define TM1637_DATA_COMMAND    0x40
#define TM1637_ADDR_COMMAND    0xC0
#define TM1637_CTRL_COMMAND    0x80

class SimpleTM1637 {

public:

  SimpleTM1637(uint8_t clk, uint8_t dio);

  void setBrightness(uint8_t displayBrightness);

  void display(const uint8_t segments[]);
 
protected:

    uint8_t clk;
	uint8_t dio;
	uint8_t delay;
	
	uint8_t brightness;

	void start();
	void stop();
	uint8_t sendByte(uint8_t data);
   
private:
	
};

#endif // SIMPLE_TM1637_H
