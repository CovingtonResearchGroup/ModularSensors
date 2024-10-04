/**
 * @file MeterAtmos14.h
 * @copyright Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino.
 * This library is published under the BSD-3 license.
 * @author Written By: Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the MeterAtmos14 sensor subclass and the variable subclasses
 * MeterAtmos14_Ea, MeterAtmos14_Temp, and MeterAtmos14_VWC.
 *
 * These are for the Meter Atmos 14 Advanced Soil Moisture probe.
 *
 * This depends on the EnviroDIY SDI-12 library and the SDI12Sensors super
 * class.
 */
/* clang-format off */
/**
 * @defgroup sensor_atmos14 Meter Atmos 14
 * Classes for the Meter Atmos 14 soil moisture probe.
 *
 * @ingroup sdi12_group
 *
 * @tableofcontents
 * @m_footernavigation
 *
 * @section sensor_atmos14_intro Introduction
 *
 * Meter Environmental makes two series of soil moisture sensors, the
 * [ECH2O series](https://www.metergroup.com/environment/products/?product_category=9525) and the
 * [Teros series](https://www.metergroup.com/environment/products/teros-12/).
 * __This page is for the Teros series.__
 *
 * Both series of sensors operate as sub-classes of the SDI12Sensors class.
 * They require a 3.5-12V power supply, which can be turned off between
 * measurements. While contrary to the manual, they will run with power as low
 * as 3.3V. On the 5TM with a stereo cable, the power is connected to the tip,
 * data to the ring, and ground to the sleeve. On the bare-wire version, the
 * power is connected to the _white_ cable, data to _red_, and ground to the
 * unshielded cable.
 *
 * @warning Coming from the factory, METER sensors are set at SDI-12 address
 * '0'.  They also output a "DDI" serial protocol string on each power up.
 * This library *disables the DDI output string* on all newer METER sensors
 * that support disabling it.  After using a METER sensor with ModularSensors,
 * you will need to manually re-enable the DDI output if you wish to use it.
 *
 * @section sensor_atmos14_datasheet Sensor Datasheet
 * Documentation for the SDI-12 Protocol commands and responses for the Meter
 * Atmos 14 can be found at:
 * http://publications.metergroup.com/Manuals/20587_ATMOS14-12_Manual_Web.pdf
 *
 * @section sensor_atmos14_voltages Voltage Ranges
 * - Supply Voltage (VCC to GND), 4.0 to 15.0 VDC
 * - Digital Input Voltage (logic high), 2.8 to 3.9 V (3.6 typical)
 * - Digital Output Voltage (logic high), 3.6 typical
 *
 * @section sensor_atmos14_flags Build flags
 * @see @ref sdi12_group_flags
 *
 * @section sensor_atmos14_ctor Sensor Constructor
 * {{ @ref MeterAtmos14::MeterAtmos14 }}
 *
 * ___
 * @section sensor_atmos14_examples Example Code
 * The Meter Teros is used in the @menulink{meter_atmos14} example.
 *
 * @menusnip{meter_atmos14}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_METERATMOS14_H_
#define SRC_SENSORS_METERATMOS14_H_

// Debugging Statement
// #define MS_METERATMOS14_DEBUG

#ifdef MS_METERATMOS14_DEBUG
#define MS_DEBUGGING_STD "MeterAtmos14"
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

/** @ingroup sensor_atmos14 */
/**@{*/

// Sensor Specific Defines
/// @brief Sensor::_numReturnedValues; the Atmos 14 can report 2 raw values -
/// counts and temperature.
#define ATMOS14_NUM_VARIABLES 4
// There are no calculated values.
#define ATMOS14_INC_CALC_VARIABLES 0

/**
 * @anchor sensor_atmos14_timing
 * @name Sensor Timing
 * The sensor timing for a Meter Atmos 14
 */
/**@{*/
/// @brief Sensor::_warmUpTime_ms; the Atmos 14 warm-up time in SDI-12 mode:
/// 260
#define ATMOS14_WARM_UP_TIME_MS 260
/// @brief Sensor::_stabilizationTime_ms; the Atmos 14 is stable after 50ms.
#define ATMOS14_STABILIZATION_TIME_MS 50
/// @brief Sensor::_measurementTime_ms; the Atmos 14 takes 50 ms to
/// complete a measurement.
#define ATMOS14_MEASUREMENT_TIME_MS 50
/// @brief Extra wake time required for an SDI-12 sensor between the "break"
/// and the time the command is sent.  The Atmos 14 requires no extra time.
#define ATMOS14_EXTRA_WAKE_TIME_MS 0
/**@}*/


