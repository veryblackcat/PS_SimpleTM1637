
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

uint8_t char2segments(char charIn){
	switch(charIn){
		case '0' : return(SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F);
		case '1' : return(SEG_B | SEG_C);
		case '2' : return(SEG_A | SEG_B | SEG_G | SEG_E | SEG_D);
		case '3' : return(SEG_A | SEG_B | SEG_C | SEG_D | SEG_G);
		case '4' : return(SEG_F | SEG_G | SEG_B | SEG_C);
		case '5' : return(SEG_A | SEG_F | SEG_G | SEG_C | SEG_D);
		case '6' : return(SEG_A | SEG_F | SEG_E | SEG_D | SEG_C | SEG_G);
		case '7' : return(SEG_A | SEG_B | SEG_C);
		case '8' : return(SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G);
		case '9' : return(SEG_G | SEG_F | SEG_A | SEG_B | SEG_C | SEG_D);
		case 'A' : return(SEG_E | SEG_F | SEG_A | SEG_B | SEG_C | SEG_G);
		case 'b' : return(SEG_F | SEG_E | SEG_D | SEG_C | SEG_G);
		case 'c' : return(0);
		case 'C' : return(SEG_A | SEG_F | SEG_E | SEG_D);
		case 'd' : return(SEG_B | SEG_C | SEG_D | SEG_E | SEG_G);
		case 'E' : return(SEG_A | SEG_F | SEG_E | SEG_D | SEG_G);
		case 'F' : return(SEG_A | SEG_F | SEG_E | SEG_G);
		case 'G' : return(0);
		case 'h' : return(0);
		case 'H' : return(0);
		case 'I' : return(0);
		case 'J' : return(0);
		case 'L' : return(0);
		case 'n' : return(0);
		case 'o' : return(0);
		case 'O' : return(0);
		case 'P' : return(0);
		case 'r' : return(0);
		case 'S' : return(0);
		case 't' : return(0);
		case 'u' : return(0);
		case 'U' : return(0);
		case 'Y' : return(0);
		case ' ' : return(0);
		case '-' : return(SEG_G);
		}
}

void string2buffer(uint8_t buffer[], String txt=0, uint8_t pos=0, uint8_t length=4){
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


