# ResistiveSoilSensor

Arduino library for resistive soil moisture sensor. Calculates soil resistance and humidity percentage from voltage reading. Supports both PULL_UP and PULL_DOWN configurations.

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Arduino Badge](https://img.shields.io/badge/framework-arduino-brightgreen?logo=arduino.svg)](https://www.arduino.cc/)
[![PlatformIO](https://img.shields.io/badge/platformio-All_µC-violet?logo=platformio)](https://platformio.org/)
[![Arduino Library Badge](https://www.ardu-badge.com/badge/Resistive_Sensor.svg)](https://github.com/Fo170?tab=repositories)

## Description

This library reads resistive soil moisture sensors (YL-69, FC-28, etc.). It calculates:
- Soil resistance (in ohms) from voltage reading
- Moisture percentage (0% = dry, 100% = saturated)
- Qualitative moisture levels (very dry, dry, moist, wet, very wet)
- Optional smoothing with moving average

### Supported Circuits

**PULL_UP (default - YL-69 module):**
```
VCC --[Rd=510kΩ]-- A0 --[Rsol]-- GND

- Dry soil → V_A0 → VCC (high resistance)
- Wet soil → V_A0 → GND (low resistance)

Formula: Rsol = Rd * V / (VCC - V)
```

**PULL_DOWN (alternative):**
```
VCC --[Rsol]-- A0 --[Rd=510kΩ]-- GND

- Dry soil → V_A0 → GND (high resistance)
- Wet soil → V_A0 → VCC (low resistance)

Formula: Rsol = Rd * (VCC - V) / V
```

### Sensor Module (YL-69)

The YL-69 sensor module includes:
- **A0**: Analog output (varies with moisture)
- **D0**: Digital output (HIGH/LOW based on threshold)
- **VR1**: Potentiometer to adjust threshold
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

other sensor :

![Sensor wiring](capteur%20conductif%202.png)

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

For long-term installations, consider using capacitive soil moisture sensors.

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

### Basic Example (PULL_UP - default)

```cpp
#include <ResistiveSoilSensor.h>

// Create sensor on pin A0 (default: PULL_UP, 510kΩ, 3.3V)
ResistiveSoilSensor sensor(A0);

void setup() {
  Serial.begin(115200);
}

void loop() {
  SensorData data = sensor.read();

  Serial.print("Voltage: ");
  Serial.print(data.voltage);
  Serial.print(" V | Resistance: ");
  Serial.print(data.soilResistance);
  Serial.print(" Ω | Moisture: ");
  Serial.print(data.moisturePercent);
  Serial.print(" % | Level: ");
  Serial.println(static_cast<int>(data.level));

  delay(1000);
}
```

### PULL_DOWN Configuration

```cpp
#include <ResistiveSoilSensor.h>

// PULL_DOWN: VCC --[Rsol]-- A0 --[Rd]-- GND
ResistiveSoilSensor sensor(A0, PullMode::PULL_DOWN);

void setup() {
  Serial.begin(115200);
  sensor.setPullResistor(510000.0f);
  sensor.setVoltage(3.3f);
}

void loop() {
  SensorData data = sensor.read();
  Serial.print("Moisture: ");
  Serial.print(data.moisturePercent);
  Serial.println(" %");
  delay(1000);
}
```

### Using Digital Output (D0)

```cpp
#include <ResistiveSoilSensor.h>

ResistiveSoilSensor sensor(A0);

void setup() {
  Serial.begin(115200);
  sensor.setDigitalPin(2);  // D0 connected to pin D2
}

void loop() {
  bool digitalState = sensor.readDigitalOutput();

  if (digitalState) {
    Serial.println("Soil is WET enough");
  } else {
    Serial.println("Soil is DRY");
  }

  delay(1000);
}
```

### Using Smoothing (Moving Average)

```cpp
#include <ResistiveSoilSensor.h>

ResistiveSoilSensor sensor(A0);

void setup() {
  Serial.begin(115200);
  sensor.setSmoothing(true, 20);  // Average over 20 samples
}

void loop() {
  SensorData data = sensor.read();
  Serial.print("Smoothed Moisture: ");
  Serial.print(data.moisturePercent);
  Serial.println(" %");
  delay(100);
}
```

### Using Static Functions

```cpp
#include <ResistiveSoilSensor.h>

void loop() {
  float v = analogRead(A0) * 3.3f / 1023.0f;

  float r = ResistiveSoilSensor::calculateSoilResistance(v, 3.3f, 510000.0f, PullMode::PULL_UP);
  float h = ResistiveSoilSensor::calculateMoisturePercent(r, 300000.0f, 1500.0f);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" %");
}
```

### Custom Thresholds for Moisture Levels

```cpp
#include <ResistiveSoilSensor.h>

ResistiveSoilSensor sensor(A0);

void setup() {
  Serial.begin(115200);

  // Customize moisture level thresholds
  // Parameters: veryDry, dry, moist, wet
  sensor.setLevelThresholds(15, 35, 65, 85);
}

void loop() {
  MoistureLevel level = sensor.readMoistureLevel();

  switch (level) {
    case MoistureLevel::VERY_DRY:
      Serial.println("🌵 Very Dry - Water now!");
      break;
    case MoistureLevel::DRY:
      Serial.println("🏜️ Dry - Water soon");
      break;
    case MoistureLevel::MOIST:
      Serial.println("🌿 Moist - OK");
      break;
    case MoistureLevel::WET:
      Serial.println("💧 Wet - Good");
      break;
    case MoistureLevel::VERY_WET:
      Serial.println("🌊 Very Wet - Too much water!");
      break;
    case MoistureLevel::ERROR:
      Serial.println("❌ Sensor error");
      break;
  }

  delay(1000);
}
```

## API Reference

### Enums

- `PullMode` - Circuit configuration (type-safe enum class)
  - `PullMode::PULL_UP` - VCC → Rd → A0 → Rsol → GND
  - `PullMode::PULL_DOWN` - VCC → Rsol → A0 → Rd → GND

- `MoistureLevel` - Qualitative moisture levels
  - `MoistureLevel::VERY_DRY` - Very dry soil
  - `MoistureLevel::DRY` - Dry soil
  - `MoistureLevel::MOIST` - Moist soil
  - `MoistureLevel::WET` - Wet soil
  - `MoistureLevel::VERY_WET` - Very wet / saturated soil
  - `MoistureLevel::ERROR` - Invalid reading

### Structures

- `SensorData` - Complete sensor reading
  - `uint16_t rawValue` - Raw ADC value (0-1023)
  - `float voltage` - Measured voltage in Volts
  - `float soilResistance` - Soil resistance in ohms
  - `float moisturePercent` - Moisture percentage (0-100)
  - `MoistureLevel level` - Qualitative level
  - `bool valid` - Data validity flag

- `CalibrationPoint` - Calibration point
  - `float resistance` - Resistance in ohms
  - `uint8_t percent` - Corresponding moisture percentage

### Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `DEFAULT_PULL_OHMS` | 510000.0f | Default pull resistor (510kΩ) |
| `DEFAULT_VOLTAGE` | 3.3f | Default supply voltage |
| `DEFAULT_DRY_OHMS` | 300000.0f | Resistance at 0% (300kΩ) |
| `DEFAULT_WET_OHMS` | 1500.0f | Resistance at 100% (1.5kΩ) |
| `ADC_MAX` | 1023 | ADC resolution |
| `VOLTAGE_TOLERANCE` | 0.005f | Saturation threshold |

### Class: ResistiveSoilSensor

#### Constructors
- `ResistiveSoilSensor(uint8_t analogPin)` - Default (PULL_UP, 510kΩ, 3.3V)
- `ResistiveSoilSensor(uint8_t analogPin, PullMode mode)` - With pull mode
- `ResistiveSoilSensor(uint8_t analogPin, PullMode mode, float pullOhms, float voltage)` - Full configuration

#### Configuration Methods
- `setPullMode(PullMode mode)` - Set PULL_UP or PULL_DOWN
- `setPullResistor(float ohms)` - Set pull resistor in ohms
- `setVoltage(float voltage)` - Set supply voltage (3.3V or 5.0V)
- `setCalibration(float dryOhms, float wetOhms)` - Set calibration points
- `setDigitalPin(uint8_t digitalPin)` - Set D0 pin (automatically sets pinMode to INPUT)
- `setSmoothing(bool enable, uint8_t samples = 10)` - Enable/disable moving average smoothing
- `setLevelThresholds(uint8_t veryDry, uint8_t dry, uint8_t moist, uint8_t wet)` - Customize moisture level thresholds

#### Read Methods
- `SensorData read()` - Read all sensor data at once
- `uint16_t readRaw()` - Read raw ADC value (0-1023)
- `float readVoltage()` - Read voltage in Volts
- `float readSoilResistance()` - Read soil resistance in ohms
- `float readMoisturePercent()` - Read moisture percentage (0-100)
- `MoistureLevel readMoistureLevel()` - Read qualitative moisture level
- `bool readDigitalOutput()` - Read D0 pin state (requires setDigitalPin first)

#### Getters
- `PullMode getPullMode()` - Returns current pull mode
- `float getPullResistor()` - Returns pull resistor in ohms
- `float getVoltage()` - Returns supply voltage
- `float getDryOhms()` - Returns dry calibration resistance
- `float getWetOhms()` - Returns wet calibration resistance
- `uint8_t getAnalogPin()` - Returns analog pin number
- `bool hasDigitalPin()` - Returns true if digital pin is configured

### Static Functions

All utility functions are now static methods of the class:

| Function | Description |
|----------|-------------|
| `ResistiveSoilSensor::calculateSoilResistance(v, voltage, pullOhms, mode)` | Calculate Rsol from voltage |
| `ResistiveSoilSensor::calculateMoisturePercent(resistance, dryOhms, wetOhms)` | Calculate moisture from resistance |
| `ResistiveSoilSensor::calculateVoltage(resistance, voltage, pullOhms, mode)` | Calculate expected voltage |
| `ResistiveSoilSensor::percentToLevel(percent, veryDry, dry, moist, wet)` | Convert percentage to qualitative level |

## Calibration

Default calibration values (may vary with sensor):

| Condition | Rsol | Voltage (PULL_UP) |
|----------|------|------------------|
| Dry (0%) | > 300 kΩ | ~3.3 V |
| Wet (100%) | ~1.5 kΩ | ~0 V |

Customize:
```cpp
sensor.setCalibration(300000.0f, 1500.0f);  // dry, wet in ohms
```

## Migration from v1.x to v1.1.0

### Breaking Changes

1. **Enum syntax**: `PULL_UP` → `PullMode::PULL_UP`
2. **Static functions**: `calculateSoilResistance(...)` → `ResistiveSoilSensor::calculateSoilResistance(...)`
3. **Digital output**: `sensor.readDigitalOutput(pin)` → `sensor.setDigitalPin(pin)` then `sensor.readDigitalOutput()`
4. **New features**: Added `SensorData`, `MoistureLevel`, smoothing, and level thresholds

### Quick Migration Guide

**v1.x code:**
```cpp
ResistiveSoilSensor sensor(A0, PULL_DOWN, 510000.0f, 3.3f);
bool state = sensor.readDigitalOutput(2);
```

**v2.0 equivalent:**
```cpp
ResistiveSoilSensor sensor(A0, PullMode::PULL_DOWN, 510000.0f, 3.3f);
sensor.setDigitalPin(2);
bool state = sensor.readDigitalOutput();
```

## Links

- [AZ-Delivery Bodenfeuchtesensor](https://www.az-delivery.de/fr/products/bodenfeuchtesensor-sonde?)
- [GitHub Repository](https://github.com/Fo170/Resistive_Sensor)

## License

MIT License