/**
 * @anchor sensor_atmos14_temp
 * @name Temperature
 * The temperature variable from a Meter Atmos 14
 * - Range is -40°C to 80°C
 * - Accuracy is +/- 0.2°C.
 *
 * {{ @ref MeterAtmos14_Temp::MeterAtmos14_Temp }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; temperature should have 2.
 *
 * 1 is reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging - resolution is 0.1°C
 */
#define ATMOS14_TEMP_RESOLUTION 2
/// @brief Sensor variable number; temperature is stored in sensorValues[1].
#define ATMOS14_TEMP_VAR_NUM 0
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "temperature"
#define ATMOS14_TEMP_VAR_NAME "temperature"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "degreeCelsius" (°C)
#define ATMOS14_TEMP_UNIT_NAME "degreeCelsius"
/// @brief Default variable short code;
#define ATMOS14_TEMP_DEFAULT_CODE "AirTemp"
/**@}*/

/**
 * @anchor sensor_atmos14_rh
 * @name RH
 * The Relative humidity variable from a Meter Atmos 14
 * - Range is 0-100% (0.00-1.00)
 * - Accuracy depends on value and ranges between 1.5 and 2.5%
 *
 * {{ @ref MeterAtmos14_RH::MeterAtmos14_RH }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; RH should have 4.
 *
 * 3 are reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging.
 */
#define ATMOS14_RH_RESOLUTION 4
/// @brief Sensor variable number; RH is stored in sensorValues[1].
#define ATMOS14_RH_VAR_NUM 1
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "permittivity"
#define ATMOS14_RH_VAR_NAME "relativeHumidity"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "faradPerMeter" (F/m)
#define ATMOS14_RH_UNIT_NAME "Dimensionless"
/// @brief Default variable short code; "RH"
#define ATMOS14_RH_DEFAULT_CODE "RH"
/**@}*/

/**
 * @anchor sensor_atmos14_pres
 * @name Atmospheric Pressure
 * The atmospheric pressure variable from a Meter Atmos 14
 *   - Range is 1-120 kPa.
 *   - Accuracy is 0.05 kPa at 25 C *
 * {{ @ref MeterAtmos14_Pres::MeterAtmos14_Pres }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; Pressure should have 3.
 * 2 are reported, adding extra digit to resolution to allow the proper number
 * of significant figures for averaging.
 */
#define ATMOS14_PRES_RESOLUTION 3
/// @brief Sensor variable number; VWC is stored in sensorValues[2].
#define ATMOS14_PRES_VAR_NUM 2
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
#define ATMOS14_PRES_VAR_NAME "pressureAbsolute"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
#define ATMOS14_PRES_UNIT_NAME "Kilopascal"
/// @brief Default variable short code; "baro"
#define ATMOS14_PRES_DEFAULT_CODE "Baro"
/**@}*/

/**
 * @anchor sensor_atmos14_vaporPressure
 * @name Vapor Pressure
 * The Vapor Pressure variable from a Meter Atmos 14
 * - Range of 0-47 kPa and a resolution of 0.01 kPa.
 *   Accuracy is variable across the range of temperatures
 *   and relative humidities. Refer to Fig 3 in Integrator
 *   Guide.
 *
 * {{ @ref MeterAtmos14_vaporPressure::MeterAtmos14_vaporPressure }}
 */
/**@{*/
/**
 * @brief Decimals places in string representation; Vapor pressure should
 * have 3.
 */
#define ATMOS14_VP_RESOLUTION 3
/// @brief Sensor variable number; EA is stored in sensorValues[0].
#define ATMOS14_VP_VAR_NUM 4
/// @brief Variable name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/variablename/);
/// "counter"
#define ATMOS14_VP_VAR_NAME "vaporPressure"
/// @brief Variable unit name in
/// [ODM2 controlled vocabulary](http://vocabulary.odm2.org/units/);
/// "count"
#define ATMOS14_VP_UNIT_NAME "Kilopascal"
/// @brief Default variable short code;
#define ATMOS14_VP_DEFAULT_CODE "AtmosVP"
/**@}*/

/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Meter Atmos 14 sensor](@ref sensor_atmos14)
 *
 * @ingroup sensor_atmos14
 */
