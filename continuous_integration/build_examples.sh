#!/bin/sh

# Ignore errors until the end
set +e

compile_failures = 0

echo " | Compiler | Example | Board | Compilation | " >> $GITHUB_STEP_SUMMARY
echo " | -------- | ------- | ----- | ----------- | " >> $GITHUB_STEP_SUMMARY

for example in single_sensor simple_logging simple_logging_LearnEnviroDIY DRWI_NoCellular DRWI_2G DRWI_DigiLTE DRWI_SIM7080LTE DRWI_Mayfly1 DRWI_Mayfly1_WiFi double_logger baro_rho_correction data_saving logging_to_MMW logging_to_ThingSpeak

do
    
    for fqbn in 'EnviroDIY:avr:envirodiy_mayfly' 'arduino:avr:mega' 'arduino:samd:mzero_bl' 'adafruit:samd:adafruit_feather_m0' 'SODAQ:samd:sodaq_autonomo'
    
    do
        echo "::group::Setting build flags"
        echo "::debug::Setting build flags for $fqbn"
        if [ $fqbn = 'EnviroDIY:avr:envirodiy_mayfly' ]; then
            echo "EXTRA_BUILD_FLAGS=-DNEOSWSERIAL_EXTERNAL_PCINT" >> $GITHUB_ENV
        fi
        if [ $fqbn = 'arduino:avr:mega' ]; then
            echo "EXTRA_BUILD_FLAGS=-DNEOSWSERIAL_EXTERNAL_PCINT" >> $GITHUB_ENV
        fi
        if [ $fqbn = 'arduino:samd:mzero_bl' ]; then
            echo "EXTRA_BUILD_FLAGS=-DNEOSWSERIAL_EXTERNAL_PCINT -DARDUINO_SAMD_ZERO -D__SAMD21G18A__ -DUSB_VID=0x2341 -DUSB_PID=0x804d -DUSBCON" >> $GITHUB_ENV
        fi
        if [ $fqbn = 'adafruit:samd:adafruit_feather_m0' ]; then
            echo "EXTRA_BUILD_FLAGS=-DNEOSWSERIAL_EXTERNAL_PCINT -DARDUINO_SAMD_ZERO -DARM_MATH_CM0PLUS -DADAFRUIT_FEATHER_M0 -D__SAMD21G18A__ -DUSB_VID=0x239A -DUSB_PID=0x800B -DUSBCON -DUSB_CONFIG_POWER=100" >> $GITHUB_ENV
        fi
        if [ $fqbn = 'SODAQ:samd:sodaq_autonomo' ]; then
            echo "EXTRA_BUILD_FLAGS=-DNEOSWSERIAL_EXTERNAL_PCINT -DVERY_LOW_POWER -D__SAMD21J18A__ -DUSB_VID=0x2341 -DUSB_PID=0x804d -DUSBCON" >> $GITHUB_ENV
        fi
        echo "::endgroup::"
        
        BUILD_EXAMPLE="examples/$example/"
        
        echo "::debug::Running Arduino CLI for examples/$example/"
        arduino-cli --config-file continuous_integration/arduino_cli.yaml compile --clean --build-property "build.extra_flags=$EXTRA_BUILD_FLAGS" --fqbn $fqbn $BUILD_EXAMPLE 2>&1
        result_code=${PIPESTATUS[0]}
        
        echo " | Arduino CLI | $example | $fqbn | $result_code |" >> $GITHUB_STEP_SUMMARY
        if [ $result_code ]; then
            compile_failures=$(($compile_failures + 1))
        fi
        echo "Total failures so far: $compile_failures"
        
    done
    
    for pio_environment in mayfly, mega, arduino_zero, adafruit_feather_m0, autonomo
    
    do
        
        PLATFORMIO_CI_SRC="examples/$example/"
        PLATFORMIO_DEFAULT_ENVS=$pio_environment
        PLATFORMIO_LIB_EXTRA_DIRS=/home/runner/.platformio/lib
        
        echo "::debug::Running PlatformIO for $PLATFORMIO_CI_SRC"
        echo "$LIBRARY_INSTALL_SOURCE"
        pio run --environment autonomo --project-conf="continuous_integration/platformio.ini" 2>&1
        result_code=${PIPESTATUS[0]}
        
        echo " | PlatformIO | $example | $pio_environment | $result_code |" >> $GITHUB_STEP_SUMMARY
        if [ $result_code ]; then
            compile_failures=$(($compile_failures + 1))
        fi
        echo "Total failures so far: $compile_failures"
        
    done
done

if [ $compile_failures ]; then
    exit 1
fi
