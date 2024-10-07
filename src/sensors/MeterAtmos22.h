/**
 * @file MeterAtmos22.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Written By: [Your Name]
 * @author Adapted By: Matt Covington
 * @brief Contains the MeterAtmos22 sensor subclass and the variable subclasses
 * MeterAtmos22_WindSpeed, MeterAtmos22_WindDirection, and MeterAtmos22_Temp.
 *
 * These are for the Meter Atmos 22 ultrasonic anemometer.
 *
 * This depends on the EnviroDIY SDI-12 library and the SDI12Sensors super
 * class.
 */
/* clang-format off */
/**
 * @defgroup sensor_atmos22 Meter Atmos 22
 * Classes for the Meter Atmos 22 ultrasonic anemometer.
 *
 * @ingroup sdi12_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_atmos22_intro Introduction
 *
 * The Meter Atmos 22 is a compact ultrasonic anemometer designed for measuring 
 * wind speed, wind direction, and temperature. It uses ultrasonic sensors to 
 * accurately measure wind speed and direction without moving parts, making it 
 * durable and maintenance-free.
 *
 * The Atmos 22 operates as a sub-class of the SDI12Sensors class and requires a 
 * 4.0 to 15.0V power supply.
 *
 * @warning Similar to other METER sensors, the Atmos 22 defaults to SDI-12 
 * address '0' from the factory. The sensor outputs a "DDI" serial protocol 
 * string on each power up, which this library disables if supported by the 
 * sensor. After using the sensor with ModularSensors, you may need to manually 
 * re-enable DDI output.
 *
 * @section sensor_atmos22_datasheet Sensor Datasheet
 * Documentation for the SDI-12 Protocol commands and responses for the Meter
 * Atmos 22 can be found at:
 * [https://publications.metergroup.com/Manuals/20588_ATMOS22-12_Manual_Web.pdf](https://publications.metergroup.com/Manuals/20588_ATMOS22-12_Manual_Web.pdf)
 *
 * @section sensor_atmos22_voltages Voltage Ranges
 * - Supply Voltage (VCC to GND), 4.0 to 15.0 VDC
 * - Digital Input Voltage (logic high), 2.8 to 3.9 V (3.6 typical)
 * - Digital Output Voltage (logic high), 3.6 typical
 *
 * @section sensor_atmos22_flags Build flags
 * @see @ref sdi12_group_flags
 *
 * @section sensor_atmos22_ctor Sensor Constructor
 * {{ @ref MeterAtmos22::MeterAtmos22 }}
 *
 * ___
 * @section sensor_atmos22_examples Example Code
 * The Meter Atmos 22 is used in the @menulink{meter_atmos22} example.
 *
 * @menusnip{meter_atmos22}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_METERATMOS22_H_
#define SRC_SENSORS_METERATMOS22_H_

// Debugging Statement
#define MS_METERATMOS22_DEBUG
#define MS_METERATMOS22_DEBUG_DEEP

#ifdef MS_METERATMOS22_DEBUG
#define MS_DEBUGGING_STD "MeterAtmos22"
#endif

#ifdef MS_SDI12SENSORS_DEBUG_DEEP
#define MS_DEBUGGING_DEEP "SDI12Sensors"
#endif

// Included Dependencies
#include "ModSensorDebugger.h"
#undef MS_DEBUGGING_STD
#undef MS_DEBUGGING_DEEP
#include "VariableBase.h"
#include "sensors/SDI12Sensors.h"

/** @ingroup sensor_atmos22 */
/**@{*/

// Sensor Specific Defines
#define ATMOS22_NUM_VARIABLES 4
#define ATMOS22_INC_CALC_VARIABLES 0

/**
 * @anchor sensor_atmos22_timing
 * @name Sensor Timing
 * The sensor timing for a Meter Atmos 22.
 */
/**@{*/
#define ATMOS22_WARM_UP_TIME_MS 30000
#define ATMOS22_STABILIZATION_TIME_MS 2000
#define ATMOS22_MEASUREMENT_TIME_MS 2000
#define ATMOS22_EXTRA_WAKE_TIME_MS 0
/**@}*/


/**
 * @anchor sensor_atmos22_windSpeed
 * @name Wind Speed
 * The wind speed variable from a Meter Atmos 22.
 */
