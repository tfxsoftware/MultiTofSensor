#ifndef MULTI_TOF_SENSOR_H
#define MULTI_TOF_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include <vector>

struct SensorInfo {
    uint8_t multiplexerAddr;
    uint8_t channel;
    Adafruit_VL53L0X* sensor;
};

struct SensorReading {
    uint8_t multiplexerAddr;
    uint8_t channel;
    uint16_t distance;
    bool valid;
    
    // Default constructor for invalid readings
    SensorReading() : multiplexerAddr(0), channel(0), distance(0), valid(false) {}
};

class MultiTofSensor {
public:
    // Constructor
    MultiTofSensor(TwoWire *wire = &Wire);
    ~MultiTofSensor();
    
    // Initialize with specific pins (ESP32)
    bool begin(int sda_pin = -1, int scl_pin = -1, uint32_t frequency = 100000);
    
    // Scan for all multiplexers and sensors
    void scanI2CBus();
    
    // Read distance from a specific sensor by index
    uint16_t readDistance(size_t sensorIndex);
    
    // Get number of found sensors
    size_t getSensorCount() const { return _sensors.size(); }
    
    // Get sensor info
    const SensorInfo* getSensorInfo(size_t index) const;

    // New method to read all sensors at once
    std::vector<SensorReading> readAllSensors();

    // Get reading from closest sensor
    SensorReading getClosestReading();
    
    // Get reading from farthest sensor
    SensorReading getFarthestReading();

private:
    static const uint8_t MAX_CHANNELS = 8;
    static const uint8_t MIN_MUX_ADDR = 0x70;
    static const uint8_t MAX_MUX_ADDR = 0x77;
    
    TwoWire *_wire;                     // I2C interface
    std::vector<SensorInfo> _sensors;   // Vector of found sensors
    
    // Select multiplexer channel
    void selectChannel(uint8_t multiplexerAddr, uint8_t channel);
    
    // Scan channels on a specific multiplexer
    void scanMultiplexerChannels(uint8_t multiplexerAddr);
    
    // Clear all multiplexer channels
    void clearMultiplexers();

    // Helper method to disable all multiplexer channels
    void disableAllChannels();
};

#endif // MULTI_TOF_SENSOR_H