# MultiTofSensor Library

An Arduino/ESP32 library for managing multiple VL53L0X Time-of-Flight distance sensors through I2C multiplexers.


### Core Functionality
- Multiple sensor management through I2C multiplexer (up to 8 channels)
- Custom I2C interface support (Wire selection)
- Dynamic sensor addition and mapping
- Automatic channel and sensor management
- Sensor Scanning
- Sensor Reading

### Sensor Configuration
- Configurable multiplexer address (default: 0x70)
- Configurable sensor addresses (default: 0x29)
- Measurement timing budget control

### Hardware Support
- ESP32 platform support
- Arduino platform support
- TCA9548A I2C Multiplexer compatibility
- VL53L0X sensor compatibility

## Dependencies

- [Adafruit_VL53L0X](https://github.com/adafruit/Adafruit_VL53L0X)
- Wire library (built into Arduino IDE)

## Hardware Requirements

- Arduino board or ESP32
- TCA9548A I2C Multiplexer (or compatible)
- One or more VL53L0X sensors
- I2C pull-up resistors (typically 4.7kΩ)
