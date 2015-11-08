#include <Wire.h>
#include "Adafruit_MCP23008.h"

// I2C Relayboard test
// connect VDD to power 5V
// connect GND to power GND
// connect SDA to analog 4 (I2C DATA)
// connect SCL to analog 5 (I2C CLOCK)

Adafruit_MCP23008 mcp;
  
void setup()
{
  mcp.begin(0); // address = 0 (valid: 0-7)
  mcp.writeGPIO(0); // set OLAT to 0
  mcp.pinMode(0, OUTPUT); // set IODIR to 0
  mcp.pinMode(1, OUTPUT);
  mcp.pinMode(2, OUTPUT);
  mcp.pinMode(3, OUTPUT);
  mcp.pinMode(4, OUTPUT);
  mcp.pinMode(5, OUTPUT);
  mcp.pinMode(6, OUTPUT);
  mcp.pinMode(7, OUTPUT);
  
  // start serial port at 9600 bps and wait for port to open:
  Serial.begin(9600);
  while (!Serial); // wait for serial port to connect. Needed for Leonardo only
  Serial.println("I2C Relayboard test - press keys 12345678 (toggle relay) C (clear all)");
}

void loop()
{
  char input;
  int i;
  static uint8_t olat;

  if (Serial.available() > 0)
  {
    input = Serial.read();
    if ((input > '0') && (input < '9'))
    {
      Serial.print("Toggle ");
      Serial.println(input);
      olat ^= 1 << ((input - 1) & 15);
      mcp.writeGPIO(olat);
    }
    if (input == 'c')
    {
      Serial.println("Clear");
      olat = 0;
      mcp.writeGPIO(olat);
    }
    for (i = 0; i < 8; i++)
    {
      Serial.print(i+1);
      Serial.print((olat & (1 << i) ? ": ON  " : ": OFF "));
    }
    Serial.println();
  }
}

