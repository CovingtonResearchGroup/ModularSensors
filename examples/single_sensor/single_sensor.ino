/*****************************************************************************
single_sensor.ino
Written By:  Sara Damiano (sdamiano@stroudcenter.org)
Development Environment: PlatformIO 3.2.1
Hardware Platform: EnviroDIY Mayfly Arduino Datalogger
Software License: BSD-3.
  Copyright (c) 2017, Stroud Water Research Center (SWRC)
  and the EnviroDIY Development Team

This sketch is an example of getting data from a single sensor, in this case, a
MaxBotix Ultrasonic Range Finder

DISCLAIMER:
THIS CODE IS PROVIDED "AS IS" - NO WARRANTY IS GIVEN.
*****************************************************************************/

// ---------------------------------------------------------------------------
// Include the base required libraries
// ---------------------------------------------------------------------------
#include <Arduino.h>
#include <SensorBase.h>
#include <MaximDS18.h>

// ---------------------------------------------------------------------------
// Set up the sensor object
// ---------------------------------------------------------------------------

// DS18B20: pin settings
DeviceAddress DS18B20Address = {0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };
const int DS18B20Data = 10;     // data  pin
const int DS18B20Power = 22;   // power pin

// Create a new instance of the sonar_range object;
MaximDS18_Temp temp(DS18B20Address, DS18B20Power, DS18B20Data);

// ---------------------------------------------------------------------------
// Board setup info
// ---------------------------------------------------------------------------
const int SERIAL_BAUD = 9600;  // Serial port BAUD rate
const int GREEN_LED = 8;  // Pin for the green LED
const int RED_LED = 9;  // Pin for the red LED

// Flashes to Mayfly's LED's
void greenred4flash()
{
  for (int i = 1; i <= 4; i++) {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    delay(50);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    delay(50);
  }
  digitalWrite(RED_LED, LOW);
}

// ---------------------------------------------------------------------------
// Main setup function
// ---------------------------------------------------------------------------
void setup()
{
    // Start the primary serial connection
    Serial.begin(SERIAL_BAUD);

    // Set up pins for the LED's
    pinMode(GREEN_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    // Blink the LEDs to show the board is on and starting up
    greenred4flash();

    // Print a start-up note to the first serial port
    Serial.println(F("Single Sensor Example - DS18B20"));

    // Set up the sensor
    temp.setup();

}


// ---------------------------------------------------------------------------
// Main loop function
// ---------------------------------------------------------------------------
void loop()
{
    // Power the sensor
    digitalWrite(DS18B20Power, HIGH);

    // Turn on the LED to show we're taking a reading
    digitalWrite(GREEN_LED, HIGH);

    // Update the sensor value
    temp.update();

    // Print the sonar result
    Serial.print("Data recieved from sonar: ");
    Serial.println(temp.getValueString());

    // Turn of sensor power
    digitalWrite(DS18B20Power, LOW);

    // Turn off the LED to show we're done with the reading
    digitalWrite(GREEN_LED, LOW);

    // Wait for the next reading
    delay(5000);
}
