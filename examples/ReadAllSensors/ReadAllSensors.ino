#include <MultiTofSensor.h>

MultiTofSensor tof;

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    
    tof.begin(21, 22);  // SDA, SCL
    tof.scanI2CBus();
    
    Serial.printf("Found %d sensors\n", tof.getSensorCount());
}

void loop() {
    auto readings = tof.readAllSensors();
    
    for (const auto& reading : readings) {
        if (reading.valid) {
            Serial.printf("Sensor (Mux: 0x%02X, Ch: %d): %dmm\n",
                        reading.multiplexerAddr,
                        reading.channel,
                        reading.distance);
        }
    }
    
    delay(100);
} 