/* clang-format on */
class MeterAtmos14 : public SDI12Sensors {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Meter Atmos 14 object.
     *
     * The SDI-12 address of the sensor, the Arduino pin controlling power
     * on/off, and the Arduino pin sending and receiving data are required for
     * the sensor constructor.  Optionally, you can include a number of distinct
     * readings to average.  The data pin must be a pin that supports pin-change
     * interrupts.
     *
     * @param SDI12address The SDI-12 address of the Atmos 14; can be a char,
     * char*, or int.
     * @warning The SDI-12 address **must** be changed from the factory
     * programmed value of "0" before the Atmos 14 can be used with
     * ModularSensors!
     * @param powerPin The pin on the mcu controlling power to the Atmos 14
     * Use -1 if it is continuously powered.
     * - The Atmos 14 requires a 3.5-12V power supply, which can be turned off
     * between measurements
     * @param dataPin The pin on the mcu connected to the data line of the
     * SDI-12 circuit.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor; optional with a
     * default value of 1.
     */
    MeterAtmos14(char SDI12address, int8_t powerPin, int8_t dataPin,
                 uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "MeterAtmos14", ATMOS14_NUM_VARIABLES,
                       ATMOS14_WARM_UP_TIME_MS, ATMOS14_STABILIZATION_TIME_MS,
                       ATMOS14_MEASUREMENT_TIME_MS, ATMOS14_EXTRA_WAKE_TIME_MS,
                       ATMOS14_INC_CALC_VARIABLES) {}
    /**
     * @copydoc MeterAtmos14::MeterAtmos14
     */
    MeterAtmos14(char* SDI12address, int8_t powerPin, int8_t dataPin,
                 uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "MeterAtmos14", ATMOS14_NUM_VARIABLES,
                       ATMOS14_WARM_UP_TIME_MS, ATMOS14_STABILIZATION_TIME_MS,
                       ATMOS14_MEASUREMENT_TIME_MS, ATMOS14_EXTRA_WAKE_TIME_MS,
                       ATMOS14_INC_CALC_VARIABLES) {}
    /**
     * @copydoc MeterAtmos14::MeterAtmos14
     */
    MeterAtmos14(int SDI12address, int8_t powerPin, int8_t dataPin,
                 uint8_t measurementsToAverage = 1)
        : SDI12Sensors(SDI12address, powerPin, dataPin, measurementsToAverage,
                       "MeterAtmos14", ATMOS14_NUM_VARIABLES,
                       ATMOS14_WARM_UP_TIME_MS, ATMOS14_STABILIZATION_TIME_MS,
                       ATMOS14_MEASUREMENT_TIME_MS, ATMOS14_EXTRA_WAKE_TIME_MS,
                       ATMOS14_INC_CALC_VARIABLES) {}
    /**
     * @brief Destroy the Meter Atmos 14 object
     */
    ~MeterAtmos14() {}

    /**
     * @copydoc SDI12Sensors::getResults()
     */
    bool getResults(void) override;
};


// Defines the Vapor Pressure Variable
/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [vapor pressure](@ref sensor_atmos14_vaporPressure) output from a
 * [Meter Atmos sensor](@ref sensor_atmos14).
 *
 * @ingroup sensor_atmos14
 */
