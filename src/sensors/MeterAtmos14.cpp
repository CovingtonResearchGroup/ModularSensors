/**
 * @file MeterAtmos14.cpp
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Written By: Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 * Edited by Matthew Covington <mcoving@uark.edu> to convert to Atmos14.
 * @brief Implements the MeterAtmos14 class.
 */

#include "MeterAtmos14.h"


bool MeterAtmos14::getResults(void) {
    // Set up the float variables for receiving data
    float vp   = -9999;
    float temp = -9999;
    float baro = -9999;
    float rh   = -9999;

    // Check if this the currently active SDI-12 Object
    bool wasActive = _SDI12Internal.isActive();
    // If it wasn't active, activate it now.
    // Use begin() instead of just setActive() to ensure timer is set
    // correctly.
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

    // Wait for the first few charaters to arrive.  The response from a data
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

    // Read the Vapor pressure
    vp = _SDI12Internal.parseFloat(SKIP_NONE);
    MS_DEEP_DBG(F("    <<<"), String(vp, 10));

    // read the next '+' out of the buffer
#ifdef MS_SDI12SENSORS_DEBUG_DEEP
    MS_DEEP_DBG(F("    <<<"), static_cast<char>(_SDI12Internal.read()));
#else
    _SDI12Internal.read();
#endif

    // Now read the temperature
    temp = _SDI12Internal.parseFloat(SKIP_NONE);
    MS_DEEP_DBG(F("    <<<"), String(temp, 10));

// read the next '+' out of the buffer
#ifdef MS_SDI12SENSORS_DEBUG_DEEP
    MS_DEEP_DBG(F("    <<<"), static_cast<char>(_SDI12Internal.read()));
#else
    _SDI12Internal.read();
#endif

    // Now read the temperature
    rh = _SDI12Internal.parseFloat(SKIP_NONE);
    MS_DEEP_DBG(F("    <<<"), String(rh, 10));

// read the next '+' out of the buffer
#ifdef MS_SDI12SENSORS_DEBUG_DEEP
    MS_DEEP_DBG(F("    <<<"), static_cast<char>(_SDI12Internal.read()));
#else
    _SDI12Internal.read();
#endif

    // Now read the atmospheric pressure
    baro = _SDI12Internal.parseFloat(SKIP_NONE);
    MS_DEEP_DBG(F("    <<<"), String(baro, 10));


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

    MS_DBG(F("Vapor pressure:"), vp);
    MS_DBG(F("Temperature Value:"), temp);
    MS_DBG(F("Relative humidity:"), rh);
    MS_DBG(F("Baro pressure:"), baro);

    // range check on temp; range is - 40°C to + 80°C
    if (temp < -50 || temp > 90) {
        temp = -9999;
        MS_DBG(F("WARNING:  temperature results out of range (-50-90)!"));
    }

    verifyAndAddMeasurementResult(ATMOS14_VP_VAR_NUM, vp);
    verifyAndAddMeasurementResult(ATMOS14_TEMP_VAR_NUM, temp);
    verifyAndAddMeasurementResult(ATMOS14_RH_VAR_NUM, rh);
    verifyAndAddMeasurementResult(ATMOS14_PRES_VAR_NUM, baro);

    return temp != -9999;
}
