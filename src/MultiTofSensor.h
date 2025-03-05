#ifndef MULTI_TOF_SENSOR_H
#define MULTI_TOF_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VL53L0X.h>

class MultiTofSensor {
public:
    // Constructor
    MultiTofSensor(TwoWire *wire = &Wire);
    
    // Initialize the multiplexer and sensors
    bool begin(uint8_t muxAddress = 0x70);
    
    // Add a new sensor to the system
    bool addSensor(uint8_t muxChannel, uint8_t sensorAddress = 0x29);
    
    // Read distance from a specific sensor
    uint16_t readDistance(uint8_t muxChannel);

private:
    TwoWire *_wire;                     // I2C interface
    uint8_t _muxAddress;                // I2C multiplexer address
    Adafruit_VL53L0X *_sensors;         // Array of VL53L0X sensor objects
    uint8_t _numSensors;                // Number of sensors currently configured
    uint8_t *_channelMap;               // Map of multiplexer channels to sensor indices
    
    // Select multiplexer channel
    bool selectChannel(uint8_t channel);
    
    // Get sensor index from mux channel
    int8_t getSensorIndex(uint8_t channel);
};

#endif // MULTI_TOF_SENSOR_H 