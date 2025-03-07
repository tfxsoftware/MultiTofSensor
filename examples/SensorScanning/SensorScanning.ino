#include <MultiTofSensor.h>

MultiTofSensor tof;

void setup() {
    Serial.begin(115200);
    
    // Wait for serial connection (useful when using USB)
    while (!Serial) delay(10);
    
    Serial.println("\n\nVL53L0X Multi-Sensor Scanner");
    Serial.println("============================");
    
    // Initialize I2C with ESP32 pins (for Arduino, just use tof.begin())
    Serial.println("Initializing I2C...");
    if (!tof.begin(21, 22)) {  // SDA, SCL
        Serial.println("Failed to initialize I2C!");
        while (1) delay(10);
    }
    
    // Start the scanning process
    Serial.println("\nScanning for sensors...");
    Serial.println("------------------------");
    tof.scanI2CBus();
    
    // Report results
    size_t sensorCount = tof.getSensorCount();
    if (sensorCount == 0) {
        Serial.println("\nNo sensors found!");
        Serial.println("Please check your connections and try again.");
        Serial.println("\nTroubleshooting tips:");
        Serial.println("1. Check power connections");
        Serial.println("2. Verify I2C connections (SDA and SCL)");
        Serial.println("3. Make sure multiplexer address is correct");
        Serial.println("4. Check if sensors are properly connected to multiplexer channels");
        while (1) delay(10);
    }
    
    Serial.printf("\nFound %d sensor(s)!\n", sensorCount);
    Serial.println("------------------------");
    
    // Print sensor map
    Serial.println("\nSensor Map:");
    Serial.println("------------------------");
    for (size_t i = 0; i < sensorCount; i++) {
        const SensorInfo* info = tof.getSensorInfo(i);
        if (info) {
            Serial.printf("Sensor %d:\n", i);
            Serial.printf("  - Multiplexer: 0x%02X\n", info->multiplexerAddr);
            Serial.printf("  - Channel: %d\n", info->channel);
            Serial.println("------------------------");
        }
    }
    
    Serial.println("\nStarting sensor readings...");
    Serial.println("------------------------");
}

void loop() {
    // Read all sensors
    auto readings = tof.readAllSensors();
    
    Serial.println("\nCurrent Readings:");
    Serial.println("------------------------");
    
    for (const auto& reading : readings) {
        if (reading.valid) {
            Serial.printf("Mux 0x%02X Ch %d: %dmm\n", 
                        reading.multiplexerAddr,
                        reading.channel,
                        reading.distance);
        } else {
            Serial.printf("Mux 0x%02X Ch %d: Invalid reading\n",
                        reading.multiplexerAddr,
                        reading.channel);
        }
    }
    
    // Also show closest and farthest readings
    auto closest = tof.getClosestReading();
    if (closest.valid) {
        Serial.printf("\nClosest object: %dmm (Mux 0x%02X, Ch %d)\n",
                    closest.distance,
                    closest.multiplexerAddr,
                    closest.channel);
    }
    
    auto farthest = tof.getFarthestReading();
    if (farthest.valid) {
        Serial.printf("Farthest object: %dmm (Mux 0x%02X, Ch %d)\n",
                    farthest.distance,
                    farthest.multiplexerAddr,
                    farthest.channel);
    }
    
    Serial.println("------------------------");
    delay(1000);  // Update once per second
} 