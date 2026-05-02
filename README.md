# Resistive_Sensor

Arduino library for resistive soil moisture sensor YL-69. Calculates soil resistance and humidity percentage from a voltage reading.

![GitHub last commit](https://img.shields.io/github/last-commit/Fo170/Resistive_Sensor)
![GitHub release (latest by date)](https://img.shields.io/github/v/release/Fo170/Resistive_Sensor)
![PlatformIO](https://badges.platformio.net/platformio/library.svg)
![License](https://img.shields.io/github/license/Fo170/Resistive_Sensor)

## Description

This library reads a resistive soil moisture sensor YL-69. It calculates:
- Soil resistance (in kΩ) from voltage reading
- Moisture percentage (0% = dry, 100% = saturated)

### Circuit (YL-69 Module)

The YL-69 sensor module uses a **fixed 510kΩ pull-up resistor** connected to VCC:

```
VCC (3.3V or 5V) ----[Rd = 510kΩ]---- Analog Pin A0 ----[Rsol]---- GND
```

- **VCC**: Power (3.3V or 5V)
- **Rd**: Fixed pull-up resistor (510 kΩ on YL-69 module)
- **Rsol**: Soil resistance (varies with humidity)
- **A0**: Analog output to Arduino

Voltage calculation (pull-up):
```
V_A0 = VCC * Rsol / (Rsol + Rd)
```

Soil resistance from voltage:
```
Rsol = Rd * V_A0 / (VCC - V_A0)
```

So:
- **Dry soil** (high Rsol): V_A0 → VCC (near 3.3V or 5V) = 0% humidity
- **Wet soil** (low Rsol): V_A0 → GND (near 0V) = 100% humidity

### Sensor Module

The YL-69 sensor module includes:
- **A0**: Analog output (varies with moisture)
- **D0**: Digital output (HIGH/LOW based on threshold)
- **VR1**: Potentiometer to adjust threshold (between GND and VCC)
- **Comparator IC** (LM393) for digital output
- **510kΩ pull-up resistor** (fixed, on PCB)

### Pinout

| Pin | Description |
|-----|-------------|
| VCC | Power (3.3V or 5V) |
| GND | Ground |
| A0 | Analog output to Arduino (A0) |
| D0 | Digital output to Arduino (D2, etc.) |

### Wiring to Arduino

![Sensor wiring](capteur%20conductif.png)

### Another sensor 
is shown for illustrative purposes. The calculation formulas need to be changed.

![Circuit](capteur%20conductif%202.png)

### Supported Boards

- Arduino Uno, Nano, Mega
- ESP8266, ESP32
- STM32
- PlatformIO compatible boards

## Important: Sensor Durability

⚠️ **This type of sensor degrades quickly over time!**

### Why?

The copper wires immersed in soil are subjected to:
- **Corrosion** due to water and moisture
- **Electrolysis** caused by the electrical current flowing between the wires
- **Salt effects** from minerals and fertilizers in the soil

This phenomenon causes the copper to oxidize and degrade within weeks or months, making the readings unreliable.

### Recommendations

1. **Do not leave the sensor permanently powered** - Only power it during measurements
2. **Use stainless steel wires** instead of copper for better durability
3. **Consider capacitive sensors** as a long-term alternative
4. **Replace the sensor regularly** if you notice drift or incorrect readings

For long-term installations, consider using capacitive soil moisture sensors (capacitive sensing technology).

## Installation

### PlatformIO Library Manager

```bash
platformio lib install Resistive_Sensor
```

Or add to `platformio.ini`:
```ini
lib_deps =
    Resistive_Sensor
```

### Arduino Library Manager

Search for "Resistive_Sensor" in Arduino IDE Library Manager.

### Manual Installation

Download the `src` folder and rename it to `Resistive_Sensor`, then copy to:
- **Arduino**: `libraries/`
- **PlatformIO**: `lib/`

## Usage

### Basic Example

```cpp
#include <ResistiveSoilSensor.h>

// Create sensor object on pin A0
// Pull-up is fixed at 510kΩ (YL-69 module)
ResistiveSoilSensor sensor(A0);

void setup() {
  Serial.begin(115200);

  // Configure voltage (3.3V or 5V)
  sensor.setVoltage(3.3f);
}

void loop() {
  float voltage = sensor.readVoltage();
  float resistance = sensor.readSoilResistanceKOhms();
  float humidity = sensor.readMoisturePercent();

  Serial.print("Tension: ");
  Serial.print(voltage);
  Serial.print(" V, Resistance: ");
  Serial.print(resistance);
  Serial.print(" kΩ, Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  delay(1000);
}
```

### Using Digital Output (D0)

The module also provides a digital output (D0) that goes HIGH or LOW based on the threshold set by VR1:

```cpp
#include <ResistiveSoilSensor.h>

ResistiveSoilSensor sensor(A0);

void setup() {
  Serial.begin(115200);
  pinMode(2, INPUT);  // D0 connected to pin D2
}

void loop() {
  bool digitalState = sensor.readDigitalOutput(2);

  if (digitalState) {
    Serial.println("Soil is WET enough");
  } else {
    Serial.println("Soil is DRY");
  }

  delay(1000);
}
```

**Note**: Adjust VR1 potentiometer to set the desired threshold level.

For simpler use without creating an object:

```cpp
#include <ResistiveSoilSensor.h>

void loop() {
  float v = analogRead(A0) * 3.3 / 1023.0;

  // Calculate soil resistance (kΩ) - 510kΩ pull-up
  float r = Calcule_Rsol(v, 3.3, 510.0);

  // Calculate humidity (%)
  float h = Calcule_Hsol(r);
  // Or directly from voltage (less accurate)
  float h2 = Calcule_Hsol_v(v);

  Serial.print("Humidity: ");
  Serial.println(h);
}
```

## Calibration

With pull-up configuration:
- **Dry soil** (high Rsol): V_A0 → VCC (high voltage, ~3.3V) = 0% humidity
- **Wet soil** (low Rsol): V_A0 → GND (low voltage, ~0V) = 100% humidity

The default calibration values (may vary with sensor):

| Condition | Rsol | Expected Voltage (3.3V) |
|----------|------|------------------------|
| Dry (0%) | > 300 kΩ | ~3.3 V |
| Wet (100%) | ~1.5 kΩ | ~0 V |

To customize calibration:

```cpp
sensor.setCalibration(300.0f, 1.5f);  // dryKOhms, wetKOhms
```

Or using the supply voltage:

```cpp
sensor.setVoltage(5.0f);         // If using 5V instead of 3.3V
```

## API Reference

### ResistiveSoilSensor Class

#### Constructor
- `ResistiveSoilSensor(uint8_t analogPin)` - Default (510kΩ pull-up, 3.3V)
- `ResistiveSoilSensor(uint8_t analogPin, float voltage)` - Custom voltage

#### Methods
- `readVoltage()` - Returns voltage (V)
- `readSoilResistance()` - Returns soil resistance (kΩ)
- `readSoilResistanceKOhms()` - Returns soil resistance (kΩ)
- `readMoisturePercent()` - Returns humidity (0-100%)
- `readMoisturePercentFromVoltage(float voltage)` - Calculate humidity from voltage
- `readDigitalOutput(uint8_t digitalPin)` - Read D0 digital output
- `setVoltage(float v)` - Set supply voltage
- `setCalibration(float dry, float wet)` - Set calibration

#### Constants
- `PULLUP_RESISTOR` - Fixed pull-up value (510.0f kΩ)

### Static Functions

- `Calcule_Rsol(float v, float voltage, float pullUp)` - Calculate Rsol from voltage (default pullUp: 510.0)
- `Calcule_Hsol(float r, float dry, float wet)` - Calculate humidity from resistance
- `Calcule_Hsol_v(float v, float voltage)` - Calculate humidity from voltage (linear approximation)
- `Calcule_Tension(float rsol, float voltage, float pullUp)` - Calculate expected voltage from Rsol

## Links

- [AZ-Delivery Bodenfeuchtesensor](https://www.az-delivery.de/fr/products/bodenfeuchtesensor-sonde?)
- [GitHub Repository](https://github.com/Fo170/Resistive_Sensor)
- [Arduino Library](https://www.arduinolibrary.info/collections/libraries)

## License

MIT License
