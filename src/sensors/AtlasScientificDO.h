/*
 *
 *This file was created by Sara Damiano and edited for use of Atlas Scientific Products by Adam Gold
 *
 * The output from the Atlas Scientifc DO is the range in degrees C.
 *     Accuracy is ± __
 *     Range is __
 *
 * Warm up time to completion of header:  __ ms
 */

// Header Guards
#ifndef AtlasScientificDO_h
#define AtlasScientificDO_h

// Debugging Statement
// #define DEBUGGING_SERIAL_OUTPUT Serial

// Included Dependencies
#include "ModSensorDebugger.h"
#include "VariableBase.h"
#include "SensorBase.h"
#include <Wire.h>

// I2C address
#define ATLAS_DO_I2C_ADDR 0x61  // 97

// Sensor Specific Defines
#define ATLAS_DO_NUM_VARIABLES 2
#define ATLAS_DO_WARM_UP_TIME_MS 0
#define ATLAS_DO_STABILIZATION_TIME_MS 0
#define ATLAS_DO_MEASUREMENT_TIME_MS 0

#define ATLAS_DOMGL_RESOLUTION 2
#define ATLAS_DOMGL_VAR_NUM 0

#define ATLAS_DOPCT_RESOLUTION 1
#define ATLAS_DOPCT_VAR_NUM 1

// The main class for the MaxBotix Sonar
class AtlasScientificDO : public Sensor
{
public:
    AtlasScientificDO(int8_t powerPin, uint8_t i2cAddressHex = ATLAS_DO_I2C_ADDR, uint8_t measurementsToAverage = 1);
    ~AtlasScientificDO();
    String getSensorLocation(void) override;
    bool setup(void) override;

    bool addSingleMeasurementResult(void) override;

protected:
    uint8_t _i2cAddressHex;
};

// The class for the DO Concentration Variable
class AtlasScientificDO_DOmgL : public Variable
{
public:
    AtlasScientificDO_DOmgL(Sensor *parentSense,
                        const char *UUID = "", const char *customVarCode = "")
      : Variable(parentSense, ATLAS_DOMGL_VAR_NUM,
               "oxygenDissolved", "milligramPerLiter",
               ATLAS_DOMGL_RESOLUTION,
               "AtlasDOmgL", UUID, customVarCode)
    {}
    ~AtlasScientificDO_DOmgL(){}
};

// The class for the DO Percent of Saturation Variable
class AtlasScientificDO_DOpct : public Variable
{
public:
    AtlasScientificDO_DOpct(Sensor *parentSense,
                        const char *UUID = "", const char *customVarCode = "")
      : Variable(parentSense, ATLAS_DOPCT_VAR_NUM,
               "oxygenDissolvedPercentOfSaturation", "percent",
               ATLAS_DOPCT_RESOLUTION,
               "AtlasDOpct", UUID, customVarCode)
    {}
    ~AtlasScientificDO_DOpct(){}
};

#endif  // Header Guard
