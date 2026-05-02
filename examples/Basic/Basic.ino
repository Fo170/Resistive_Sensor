/* Basic Example for Resistive_Soil_Sensor (YL-69)
 * 
 * This example shows how to use the ResistiveSoilSensor library
 * to read a YL-69 soil moisture sensor and display the results.
 * 
 * The YL-69 module has a fixed 510kΩ pull-up resistor on the PCB.
 */

#include <ResistiveSoilSensor.h>

// Create sensor object on pin A0
// Default: 510kΩ pull-up (YL-69 module), 3.3V supply
ResistiveSoilSensor sensor(A0);

void setup() {
    Serial.begin(115200);
    Serial.println("=== YL-69 Soil Moisture Sensor Demo ===");

    // Configure voltage (3.3V for ESP8266/ESP32, 5V for Arduino)
    sensor.setVoltage(3.3f);

    // Optional: Set calibration values for your sensor
    // Measure resistance when dry and when saturated in water
    sensor.setCalibration(300.0f, 1.5f);  // dry, wet resistance in kΩ

    Serial.println("Ready to measure!");
}

void loop() {
    // Read voltage from analog pin
    float voltage = sensor.readVoltage();

    // Calculate soil resistance (kΩ)
    float resistance = sensor.readSoilResistanceKOhms();

    // Calculate humidity percentage (0-100%)
    // Uses calibration: 0% = dry (300kΩ), 100% = wet (1.5kΩ)
    float humidity = sensor.readMoisturePercent();

    // Display results
    Serial.print("Voltage: ");
    Serial.print(voltage, 3);
    Serial.print(" V | Resistance: ");
    if (resistance > 999900.0f) {
        Serial.print("INF (dry)");
    } else {
        Serial.print(resistance, 1);
        Serial.print(" kΩ");
    }
    Serial.print(" | Humidity: ");
    Serial.print(humidity, 1);
    Serial.println(" %");

    // Wait 1 second between readings
    delay(1000);
}
