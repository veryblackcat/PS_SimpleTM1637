
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
  dioLOW(); //TM1637_DIO_LOW;
  clkWaiting(); //TM1637_CLK_WAITING;
}

void SimpleTM1637::stop() {
	dioLOW(); //TM1637_DIO_LOW;
	clkWaiting(); //TM1637_CLK_WAITING;
	clkHIGH(); //TM1637_CLK_HIGH;
	clkWaiting(); //TM1637_CLK_WAITING;
	dioHIGH(); //TM1637_DIO_HIGH;
	clkWaiting(); //TM1637_CLK_WAITING;
}

uint8_t SimpleTM1637::sendByte(uint8_t data) {
  for(uint8_t i = 0; i < 8; i++) {
	clkLOW(); //TM1637_CLK_LOW;
	clkWaiting(); //TM1637_CLK_WAITING;

    if (data & 0x01)
      dioHIGH(); //TM1637_DIO_HIGH;
    else
      dioLOW(); //TM1637_DIO_LOW;

	clkHIGH(); //TM1637_CLK_HIGH;
	clkWaiting(); //TM1637_CLK_WAITING;
	data >>= 1;
  }

  // checking ACK
  dioHIGH(); //TM1637_DIO_HIGH;
  clkLOW(); //TM1637_CLK_LOW;
  clkWaiting(); //TM1637_CLK_WAITING;
  clkHIGH(); //TM1637_CLK_HIGH;
  // waiting for ACK
  uint32_t _timeout = micros() + TM1637_ACK_TIMEOUT;
  while (digitalRead(dio)){
	  if(_timeout >= micros()) return 1;
  }
  clkWaiting(); //TM1637_CLK_WAITING;
  
  clkLOW(); //TM1637_CLK_LOW;
  clkWaiting(); //TM1637_CLK_WAITING;

  return 0;
}


