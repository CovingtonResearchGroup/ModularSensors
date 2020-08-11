/**
 * @file KellerAcculevel.h
 * @copyright 2020 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Written By: Anthony Aufdenkampe <aaufdenkampe@limno.com>
 * Edited by Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Contains the KellerAcculevel sensor subclass and the
 * KellerAcculevel_Pressure, KellerAcculevel_Temp, and KellerAcculevel_Height
 * variable subclasses.
 *
 * These are for Keller Series 30, Class 5, Group 20 sensors using Modbus
 * communication, that are software version 5.20-12.28 and later (i.e. made
 * after the 2012 in the 28th week).
 *
 * Only tested on the Acculevel.
 *
 *
 * @defgroup acculevel_group Keller Acculevel
 * Classes for the @ref acculevel_page
 *
 * @copydoc acculevel_page
 */
/* clang-format off */
/**
 * @page acculevel_page Keller Acculevel
 *
 * @tableofcontents
 *
 * @section acculevel_datasheet Sensor Datasheet
 * [Manual](https://www.kelleramerica.com/manuals-and-software/manuals/Keller_America_Users_Guide.pdf)
 * [Datasheet](https://www.kelleramerica.com/pdf-library/High%20Accuracy%20Analog%20Digital%20Submersible%20Level%20Transmitters%20Acculevel.pdf)
 *
 * @section acculevel_sensor the Keller Acculevel Sensor
 * @ctor_doc{KellerAcculevel, byte modbusAddress, Stream* stream, int8_t powerPin, int8_t powerPin2, int8_t enablePin, uint8_t measurementsToAverage}
 * @subsection acculevel_timing Sensor Timing
 *
 * @section acculevel_pressure Pressure Output
 * @variabledoc{KellerAcculevel,Pressure}
 *   - Range is 0 to 11 bar
 *   - Accuracy is Standard ±0.1% FS, Optional ±0.05% FS
 *   - Result stored in sensorValues[0]
 *   - Resolution is 0.002%
 *   - Reported as millibar (mb)
 *   - Default variable code is kellerAccuPress
 *
 * @section acculevel_temp Temperature Output
 * @variabledoc{KellerAcculevel,Temp}
 *   - Range is -10°C to 60°C
 *   - Accuracy is not specified in the sensor datasheet
 *   - Result stored in sensorValues[1]
 *   - Resolution is 0.01°C
 *   - Reported as degrees Celsius (°C)
 *   - Default variable code is kellerAccuTemp
 *
 * @section acculevel_height Height Output
 * @variabledoc{KellerAcculevel,Height}
 *   - Range is 0 to 900 feet
 *   - Accuracy is Standard ±0.1% FS, Optional ±0.05% FS
 *   - Result stored in sensorValues[2]
 *   - Resolution is 0.002%
 *   - Reported as meters (m)
 *   - Default variable code is kellerAccuHeight
 *
 * @ingroup keller_group
 * ___
 * @section acculevel_examples Example Code
 * The Keller Acculevel is used in the @menulink{acculevel} example.
 *
 * @menusnip{acculevel}
 */
/* clang-format on */

// Header Guards
#ifndef SRC_SENSORS_KELLERACCULEVEL_H_
#define SRC_SENSORS_KELLERACCULEVEL_H_

// Included Dependencies
#include "sensors/KellerParent.h"

// Sensor Specific Defines

/// Sensor::_warmUpTime_ms; the Acculevel warms up in 500ms.
#define ACCULEVEL_WARM_UP_TIME_MS 500
/// Sensor::_stabilizationTime_ms; the Acculevel is stable after 5000ms.
#define ACCULEVEL_STABILIZATION_TIME_MS 5000
/**
 * @brief Sensor::_measurementTime_ms; the Acculevel takes 1500ms to complete a
 * measurement.
 */
#define ACCULEVEL_MEASUREMENT_TIME_MS 1500

/// Decimals places in string representation; pressure should have 5.
#define ACCULEVEL_PRESSURE_RESOLUTION 5

/// Decimals places in string representation; temperature should have 2.
#define ACCULEVEL_TEMP_RESOLUTION 2

/// Decimals places in string representation; height should have 4.
#define ACCULEVEL_HEIGHT_RESOLUTION 4


/* clang-format off */
/**
 * @brief The Sensor sub-class for the
 * [Keller Acculevel sensor](@ref acculevel_page).
 *
 * @ingroup acculevel_group
 */