/* clang-format on */
class MeterAtmos14_VaporPressure : public Variable {
 public:
    /**
     * @brief Construct a new MeterAtmos14_VaporPressure object.
     *
     * @param parentSense The parent MeterAtmos14 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "AtmosVP".
     */
    explicit MeterAtmos14_VaporPressure(
        MeterAtmos14* parentSense, const char* uuid = "",
        const char* varCode = ATMOS14_VP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)ATMOS14_VP_VAR_NUM,
                   (uint8_t)ATMOS14_VP_RESOLUTION, ATMOS14_VP_VAR_NAME,
                   ATMOS14_VP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new MeterAtmos14_VaporPressure object.
     *
     * @note This must be tied with a parent MeterAtmos14 before it can be used.
     */
    MeterAtmos14_VaporPressure()
        : Variable((const uint8_t)ATMOS14_VP_VAR_NUM,
                   (uint8_t)ATMOS14_VP_RESOLUTION, ATMOS14_VP_VAR_NAME,
                   ATMOS14_VP_UNIT_NAME, ATMOS14_VP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the MeterAtmos14_VaporPressure object - no action needed.
     */
    ~MeterAtmos14_VaporPressure() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [Relative Humidity (RH) output](@ref sensor_atmos14_rh)
 * from a [Meter Atmos sensor](@ref sensor_atmos14).
 *
 * @ingroup sensor_atmos14
 */
/* clang-format on */
class MeterAtmos14_RH : public Variable {
 public:
    /**
     * @brief Construct a new MeterAtmos14_RH object.
     *
     * @param parentSense The parent MeterAtmos14 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "RH".
     */
    explicit MeterAtmos14_RH(MeterAtmos14* parentSense, const char* uuid = "",
                             const char* varCode = ATMOS14_RH_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)ATMOS14_RH_VAR_NUM,
                   (uint8_t)ATMOS14_RH_RESOLUTION, ATMOS14_RH_VAR_NAME,
                   ATMOS14_RH_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new MeterAtmos14_RH object.
     *
     * @note This must be tied with a parent MeterAtmos14 before it can be used.
     */
    MeterAtmos14_RH()
        : Variable((const uint8_t)ATMOS14_RH_VAR_NUM,
                   (uint8_t)ATMOS14_RH_RESOLUTION, ATMOS14_RH_VAR_NAME,
                   ATMOS14_RH_UNIT_NAME, ATMOS14_RH_DEFAULT_CODE) {}
    /**
     * @brief Destroy the MeterAtmos14_RH object - no action needed.
     */
    ~MeterAtmos14_RH() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [Atmospheric Pressure output](@ref sensor_atmos14_pres)
 * from a [Meter Atmos sensor](@ref sensor_atmos14).
 *
 * @ingroup sensor_atmos14
 */
/* clang-format on */
class MeterAtmos14_Pres : public Variable {
 public:
    /**
     * @brief Construct a new MeterAtmos14_Pres object.
     *
     * @param parentSense The parent MeterAtmos14 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "Baro".
     */
    explicit MeterAtmos14_Pres(MeterAtmos14* parentSense, const char* uuid = "",
                               const char* varCode = ATMOS14_PRES_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)ATMOS14_PRES_VAR_NUM,
                   (uint8_t)ATMOS14_PRES_RESOLUTION, ATMOS14_PRES_VAR_NAME,
                   ATMOS14_PRES_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new MeterAtmos14_Pres object.
     *
     * @note This must be tied with a parent MeterAtmos14 before it can be used.
     */
    MeterAtmos14_Pres()
        : Variable((const uint8_t)ATMOS14_PRES_VAR_NUM,
                   (uint8_t)ATMOS14_PRES_RESOLUTION, ATMOS14_PRES_VAR_NAME,
                   ATMOS14_PRES_UNIT_NAME, ATMOS14_PRES_DEFAULT_CODE) {}
    /**
     * @brief Destroy the MeterAtmos14_Pres object - no action needed.
     */
    ~MeterAtmos14_Pres() {}
};

/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [Temperature output](@ref sensor_atmos14_temp)
 * from a [Meter Atmos sensor](@ref sensor_atmos14).
 *
 * @ingroup sensor_atmos14
 */
/* clang-format on */
class MeterAtmos14_Temp : public Variable {
 public:
    /**
     * @brief Construct a new MeterAtmos14_Temp object.
     *
     * @param parentSense The parent MeterAtmos14 providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "AirTemp".
     */
    explicit MeterAtmos14_Temp(MeterAtmos14* parentSense, const char* uuid = "",
                               const char* varCode = ATMOS14_TEMP_DEFAULT_CODE)
        : Variable(parentSense, (const uint8_t)ATMOS14_TEMP_VAR_NUM,
                   (uint8_t)ATMOS14_TEMP_RESOLUTION, ATMOS14_TEMP_VAR_NAME,
                   ATMOS14_TEMP_UNIT_NAME, varCode, uuid) {}
    /**
     * @brief Construct a new MeterAtmos14_Temp object.
     *
     * @note This must be tied with a parent MeterAtmos14 before it can be used.
     */
    MeterAtmos14_Temp()
        : Variable((const uint8_t)ATMOS14_TEMP_VAR_NUM,
                   (uint8_t)ATMOS14_TEMP_RESOLUTION, ATMOS14_TEMP_VAR_NAME,
                   ATMOS14_TEMP_UNIT_NAME, ATMOS14_TEMP_DEFAULT_CODE) {}
    /**
     * @brief Destroy the MeterAtmos14_Temp object - no action needed.
     */
    ~MeterAtmos14_Temp() {}
};

/**@}*/
#endif  // SRC_SENSORS_METERATMOS14_H_
