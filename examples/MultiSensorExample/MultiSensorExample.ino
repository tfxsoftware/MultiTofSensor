#include <MultiTofSensor.h>

// Create instance of the MultiTofSensor class
MultiTofSensor tof;

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);
    
    Serial.println("Multi VL53L0X Test");
    
    // Initialize the sensor manager with default multiplexer address (0x70)
    if (!tof.begin()) {
        Serial.println("Failed to initialize sensor manager!");
        while (1);
    }
    
    // Add sensors on different multiplexer channels
    // You can add up to 8 sensors (channels 0-7)
    if (!tof.addSensor(0)) {
        Serial.println("Failed to add sensor on channel 0!");
    }
    
    if (!tof.addSensor(1)) {
        Serial.println("Failed to add sensor on channel 1!");
    }
    
    // Set different modes for different sensors
    tof.setHighAccuracy(0);  // High accuracy mode for sensor 0
    tof.setHighSpeed(1);     // High speed mode for sensor 1
    
    Serial.println("Ready!");
}

void loop() {
    // Read and print distances from both sensors
    uint16_t distance0 = tof.readDistance(0);
    uint16_t distance1 = tof.readDistance(1);
    
    Serial.print("Sensor 0: ");
    Serial.print(distance0);
    Serial.print("mm, Sensor 1: ");
    Serial.print(distance1);
    Serial.println("mm");
    
    delay(100);
} 