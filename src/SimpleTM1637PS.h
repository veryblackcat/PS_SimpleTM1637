

#ifndef SIMPLE_TM1637PS_H
#define SIMPLE_TM1637PS_H

#include <inttypes.h>
#include <Arduino.h>

//                C 
//                PGFEDCBA
#define SEG_A   0b00000001
#define SEG_B   0b00000010
#define SEG_C   0b00000100
#define SEG_D   0b00001000
#define SEG_E   0b00010000
#define SEG_F   0b00100000
#define SEG_G   0b01000000
#define SEG_CP  0b10000000 // colon

/*
   pos = 0      pos = 1           pos = 2      pos = 3

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
  SEG_E | SEG_F | SEG_A | SEG_B | SEG_C | SEG_G,          // A
  SEG_F | SEG_E | SEG_D | SEG_C | SEG_G,                  // b
  SEG_A | SEG_F | SEG_E | SEG_D,                          // C
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,                  // d
  SEG_A | SEG_F | SEG_E | SEG_D | SEG_G,                  // E
  SEG_A | SEG_F | SEG_E | SEG_G,                          // F
  };

#define TM1637_DELAY_BUS   100
#define TM1637_ACK_TIMEOUT 200

#define TM1637_DATA_COMMAND 0x40
#define TM1637_ADDR_COMMAND 0xC0
#define TM1637_CTRL_COMMAND 0x80

class SimpleTM1637 {

public:

  SimpleTM1637(uint8_t clk, uint8_t dio);

  void setBrightness(uint8_t displayBrightness);

  void number2buffer(uint8_t buffer[], int16_t number=0, uint8_t pos=0, uint8_t length=4, bool leadingZeros=false, uint8_t base=10);
  //void writeDEC(int16_t number=0, uint8_t pos=0, uint8_t length=4, bool leadingZeros=false);
  void colon(bool colonON=false);
  void clear(uint8_t pos=0, uint8_t length=4);
  void display();
  void displayRAW(const uint8_t segments[], uint8_t pos=0, uint8_t length=4);
  
  // inline
  void writeDEC(int16_t number=0, uint8_t pos=0, uint8_t length=4, bool leadingZeros=false) {
	  number2buffer(displayBuffer, number, pos, length, leadingZeros);
  }
 
protected:

    uint8_t clk;
	uint8_t dio;
	uint8_t delayBUS;
	
	uint8_t brightness;
	uint8_t displayBuffer[4];
	uint8_t displayPoints;

	void start();
	void stop();
	uint8_t sendByte(uint8_t data);
	
	// inline
	void clkLOW()  {pinMode(clk, OUTPUT);}
	void clkHIGH() {pinMode(clk, INPUT); }
	void dioLOW()  {pinMode(dio, OUTPUT);}
	void dioHIGH() {pinMode(dio, INPUT); }
	void clkWaiting() {delayMicroseconds(delayBUS);}
   
private:
	
};

#endif // SIMPLE_TM1637PS_H
