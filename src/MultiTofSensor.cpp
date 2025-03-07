#include "MultiTofSensor.h"

MultiTofSensor::MultiTofSensor(TwoWire *wire) {
    _wire = wire;
    _numSensors = 0;
    _sensors = nullptr;
    _channelMap = nullptr;
}

MultiTofSensor::~MultiTofSensor() {
    for (auto& sensor : _sensors) {
        delete sensor.sensor;
    }
}

bool MultiTofSensor::begin(int sda_pin, int scl_pin, uint32_t frequency) {
    if (sda_pin >= 0 && scl_pin >= 0) {
        _wire->begin(sda_pin, scl_pin);
    } else {
        _wire->begin();
    }
    _wire->setClock(frequency);
    
    return true;
}

void MultiTofSensor::scanI2CBus() {
    Serial.println("\nScanning for I2C multiplexers...");
    clearMultiplexers();
    
    for (uint8_t addr = MIN_MUX_ADDR; addr <= MAX_MUX_ADDR; addr++) {
        _wire->beginTransmission(addr);
        uint8_t error = _wire->endTransmission();
        
        if (error == 0) {
            Serial.printf("Found multiplexer at address 0x%02X\n", addr);
            scanMultiplexerChannels(addr);
        }
        delay(10);
    }
    
    Serial.printf("Total sensors found: %d\n", _sensors.size());
}

void MultiTofSensor::scanMultiplexerChannels(uint8_t multiplexerAddr) {
    for (uint8_t channel = 0; channel < MAX_CHANNELS; channel++) {
        selectChannel(multiplexerAddr, channel);
        
        Adafruit_VL53L0X* sensor = new Adafruit_VL53L0X();
        if (sensor->begin(0x29, false, _wire)) {
            SensorInfo info = {multiplexerAddr, channel, sensor};
            _sensors.push_back(info);
            
            Serial.printf("  Found sensor on channel %d\n", channel);
        } else {
            delete sensor;
        }
        delay(10);
    }
}

uint16_t MultiTofSensor::readDistance(size_t sensorIndex) {
    if (sensorIndex >= _sensors.size()) {
        return 0;
    }
    
    const auto& info = _sensors[sensorIndex];
    selectChannel(info.multiplexerAddr, info.channel);
    
    VL53L0X_RangingMeasurementData_t measure;
    info.sensor->rangingTest(&measure, false);
    
    clearMultiplexers();
    
    if (measure.RangeStatus != 4) {
        return measure.RangeMilliMeter;
    }
    return 0;
}

void MultiTofSensor::selectChannel(uint8_t multiplexerAddr, uint8_t channel) {
    clearMultiplexers();
    
    if (channel < MAX_CHANNELS) {
        _wire->beginTransmission(multiplexerAddr);
        _wire->write(1 << channel);
        _wire->endTransmission();
        delay(10);
    }
}

void MultiTofSensor::clearMultiplexers() {
    for (uint8_t addr = MIN_MUX_ADDR; addr <= MAX_MUX_ADDR; addr++) {
        _wire->beginTransmission(addr);
        _wire->write(0);
        _wire->endTransmission();
    }
    delayMicroseconds(200);
}

const SensorInfo* MultiTofSensor::getSensorInfo(size_t index) const {
    if (index >= _sensors.size()) {
        return nullptr;
    }
    return &_sensors[index];
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

bool MultiTofSensor::setMeasurementTimingBudget(uint8_t muxChannel, uint32_t budget_us) {
    int8_t sensorIndex = getSensorIndex(muxChannel);
    if (sensorIndex < 0) {
        return false;
    }
    
    if (!selectChannel(muxChannel)) {
        return false;
    }
    
    return _sensors[sensorIndex].sensor->setMeasurementTimingBudget(budget_us);
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

int8_t MultiTofSensor::getSensorIndex(uint8_t channel) {
    for (uint8_t i = 0; i < _numSensors; i++) {
        if (_channelMap[i] == channel) {
            return i;
        }
    }
    return -1;
} 

void MultiTofSensor::scanSensors() {
    
}

std::vector<SensorReading> MultiTofSensor::readAllSensors() {
    std::vector<SensorReading> readings;
    
    // First disable all channels on all multiplexers
    disableAllChannels();
    
    // Read each sensor
    for (const auto& sensorInfo : _sensors) {
        // Select only this sensor's channel
        selectChannel(sensorInfo.multiplexerAddr, sensorInfo.channel);
        delay(1);  // Give sensor time to stabilize
        
        SensorReading reading;
        reading.multiplexerAddr = sensorInfo.multiplexerAddr;
        reading.channel = sensorInfo.channel;
        
        VL53L0X_RangingMeasurementData_t measure;
        sensorInfo.sensor->rangingTest(&measure, false);
        
        if (measure.RangeStatus != 4) {
            reading.distance = measure.RangeMilliMeter;
            reading.valid = true;
        } else {
            reading.distance = 0;
            reading.valid = false;
        }
        
        readings.push_back(reading);
        
        // Disable this channel before moving to next
        _wire->beginTransmission(sensorInfo.multiplexerAddr);
        _wire->write(0);
        _wire->endTransmission();
        delayMicroseconds(200);
    }
    
    // Finally, disable all channels again
    disableAllChannels();
    
    return readings;
}

void MultiTofSensor::disableAllChannels() {
    for (uint8_t addr = MIN_MUX_ADDR; addr <= MAX_MUX_ADDR; addr++) {
        _wire->beginTransmission(addr);
        _wire->write(0);  // Write 0 to disable all channels
        _wire->endTransmission();
    }
    delayMicroseconds(200);
}