/**@{*/
#define ATMOS22_WS_RESOLUTION 3
#define ATMOS22_WS_VAR_NUM 0
#define ATMOS22_WS_VAR_NAME "windSpeed"
#define ATMOS22_WS_UNIT_NAME "Meter per Second"
#define ATMOS22_WS_DEFAULT_CODE "WindSpd"
/**@}*/

/**
 * @anchor sensor_atmos22_windDirection
 * @name Wind Direction
 * The wind direction variable from a Meter Atmos 22.
 */
/**@{*/
#define ATMOS22_WD_RESOLUTION 1
#define ATMOS22_WD_VAR_NUM 1
#define ATMOS22_WD_VAR_NAME "windDirection"
#define ATMOS22_WD_UNIT_NAME "Degree"
#define ATMOS22_WD_DEFAULT_CODE "WindDir"
/**@}*/

/**
 * @anchor sensor_atmos22_windGust
 * @name Wind Gust
 * The wind gust variable from a Meter Atmos 22.
 */
/**@{*/
#define ATMOS22_WG_RESOLUTION 3
#define ATMOS22_WG_VAR_NUM 2
#define ATMOS22_WG_VAR_NAME "windGustSpeed"
#define ATMOS22_WG_UNIT_NAME "Meter perSecond"
#define ATMOS22_WG_DEFAULT_CODE "Gust"
/**@}*/


/**
 * @anchor sensor_atmos22_temp
 * @name Temperature
 * The temperature variable from a Meter Atmos 22.
 */
/**@{*/
#define ATMOS22_TEMP_RESOLUTION 2
#define ATMOS22_TEMP_VAR_NUM 3
#define ATMOS22_TEMP_VAR_NAME "temperature"
#define ATMOS22_TEMP_UNIT_NAME "Degree Celsius"
#define ATMOS22_TEMP_DEFAULT_CODE "AirTemp"
/**@}*/

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Meter Atmos 22 sensor](@ref sensor_atmos22)
 *
 * @ingroup sensor_atmos22
 */
/* clang-format on */
class MeterAtmos22 : public SDI12Sensors {
 public:
    // Constructors with overloads
    MeterAtmos22(char SDI12address, int8_t powerPin, int8_t dataPin,
                 uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "MeterAtmos22", ATMOS22_NUM_VARIABLES,
                       ATMOS22_WARM_UP_TIME_MS, ATMOS22_STABILIZATION_TIME_MS,
                       ATMOS22_MEASUREMENT_TIME_MS, ATMOS22_EXTRA_WAKE_TIME_MS,
                       ATMOS22_INC_CALC_VARIABLES) {}
    MeterAtmos22(char* SDI12address, int8_t powerPin, int8_t dataPin,
                 uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "MeterAtmos22", ATMOS22_NUM_VARIABLES,
                       ATMOS22_WARM_UP_TIME_MS, ATMOS22_STABILIZATION_TIME_MS,
                       ATMOS22_MEASUREMENT_TIME_MS, ATMOS22_EXTRA_WAKE_TIME_MS,
                       ATMOS22_INC_CALC_VARIABLES) {}
    MeterAtmos22(int SDI12address, int8_t powerPin, int8_t dataPin,
                 uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "MeterAtmos22", ATMOS22_NUM_VARIABLES,
                       ATMOS22_WARM_UP_TIME_MS, ATMOS22_STABILIZATION_TIME_MS,
                       ATMOS22_MEASUREMENT_TIME_MS, ATMOS22_EXTRA_WAKE_TIME_MS,
                       ATMOS22_INC_CALC_VARIABLES) {}
    ~MeterAtmos22() {}

    bool getResults(void) override;
};


// Defines the Wind Speed Variable
/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [wind speed](@ref sensor_atmos22_windSpeed) output from a
 * [Meter Atmos 22 sensor](@ref sensor_atmos22).
 *
 * @ingroup sensor_atmos22
 */
