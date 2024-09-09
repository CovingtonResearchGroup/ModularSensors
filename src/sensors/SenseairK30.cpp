/**
 * @file SenseairK30.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Matthew D. Covington <mcoving@uark.edu>
 *
 * @brief Implements the SenseairK30 class.
 */

#include "SenseairK30.h"

uint8_t read_CO2[]     = {0xFE, 0X44, 0X00, 0X08, 0X02, 0X9F, 0X25};
int     responseLength = 7;


SenseairK30::SenseairK30(Stream* stream, int8_t powerPin, int8_t triggerPin,
                         uint8_t measurementsToAverage, float valMultiplier = 1)
    : Sensor("SenseairK30", K30_NUM_VARIABLES, K30_WARM_UP_TIME_MS,
             K30_STABILIZATION_TIME_MS, K30_MEASUREMENT_TIME_MS, powerPin, -1,
             measurementsToAverage),
      _triggerPin(triggerPin),
      _stream(stream),
      _valMultiplier(valMultiplier) {}
SenseairK30::SenseairK30(Stream& stream, int8_t powerPin, int8_t triggerPin,
                         uint8_t measurementsToAverage, float valMultiplier = 1)
    : Sensor("SenseairK30", K30_NUM_VARIABLES, K30_WARM_UP_TIME_MS,
             K30_STABILIZATION_TIME_MS, K30_MEASUREMENT_TIME_MS, powerPin, -1,
             measurementsToAverage, K30_INC_CALC_VARIABLES),
      _triggerPin(triggerPin),
      _stream(&stream),
      _valMultiplier(valMultiplier) {}
// Destructor
SenseairK30::~SenseairK30() {}


// unfortunately, we really cannot know where the stream is attached.
String SenseairK30::getSensorLocation(void) {
    // attach the trigger pin to the stream number
    String loc = "co2Stream_trigger" + String(_triggerPin);
    return loc;
}


bool SenseairK30::setup(void) {
    // Set up the trigger, if applicable
    if (_triggerPin >= 0) {
        pinMode(_triggerPin, OUTPUT);
        digitalWrite(_triggerPin, LOW);
    }

    // Set the stream timeout
    // Even the slowest sensors should respond at a rate of 6Hz (166ms).
    _stream->setTimeout(200);

    return Sensor::setup();  // this will set pin modes and the setup status bit
}


// Parsing and tossing the header lines in the wake-up
bool SenseairK30::wake(void) {
    // Sensor::wake() checks if the power pin is on and sets the wake timestamp
    // and status bits.  If it returns false, there's no reason to go on.
    if (!Sensor::wake()) return false;

    return true;
}


bool SenseairK30::addSingleMeasurementResult(void) {
    // Initialize values
    bool    success = false;
    int16_t result  = -9999;

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));

    // Clear anything out of the stream buffer
    auto junkChars = static_cast<uint8_t>(_stream->available());
    if (junkChars) {
        MS_DBG(F("Dumping"), junkChars,
               F("characters from MaxBotix stream buffer:"));
        for (uint8_t i = 0; i < junkChars; i++) {
#ifdef MS_SENSEAIRK30_DEBUG
            DEBUGGING_SERIAL_OUTPUT.print(_stream->read());
#else
            _stream->read();
#endif
        }
#ifdef MS_SENSEAIRK30_DEBUG
        DEBUGGING_SERIAL_OUTPUT.println();
#endif
    }

    uint8_t co2Attempts = 0;
    while (success == false && co2Attempts < 25) {
        int timeout = 0;
        MS_DBG(F("Starting read from K30..."));
        int write_size;
        while (!_stream->available()) {
            write_size = _stream->write(read_CO2, responseLength);
            //            MS_DBG(F("write size="), write_size);
            timeout++;
            //            MS_DBG(F("timeout="), timeout);
            delay(50);
            if (timeout > 50) { break; }
        }

        timeout = 0;
        MS_DBG(F("Waiting for response..."));
        while (_stream->available() < responseLength) {
            MS_DBG(F("Stream is available."));
            timeout++;
            if (timeout > 10) {
                while (_stream->available()) { _stream->read(); }

                break;
            }
            delay(50);
        }

        if (_stream->available() >= responseLength) {
            uint8_t packet[responseLength];
            MS_DBG(F("Reading packet..."));
            _stream->readBytes(packet, responseLength);

            // Calculate value
            int high = packet[3];
            int low  = packet[4];
            result   = (high * 256 + low) * _valMultiplier;
            MS_DBG(F("  CO2: "), result);
        } else {
            MS_DBG(F("Got response of wrong length!"));
            result = -9999;
        }
        co2Attempts++;


        if (result <= 0) {
            MS_DBG(F("  Bad or Suspicious Result, Retry Attempt #"),
                   co2Attempts);
            result = -9999;
        } else {
            MS_DBG(F("  Good result found"));
            success = true;
        }
    }
    /**}
} else {
    MS_DBG(getSensorNameAndLocation(), F("is not currently measuring!"));
}*/

    verifyAndAddMeasurementResult(K30_VAR_NUM, result);

    // Unset the time stamp for the beginning of this measurement
    _millisMeasurementRequested = 0;
    // Unset the status bits for a measurement request (bits 5 & 6)
    //_sensorStatus &= 0b10011111;

    // Return values shows if we got a not-obviously-bad reading
    return success;
}
