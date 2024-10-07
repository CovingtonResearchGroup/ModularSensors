/**
 * @file MeterAtmos22.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Adapted by: Matthew Covington
 * @brief Implements the MeterAtmos22 class.
 */

#include "MeterAtmos22.h"

static float maxWindSpeed = 30.0;
static float maxWindGust  = 30.0;


bool MeterAtmos22::getResults(void) {
    // Set up the float variables for receiving data
    float windSpeed   = -9999;
    float windDir     = -9999;
    float windGust    = -9999;
    float temperature = -9999;


    // Check if this is the currently active SDI-12 Object
    bool wasActive = _SDI12Internal.isActive();
    // If it wasn't active, activate it now.
    // Use begin() instead of just setActive() to ensure timer is set correctly.
    if (!wasActive) _SDI12Internal.begin();
    // Empty the buffer
    _SDI12Internal.clearBuffer();

    MS_DBG(getSensorNameAndLocation(), F("is reporting:"));
    String getDataCommand = "";
    getDataCommand += _SDI12address;
    // SDI-12 command to get data [address][D][dataOption][!]
    getDataCommand += "D0!";
    _SDI12Internal.sendCommand(getDataCommand, _extraWakeTime);
    delay(30);  // It just needs this little delay
    MS_DEEP_DBG(F("    >>>"), getDataCommand);

    // Wait for the first few characters to arrive. The response from a data
    // request should always have more than three characters
    uint32_t start = millis();
    while (_SDI12Internal.available() < 3 && (millis() - start) < 1500) {
        // wait
    }
    // read the returned address to remove it from the buffer
    auto returnedAddress = static_cast<char>(_SDI12Internal.read());
    // print out a warning if the address doesn't match up
    if (returnedAddress != _SDI12address) {
        MS_DBG(F("Warning, expecting data from"), _SDI12address,
               F("but got data from"), returnedAddress);
    }
    // Start printing out the returned data
    MS_DEEP_DBG(F("    <<<"), returnedAddress);

    // read the '+' out of the buffer, and print it if we're debugging
#ifdef MS_SDI12SENSORS_DEBUG_DEEP
    MS_DEEP_DBG(F("    <<<"), static_cast<char>(_SDI12Internal.read()));
#else
    _SDI12Internal.read();
#endif

    // Read the Wind Speed
    windSpeed = _SDI12Internal.parseFloat(SKIP_NONE);
    MS_DEEP_DBG(F("    <<< Wind Speed:"), String(windSpeed, 10));

    // read the next '+' out of the buffer
#ifdef MS_SDI12SENSORS_DEBUG_DEEP
    MS_DEEP_DBG(F("    <<<"), static_cast<char>(_SDI12Internal.read()));
#else
    _SDI12Internal.read();
#endif

    // Now read the Wind Direction
    windDir = _SDI12Internal.parseFloat(SKIP_NONE);
    MS_DEEP_DBG(F("    <<< Wind Direction:"), String(windDir, 10));

    // read the next '+' out of the buffer
#ifdef MS_SDI12SENSORS_DEBUG_DEEP
    MS_DEEP_DBG(F("    <<<"), static_cast<char>(_SDI12Internal.read()));
#else
    _SDI12Internal.read();
#endif

    // Now read the Wind Gust Speed
    windGust = _SDI12Internal.parseFloat(SKIP_NONE);
    MS_DEEP_DBG(F("    <<< Wind Gust Speed:"), String(windGust, 10));

    // read the next '+' out of the buffer
#ifdef MS_SDI12SENSORS_DEBUG_DEEP
    MS_DEEP_DBG(F("    <<<"), static_cast<char>(_SDI12Internal.read()));
#else
    _SDI12Internal.read();
#endif

    while (_SDI12Internal.available()) {
#ifdef MS_SDI12SENSORS_DEBUG_DEEP
        MS_DEEP_DBG(F("    <<<"), static_cast<char>(_SDI12Internal.read()));
#else
        _SDI12Internal.read();
#endif
    }

    getDataCommand = "";
    getDataCommand += _SDI12address;
    // SDI-12 command to get data [address][D][dataOption][!]
    getDataCommand += "D1!";
    _SDI12Internal.sendCommand(getDataCommand, _extraWakeTime);
    delay(30);  // It just needs this little delay
    MS_DEEP_DBG(F("    >>>"), getDataCommand);

    // Wait for the first few characters to arrive. The response from a data
    // request should always have more than three characters
    uint32_t start = millis();
    while (_SDI12Internal.available() < 3 && (millis() - start) < 1500) {
        // wait
    }
    // read the returned address to remove it from the buffer
    auto returnedAddress = static_cast<char>(_SDI12Internal.read());
    // print out a warning if the address doesn't match up
    if (returnedAddress != _SDI12address) {
        MS_DBG(F("Warning, expecting data from"), _SDI12address,
               F("but got data from"), returnedAddress);
    }
    // Start printing out the returned data
    MS_DEEP_DBG(F("    <<<"), returnedAddress);

    // read the '+' out of the buffer, and print it if we're debugging
#ifdef MS_SDI12SENSORS_DEBUG_DEEP
    MS_DEEP_DBG(F("    <<<"), static_cast<char>(_SDI12Internal.read()));
#else
    _SDI12Internal.read();
#endif

    // Read the Air Temp
    temperature = _SDI12Internal.parseFloat(SKIP_NONE);
    MS_DEEP_DBG(F("    <<< Air Temperature:"), String(temperature, 10));

    // read and dump anything else
    while (_SDI12Internal.available()) {
#ifdef MS_SDI12SENSORS_DEBUG_DEEP
        MS_DEEP_DBG(F("    <<<"), static_cast<char>(_SDI12Internal.read()));
#else
        _SDI12Internal.read();
#endif
    }

    // Empty the buffer again
    _SDI12Internal.clearBuffer();

    // De-activate the SDI-12 Object
    // Use end() instead of just forceHold to un-set the timers
    if (!wasActive) _SDI12Internal.end();

    // Output the sensor readings
    MS_DBG(F("Wind Speed:"), windSpeed);
    MS_DBG(F("Wind Direction:"), windDir);
    MS_DBG(F("Wind Gust Speed:"), windGust);
    MS_DBG(F("Temperature:"), temperature);

    if (windSpeed < 0 || windSpeed > maxWindSpeed) {
        windSpeed = -9999;
        MS_DBG(F("WARNING: wind speed results out of range!"));
    }
    if (windDir < 0 || windDir > 360) {
        windDir = -9999;
        MS_DBG(F("WARNING: wind direction results out of range!"));
    }
    if (windGust < 0 || windGust > maxWindGust) {
        windGust = -9999;
        MS_DBG(F("WARNING: wind gust results out of range!"));
    }

    // Check for valid temperature range (-40 to 80°C)
    if (temperature < -40 || temperature > 80) {
        temperature = -9999;
        MS_DBG(F("WARNING:  temperature results out of range (-40 to 80°C)!"));
    }

    // Save the sensor readings as measurement results
    verifyAndAddMeasurementResult(ATMOS22_WS_VAR_NUM, windSpeed);
    verifyAndAddMeasurementResult(ATMOS22_WD_VAR_NUM, windDir);
    verifyAndAddMeasurementResult(ATMOS22_WG_VAR_NUM, windGust);
    verifyAndAddMeasurementResult(ATMOS22_TEMP_VAR_NUM, temperature);

    return temperature != -9999;
}
