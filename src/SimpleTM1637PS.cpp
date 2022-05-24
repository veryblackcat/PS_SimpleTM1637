
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
}

void SimpleTM1637::setBrightness(uint8_t displayBrightness) {
	if(displayBrightness > 7) displayBrightness = 7;
	brightness = displayBrightness | 0x08;
}

void SimpleTM1637::displayRAW(const uint8_t segments[], uint8_t pos, uint8_t length) {
	length -= pos;
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
  TM1637_DIO_LOW;
  delayMicroseconds(delayBUS);
}

void SimpleTM1637::stop() {
	TM1637_DIO_LOW;
	delayMicroseconds(delayBUS);
	TM1637_CLK_HIGH;
	delayMicroseconds(delayBUS);
	TM1637_DIO_HIGH;
	delayMicroseconds(delayBUS);
}

uint8_t SimpleTM1637::sendByte(uint8_t data) {
  for(uint8_t i = 0; i < 8; i++) {
	TM1637_CLK_LOW;
	delayMicroseconds(delayBUS);

    if (data & 0x01)
      TM1637_DIO_HIGH;
    else
      TM1637_DIO_LOW;

	TM1637_CLK_HIGH;
	delayMicroseconds(delayBUS);
	data >>= 1;
  }

  // checking ACK
  TM1637_DIO_HIGH;
  TM1637_CLK_LOW;
  delayMicroseconds(delayBUS);
  TM1637_CLK_HIGH;
  // waiting for ACK
  uint32_t _timeout = micros() + TM1637_ACK_TIMEOUT;
  while (digitalRead(dio)){
	  if(_timeout >= micros()) return 1;
  }
  delayMicroseconds(delayBUS);
  
  TM1637_CLK_LOW;
  delayMicroseconds(delayBUS);

  return 0;
}


