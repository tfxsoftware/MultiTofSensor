#ifndef MULTI_TOF_SENSOR_H
#define MULTI_TOF_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include <vector>

/**
 * @brief Structure to hold information about a single VL53L0X sensor
 * 
 * Contains the multiplexer address, channel number, and pointer to the sensor object
 */
struct SensorInfo {
    uint8_t multiplexerAddr;     ///< I2C address of the multiplexer (typically 0x70-0x77)
    uint8_t channel;            ///< Channel number on the multiplexer (0-7)
    Adafruit_VL53L0X* sensor;   ///< Pointer to the VL53L0X sensor object
};

/**
 * @brief Structure to hold a single sensor reading
 * 
 * Contains the sensor's location (multiplexer and channel), distance reading,
 * and validity flag
 */
struct SensorReading {
    uint8_t multiplexerAddr;     ///< I2C address of the multiplexer
    uint8_t channel;            ///< Channel number on the multiplexer
    uint16_t distance;          ///< Distance reading in millimeters
    bool valid;                 ///< Flag indicating if the reading is valid
    
    /// Default constructor for invalid readings
    SensorReading() : multiplexerAddr(0), channel(0), distance(0), valid(false) {}
};

/**
 * @brief Main class for managing multiple VL53L0X sensors through I2C multiplexers
 */
class MultiTofSensor {
public:
    /**
     * @brief Construct a new Multi Tof Sensor object
     * 
     * @param wire Pointer to TwoWire instance (defaults to &Wire)
     */
    MultiTofSensor(TwoWire *wire = &Wire);
    
    /**
     * @brief Destroy the Multi Tof Sensor object
     * 
     * Cleans up dynamically allocated sensor objects
     */
    ~MultiTofSensor();
    
    /**
     * @brief Initialize the I2C bus and prepare for sensor operations
     * 
     * @param sda_pin SDA pin number (use -1 for default Arduino pins)
     * @param scl_pin SCL pin number (use -1 for default Arduino pins)
     * @param frequency I2C bus frequency in Hz (default 100kHz)
     * @return true if initialization successful
     * @return false if initialization failed
     */
    bool begin(int sda_pin = -1, int scl_pin = -1, uint32_t frequency = 100000);
    
    /**
     * @brief Scan I2C bus for multiplexers and sensors
     * 
     * Automatically detects all multiplexers and their connected VL53L0X sensors.
     * Stores found sensors in internal vector.
     */
    void scanI2CBus();
    
    /**
     * @brief Get the number of sensors found during scanning
     * 
     * @return size_t Number of sensors currently managed
     */
    size_t getSensorCount() const { return _sensors.size(); }
    
    /**
     * @brief Get information about a specific sensor
     * 
     * @param index Index of the sensor
     * @return const SensorInfo* Pointer to sensor information, nullptr if index invalid
     */
    const SensorInfo* getSensorInfo(size_t index) const;
    
    /**
     * @brief Read all sensors and return their measurements
     * 
     * @return std::vector<SensorReading> Vector of readings from all sensors
     */
    std::vector<SensorReading> readAllSensors();
    
    /**
     * @brief Get reading from the closest detected object
     * 
     * Reads all sensors and returns information about the closest detected object.
     * @return SensorReading Reading from the sensor that detected the closest object
     */
    SensorReading getClosestReading();
    
    /**
     * @brief Get reading from the farthest detected object
     * 
     * Reads all sensors and returns information about the farthest detected object.
     * @return SensorReading Reading from the sensor that detected the farthest object
     */
    SensorReading getFarthestReading();

private:
    static const uint8_t MAX_CHANNELS = 8;     ///< Maximum channels per multiplexer
    static const uint8_t MIN_MUX_ADDR = 0x70;  ///< First possible multiplexer address
    static const uint8_t MAX_MUX_ADDR = 0x77;  ///< Last possible multiplexer address
    
    TwoWire *_wire;                           ///< I2C interface pointer
    std::vector<SensorInfo> _sensors;         ///< Vector of found sensors
    
    /**
     * @brief Select a specific channel on a multiplexer
     * 
     * @param multiplexerAddr I2C address of the multiplexer
     * @param channel Channel number to select (0-7)
     */
    void selectChannel(uint8_t multiplexerAddr, uint8_t channel);
    
    /**
     * @brief Scan all channels on a specific multiplexer for sensors
     * 
     * @param multiplexerAddr I2C address of the multiplexer to scan
     */
    void scanMultiplexerChannels(uint8_t multiplexerAddr);
    
    /**
     * @brief Disable all channels on all multiplexers
     * 
     * Used to ensure clean state between operations
     */
    void disableAllChannels();
};

#endif // MULTI_TOF_SENSOR_H