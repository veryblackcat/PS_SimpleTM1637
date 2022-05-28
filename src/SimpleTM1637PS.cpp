
#include <inttypes.h>
#include <Arduino.h>
#include <SimpleTM1637PS.h>

SimpleTM1637::SimpleTM1637(uint8_t clk, uint8_t dio) {
	SimpleTM1637::clk = clk;
	SimpleTM1637::dio = dio;
	
	delayBUS = TM1637_DELAY_BUS;
	brightness = 2;

    pinMode(clk, INPUT);
    pinMode(dio,INPUT);
	digitalWrite(clk, LOW);
	digitalWrite(dio, LOW);
	
	displayPoints = 0;
}

void SimpleTM1637::setBrightness(uint8_t displayBrightness) {
	if(displayBrightness > 7) displayBrightness = 7;
	brightness = displayBrightness | 0x08;
}

void SimpleTM1637::number2buffer(uint8_t buffer[], int16_t number, uint8_t pos, uint8_t length, bool leadingZeros, uint8_t base){
	if(number < 0){
		number = -number;
		buffer[pos] = SEG_G; // minus sign
		pos++; 
		length--;
	}
	uint8_t _endPos = length + pos - 1;
	for(int8_t i = _endPos; i >= pos ; i--){
		if(leadingZeros){ 
			buffer[i] = digit2segments[number % base];
		} else {
			if(number == 0){ 
				if(i == _endPos) buffer[i] = digit2segments[0];
				else buffer[i] = 0;
			}
			else {
				buffer[i] = digit2segments[number % base];
			}
		}
		number /= base;
	}
}

/*
void SimpleTM1637::writeDEC(int16_t number, uint8_t pos, uint8_t length, bool leadingZeros){
	if(number < 0){
		number = -number;
		displayBuffer[pos] = SEG_G; // minus sign
		pos++; 
		length--;
	}
	uint8_t _endPos = length + pos - 1;
	for(int8_t i = _endPos; i >= pos ; i--){
		if(leadingZeros){ 
			displayBuffer[i] = digit2segments[number % 10];
		} else {
			if(number == 0){ 
				if(i == _endPos) displayBuffer[i] = digit2segments[0];
				else displayBuffer[i] = 0;
			}
			else {
				displayBuffer[i] = digit2segments[number % 10];
			}
		}
		number /= 10;
    }
}
*/

uint8_t char2segments(char CharIn){
	switch(charAt(i)){
		'0' : return();
		'1' : return();
		'2' : return();
		'3' : return();
		'4' : return();
		'5' : return();
		'6' : return();
		'7' : return();
		'8' : return();
		'9' : return();
		'A' : return();
		'b' : return();
		'c' : return();
		'C' : return();
		'd' : return();
		'E' : return();
		'F' : return();
		'G' : return();
		'h' : return();
		'H' : return();
		'I' : return();
		'J' : return();
		'L' : return();
		'n' : return();
		'o' : return();
		'O' : return();
		'P' : return();
		'S' : return();
		't' : return();
		'u' : return();
		'U' : return();
		'Y' : return();
		' ' : return(0);
		'-' : return(SEG_G);
		}
}

void string2buffer(uint8_t buffer[], String txt=0, uint8_t pos=0, uint8_t length=4, bool leadingZeros=false, uint8_t base=10){
	for(uint8_t i=0; i<length; i++){

	}
}

void SimpleTM1637::colon(bool colonON){
	if(colonON) displayPoints |= SEG_CP;
	else displayPoints &= ~SEG_CP;
}

void SimpleTM1637::display(){
	if(displayPoints & SEG_CP) displayBuffer[1] |= SEG_CP;
	else displayBuffer[1] &= ~SEG_CP;
	displayRAW(displayBuffer);
}

void SimpleTM1637::clear(uint8_t pos, uint8_t length){
	for (uint8_t i = pos; i < length; i++) displayBuffer[i] = 0;
}

void SimpleTM1637::displayRAW(const uint8_t segments[], uint8_t pos, uint8_t length) {
	if(pos > 3) pos = 3;
	  //if(length > 4) length = 4;
	if(length > 4 - pos) length = 4 - pos;
	// Data command setting
	// Write data to display register, Automatic address adding, Normal mode
	start();
	sendByte(TM1637_DATA_COMMAND);
	stop();

	// Address command setting GRID1 (COH) - first digit (left)
	start();
	sendByte(TM1637_ADDR_COMMAND + pos);

	// Send byte
	for (uint8_t i=0; i < length; i++) sendByte(segments[i]);
	stop();

	// Display and control command setting - brightness, on/off
	start();
	sendByte(TM1637_CTRL_COMMAND | brightness);
	stop();
}

void SimpleTM1637::start() {
  dioLOW();
  clkWaiting();
}

void SimpleTM1637::stop() {
	dioLOW();
	clkWaiting();
	clkHIGH();
	clkWaiting();
	dioHIGH();
	clkWaiting();
}

uint8_t SimpleTM1637::sendByte(uint8_t data) {
  for(uint8_t i = 0; i < 8; i++) {
	clkLOW();
	clkWaiting();

    if (data & 0x01)
      dioHIGH();
    else
      dioLOW();

	clkHIGH();
	clkWaiting();
	data >>= 1;
  }

  // checking ACK
  dioHIGH();
  clkLOW();
  clkWaiting();
  clkHIGH();
  // waiting for ACK
  uint32_t _timeout = micros() + TM1637_ACK_TIMEOUT;
  while (digitalRead(dio)){
	  if(_timeout >= micros()) return 1;
  }
  clkWaiting();
  
  clkLOW();
  clkWaiting();

  return 0;
}