/* clang-format on */
class KellerAcculevel : public KellerParent {
 public:
    // Constructors with overloads
    /**
     * @brief Construct a new Keller Acculevel
     *
     * @param modbusAddress The modbus address of the sensor.
     * @param stream An Arduino data stream for modbus communication
     * @param powerPin A pin on the mcu controlling power to the sensor.
     * Defaults to -1.
     * @param powerPin2 A pin on the mcu controlling power to the RS485 adapter.
     * Defaults to -1.
     * @param enablePin A pin on the mcu controlling the direction enable on the
     * RS485 adapter.  Defaults to -1.
     * @param measurementsToAverage The number of measurements to take and
     * average before giving a "final" result from the sensor.  Defaults to 1.
     */
    KellerAcculevel(byte modbusAddress, Stream* stream, int8_t powerPin,
                    int8_t powerPin2 = -1, int8_t enablePin = -1,
                    uint8_t measurementsToAverage = 1)
        : KellerParent(
              modbusAddress, stream, powerPin, powerPin2, enablePin,
              measurementsToAverage, Acculevel_kellerModel, "KellerAcculevel",
              KELLER_NUM_VARIABLES, ACCULEVEL_WARM_UP_TIME_MS,
              ACCULEVEL_STABILIZATION_TIME_MS, ACCULEVEL_MEASUREMENT_TIME_MS) {}
    /**
     * @copydoc KellerAcculevel::KellerAcculevel
     */
    KellerAcculevel(byte modbusAddress, Stream& stream, int8_t powerPin,
                    int8_t powerPin2 = -1, int8_t enablePin = -1,
                    uint8_t measurementsToAverage = 1)
        : KellerParent(
              modbusAddress, stream, powerPin, powerPin2, enablePin,
              measurementsToAverage, Acculevel_kellerModel, "KellerAcculevel",
              KELLER_NUM_VARIABLES, ACCULEVEL_WARM_UP_TIME_MS,
              ACCULEVEL_STABILIZATION_TIME_MS, ACCULEVEL_MEASUREMENT_TIME_MS) {}
    // Destructor
    ~KellerAcculevel() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [gauge pressure (vented and barometric pressure corrected) output](@ref acculevel_pressure)
 * from a [Keller Acculevel](@ref acculevel_page).
 *
 * @ingroup acculevel_group
 */
/* clang-format on */
class KellerAcculevel_Pressure : public Variable {
 public:
    /**
     * @brief Construct a new KellerAcculevel_Pressure object.
     *
     * @param parentSense The parent KellerAcculevel providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "kellerAccuPress".
     */
    explicit KellerAcculevel_Pressure(KellerAcculevel* parentSense,
                                      const char*      uuid = "",
                                      const char* varCode   = "kellerAccuPress")
        : Variable(parentSense, (const uint8_t)KELLER_PRESSURE_VAR_NUM,
                   (uint8_t)ACCULEVEL_PRESSURE_RESOLUTION, "pressureGauge",
                   "millibar", varCode, uuid) {}
    /**
     * @brief Construct a new KellerAcculevel_Pressure object.
     *
     * @note This must be tied with a parent KellerAcculevel before it can be
     * used.
     */
    KellerAcculevel_Pressure()
        : Variable((const uint8_t)KELLER_PRESSURE_VAR_NUM,
                   (uint8_t)ACCULEVEL_PRESSURE_RESOLUTION, "pressureGauge",
                   "millibar", "kellerAccuPress") {}
    /**
     * @brief Destroy the KellerAcculevel_Pressure object - no action needed.
     */
    ~KellerAcculevel_Pressure() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [temperature output](@ref acculevel_temp) from a
 * [Keller Acculevel](@ref acculevel_page).
 *
 * @ingroup acculevel_group
 */
/* clang-format on */
class KellerAcculevel_Temp : public Variable {
 public:
    /**
     * @brief Construct a new KellerAcculevel_Temp object.
     *
     * @param parentSense The parent KellerAcculevel providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "kellerAccuTemp".
     */
    explicit KellerAcculevel_Temp(KellerAcculevel* parentSense,
                                  const char*      uuid    = "",
                                  const char*      varCode = "kellerAccuTemp")
        : Variable(parentSense, (const uint8_t)KELLER_TEMP_VAR_NUM,
                   (uint8_t)ACCULEVEL_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", varCode, uuid) {}
    /**
     * @brief Construct a new KellerAcculevel_Temp object.
     *
     * @note This must be tied with a parent KellerAcculevel before it can be
     * used.
     */
    KellerAcculevel_Temp()
        : Variable((const uint8_t)KELLER_TEMP_VAR_NUM,
                   (uint8_t)ACCULEVEL_TEMP_RESOLUTION, "temperature",
                   "degreeCelsius", "kellerAccuTemp") {}
    /**
     * @brief Destroy the KellerAcculevel_Temp object - no action needed.
     */
    ~KellerAcculevel_Temp() {}
};


/* clang-format off */
/**
 * @brief The Variable sub-class used for the
 * [gauge height (water level with regard to an arbitrary gage datum) output](@ref acculevel_height)
 * from a [Keller Acculevel](@ref acculevel_page).
 *
 * @ingroup acculevel_group
 */
/* clang-format on */
class KellerAcculevel_Height : public Variable {
 public:
    /**
     * @brief Construct a new KellerAcculevel_Height object.
     *
     * @param parentSense The parent KellerAcculevel providing the result
     * values.
     * @param uuid A universally unique identifier (UUID or GUID) for the
     * variable; optional with the default value of an empty string.
     * @param varCode A short code to help identify the variable in files;
     * optional with a default value of "kellerAccuHeight".
     */
    explicit KellerAcculevel_Height(KellerAcculevel* parentSense,
                                    const char*      uuid = "",
                                    const char* varCode   = "kellerAccuHeight")
        : Variable(parentSense, (const uint8_t)KELLER_HEIGHT_VAR_NUM,
                   (uint8_t)ACCULEVEL_HEIGHT_RESOLUTION, "gaugeHeight", "meter",
                   varCode, uuid) {}
    /**
     * @brief Construct a new KellerAcculevel_Height object.
     *
     * @note This must be tied with a parent KellerAcculevel before it can be
     * used.
     */
    KellerAcculevel_Height()
        : Variable((const uint8_t)KELLER_HEIGHT_VAR_NUM,
                   (uint8_t)ACCULEVEL_HEIGHT_RESOLUTION, "gaugeHeight", "meter",
                   "kellerAccuHeight") {}
    /**
     * @brief Destroy the KellerAcculevel_Height object - no action needed.
     */
    ~KellerAcculevel_Height() {}
};

#endif  // SRC_SENSORS_KELLERACCULEVEL_H_
