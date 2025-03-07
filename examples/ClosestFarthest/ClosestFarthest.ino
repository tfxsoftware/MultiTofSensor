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
    // Get closest reading
    auto closest = tof.getClosestReading();
    if (closest.valid) {
        Serial.printf("Closest object (Mux: 0x%02X, Ch: %d): %dmm\n",
                    closest.multiplexerAddr,
                    closest.channel,
                    closest.distance);
    }
    
    // Get farthest reading
    auto farthest = tof.getFarthestReading();
    if (farthest.valid) {
        Serial.printf("Farthest object (Mux: 0x%02X, Ch: %d): %dmm\n",
                    farthest.multiplexerAddr,
                    farthest.channel,
                    farthest.distance);
    }
    
    Serial.println("-------------------");
    delay(100);
} 