/* clang-format on */
class MeterAtmos22_WindSpeed : public Variable {
 public:
    explicit MeterAtmos22_WindSpeed(
        MeterAtmos22* parentSense, const char* uuid = "",
        const char* varCode = ATMOS22_WS_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)ATMOS22_WS_VAR_NUM,
                   (uint8_t)ATMOS22_WS_RESOLUTION, ATMOS22_WS_VAR_NAME,
                   ATMOS22_WS_UNIT_NAME, varCode, uuid) {}

    MeterAtmos22_WindSpeed()
        : Variable((const uint8_t)ATMOS22_WS_VAR_NUM,
                   (uint8_t)ATMOS22_WS_RESOLUTION, ATMOS22_WS_VAR_NAME,
                   ATMOS22_WS_UNIT_NAME, ATMOS22_WS_DEFAULT_CODE) {}
    ~MeterAtmos22_WindSpeed() {}
};


// Defines the Wind Direction Variable
/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [wind direction](@ref sensor_atmos22_windDirection) output from a
 * [Meter Atmos 22 sensor](@ref sensor_atmos22).
 *
 * @ingroup sensor_atmos22
 */
/* clang-format on */
class MeterAtmos22_WindDirection : public Variable {
 public:
    explicit MeterAtmos22_WindDirection(
        MeterAtmos22* parentSense, const char* uuid = "",
        const char* varCode = ATMOS22_WD_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)ATMOS22_WD_VAR_NUM,
                   (uint8_t)ATMOS22_WD_RESOLUTION, ATMOS22_WD_VAR_NAME,
                   ATMOS22_WD_UNIT_NAME, varCode, uuid) {}

    MeterAtmos22_WindDirection()
        : Variable((const uint8_t)ATMOS22_WD_VAR_NUM,
                   (uint8_t)ATMOS22_WD_RESOLUTION, ATMOS22_WD_VAR_NAME,
                   ATMOS22_WD_UNIT_NAME, ATMOS22_WD_DEFAULT_CODE) {}
    ~MeterAtmos22_WindDirection() {}
};

// Defines the Wind Gust Speed Variable
/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [wind gust speed](@ref sensor_atmos22_windGust) output from a
 * [Meter Atmos 22 sensor](@ref sensor_atmos22).
 *
 * @ingroup sensor_atmos22
 */
/* clang-format on */
class MeterAtmos22_WindGust : public Variable {
 public:
    explicit MeterAtmos22_WindGust(
        MeterAtmos22* parentSense, const char* uuid = "",
        const char* varCode = ATMOS22_WG_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)ATMOS22_WG_VAR_NUM,
                   (uint8_t)ATMOS22_WG_RESOLUTION, ATMOS22_WG_VAR_NAME,
                   ATMOS22_WG_UNIT_NAME, varCode, uuid) {}

    MeterAtmos22_WindGust()
        : Variable((const uint8_t)ATMOS22_WG_VAR_NUM,
                   (uint8_t)ATMOS22_WG_RESOLUTION, ATMOS22_WG_VAR_NAME,
                   ATMOS22_WG_UNIT_NAME, ATMOS22_WG_DEFAULT_CODE) {}
    ~MeterAtmos22_WindGust() {}
};


// Defines the Temperature Variable
/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature](@ref sensor_atmos22_temp) output from a
 * [Meter Atmos 22 sensor](@ref sensor_atmos22).
 *
 * @ingroup sensor_atmos22
 */
/* clang-format on */
class MeterAtmos22_Temp : public Variable {
 public:
    explicit MeterAtmos22_Temp(MeterAtmos22* parentSense, const char* uuid = "",
                               const char* varCode = ATMOS22_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)ATMOS22_TEMP_VAR_NUM,
                   (uint8_t)ATMOS22_TEMP_RESOLUTION, ATMOS22_TEMP_VAR_NAME,
                   ATMOS22_TEMP_UNIT_NAME, varCode, uuid) {}

    MeterAtmos22_Temp()
        : Variable((const uint8_t)ATMOS22_TEMP_VAR_NUM,
                   (uint8_t)ATMOS22_TEMP_RESOLUTION, ATMOS22_TEMP_VAR_NAME,
                   ATMOS22_TEMP_UNIT_NAME, ATMOS22_TEMP_DEFAULT_CODE) {}
    ~MeterAtmos22_Temp() {}
};

#endif  // SRC_SENSORS_METERATMOS22_H_
