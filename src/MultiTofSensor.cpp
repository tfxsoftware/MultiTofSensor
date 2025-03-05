#include "MultiTofSensor.h"

MultiTofSensor::MultiTofSensor(TwoWire *wire) {
    _wire = wire;
    _numSensors = 0;
    _sensors = nullptr;
    _channelMap = nullptr;
}

bool MultiTofSensor::begin(uint8_t muxAddress) {
    _muxAddress = muxAddress;
    
    // Initialize the I2C bus
    _wire->begin();
    
    // Test communication with multiplexer
    _wire->beginTransmission(_muxAddress);
    if (_wire->endTransmission() != 0) {
        return false;
    }
    
    // Allocate initial memory for sensors and channel map
    _sensors = new Adafruit_VL53L0X[8];  // Support up to 8 sensors initially
    _channelMap = new uint8_t[8];
    
    return true;
}

bool MultiTofSensor::addSensor(uint8_t muxChannel, uint8_t sensorAddress) {
    if (muxChannel > 7 || _numSensors >= 8) {
        return false;
    }
    
    // Select the multiplexer channel
    if (!selectChannel(muxChannel)) {
        return false;
    }
    
    // Initialize the sensor
    if (!_sensors[_numSensors].begin(sensorAddress, false, _wire)) {
        return false;
    }
    
    // Map the channel to the sensor index
    _channelMap[_numSensors] = muxChannel;
    _numSensors++;
    
    return true;
}

uint16_t MultiTofSensor::readDistance(uint8_t muxChannel) {
    int8_t sensorIndex = getSensorIndex(muxChannel);
    if (sensorIndex < 0) {
        return 0;
    }
    
    if (!selectChannel(muxChannel)) {
        return 0;
    }
    
    VL53L0X_RangingMeasurementData_t measure;
    _sensors[sensorIndex].rangingTest(&measure, false);
    
    if (measure.RangeStatus != 4) {
        return measure.RangeMilliMeter;
    }
    return 0;
}

bool MultiTofSensor::setMeasurementTimingBudget(uint8_t muxChannel, uint32_t budget_us) {
    int8_t sensorIndex = getSensorIndex(muxChannel);
    if (sensorIndex < 0) {
        return false;
    }
    
    if (!selectChannel(muxChannel)) {
        return false;
    }
    
    return _sensors[sensorIndex].setMeasurementTimingBudget(budget_us);
}

bool MultiTofSensor::setHighAccuracy(uint8_t muxChannel) {
    return setMeasurementTimingBudget(muxChannel, 200000);
}

bool MultiTofSensor::setHighSpeed(uint8_t muxChannel) {
    return setMeasurementTimingBudget(muxChannel, 20000);
}

bool MultiTofSensor::setLongRange(uint8_t muxChannel) {
    int8_t sensorIndex = getSensorIndex(muxChannel);
    if (sensorIndex < 0) {
        return false;
    }
    
    if (!selectChannel(muxChannel)) {
        return false;
    }
    
    // Increase timing budget for better accuracy at long range
    if (!setMeasurementTimingBudget(muxChannel, 33000)) {
        return false;
    }
    
    return true;
}

bool MultiTofSensor::selectChannel(uint8_t channel) {
    if (channel > 7) {
        return false;
    }
    
    _wire->beginTransmission(_muxAddress);
    _wire->write(1 << channel);
    return (_wire->endTransmission() == 0);
}

int8_t MultiTofSensor::getSensorIndex(uint8_t channel) {
    for (uint8_t i = 0; i < _numSensors; i++) {
        if (_channelMap[i] == channel) {
            return i;
        }
    }
    return -1;
} 