#include <ResistiveSoilSensor.h>
// Example for Resistive_Soil_Sensor (YL-69)
// Capteur côté GND (montage classique)
ResistiveSoilSensor sensor(A0);

// OU capteur côté VCC
// ResistiveSoilSensor sensor(A0, PullMode::PULL_DOWN);

void setup() {
  Serial.begin(9600);
  
  // Configuration optionnelle
  sensor.setVoltage(5.0f);
  sensor.setPullResistor(510000.0f);
  sensor.setCalibration(300000.0f, 1500.0f);  // sec, mouillé
  sensor.setDigitalPin(2);  // D0 du LM393 sur pin 2
  sensor.setSmoothing(true, 20);  // Moyenne sur 20 échantillons
  
  // Seuils personnalisés
  sensor.setLevelThresholds(10, 30, 60, 80);
}

void loop() {
  SensorData data = sensor.read();
  
  Serial.print("Humidité: ");
  Serial.print(data.moisturePercent);
  Serial.print("% | Niveau: ");
  Serial.print(static_cast<int>(data.level));
  Serial.print(" | Résistance: ");
  Serial.print(data.soilResistance);
  Serial.println("Ω");
  
  delay(1000);
}