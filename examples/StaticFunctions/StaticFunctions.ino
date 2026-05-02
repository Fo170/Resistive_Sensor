/* Static Functions Example for Resistive_Soil_Sensor (YL-69)
 * 
 * This example shows how to use the static functions
 * for simpler usage without creating an object.
 * 
 * The YL-69 module has a fixed 510kΩ pull-up resistor on the PCB.
 */

#include <ResistiveSoilSensor.h>

// Pin definitions
const uint8_t SOIL_SENSOR_PIN = A0;

// Supply voltage
const float SUPPLY_VOLTAGE = 3.3f;    // 3.3V or 5V

// YL-69 fixed pull-up resistor value
const float PULLUP_KOHMS = 510.0f;    // 510 kΩ (fixed on YL-69 module)

void setup() {
    Serial.begin(115200);
    Serial.println("=== YL-69 Static Functions Demo ===");
    Serial.print("Voltage: ");
    Serial.print(SUPPLY_VOLTAGE);
    Serial.print("V, Pull-up: ");
    Serial.print(PULLUP_KOHMS);
    Serial.println("kΩ (YL-69 fixed)");
}

void loop() {
    // Read raw analog value
    int rawValue = analogRead(SOIL_SENSOR_PIN);

    // Convert to voltage
    float voltage = rawValue * SUPPLY_VOLTAGE / 1023.0f;

    // Calculate soil resistance using static function
    // Calcule_Rsol(voltage, supplyVoltage, pullUpKOhms)
    // Default pullUp is 510.0kΩ for YL-69
    float resistance = Calcule_Rsol(voltage, SUPPLY_VOLTAGE, PULLUP_KOHMS);

    // Calculate humidity using static function
    // Calcule_Hsol(resistance, dryKOhms, wetKOhms)
    float humidity = Calcule_Hsol(resistance, 300.0f, 1.5f);

    // Alternative: Calculate humidity directly from voltage
    // (linear approximation, less accurate than using resistance)
    float humidityLinear = Calcule_Hsol_v(voltage, SUPPLY_VOLTAGE);

    Serial.print("Raw: ");
    Serial.print(rawValue);
    Serial.print(" | V: ");
    Serial.print(voltage, 3);
    Serial.print("V | R: ");
    if (resistance > 999900.0f) {
        Serial.print("INF");
    } else {
        Serial.print(resistance, 1);
        Serial.print("kΩ");
    }
    Serial.print(" | H: ");
    Serial.print(humidity, 1);
    Serial.print("% (linear: ");
    Serial.print(humidityLinear, 1);
    Serial.println("%)");

    delay(1000);
}
