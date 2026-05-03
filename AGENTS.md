# AGENTS.md - Instructions for OpenCode working in this repository

## Repository Overview

This is the **ResistiveSoilSensor** Arduino library - a resistive soil moisture sensor library for PlatformIO and Arduino IDE. Supports both PULL_UP and PULL_DOWN configurations.

## Key Files

- `src/ResistiveSoilSensor.h` - Main library source (single header file)
- `library.json` - PlatformIO/library manager metadata
- `library.properties` - Arduino IDE library metadata
- `README.md` - Full documentation
- `platformio.ini` - PlatformIO environments
- `keywords.txt` - Arduino IDE keywords

## How to Verify

1. **PlatformIO**: Run `pio lib install ResistiveSoilSensor` then compile in each env:
   ```bash
   pio run -e uno
   pio run -e nodemcu
   pio run -e esp32dev
   pio run -e esp32-s2
   pio run -e stm32
   ```

2. **Arduino**: Include library and compile a sketch using ResistiveSoilSensor.h

## Library Structure

```
Resistive_Sensor/
├── src/
│   └── ResistiveSoilSensor.h    (single file - no .cpp)
├── examples/
│   └── Basic/Basic.ino
├── README.md
├── library.json
├── library.properties
├── platformio.ini
├── keywords.txt
├── AGENTS.md
├── .gitignore
├── lien.txt
├──电容式土壤传感器.png
└──电容式土壤传感器 2.png
```

## Circuit Configuration

**PULL_UP (default - YL-69):**
```
VCC --[Rd=510kΩ]-- A0 --[Rsol]-- GND
- Dry → V_A0 → VCC (0%)
- Wet → V_A0 → GND (100%)
```

**PULL_DOWN:**
```
VCC --[Rsol]-- A0 --[Rd=510kΩ]-- GND
- Dry → V_A0 → GND (0%)
- Wet → V_A0 → VCC (100%)
```

## New Features (v1.1.0)

- **PullMode enum class**: `PullMode::PULL_UP`, `PullMode::PULL_DOWN`
- **MoistureLevel enum class**: VERY_DRY, DRY, MOIST, WET, VERY_WET, ERROR
- **SensorData struct**: Complete reading (raw, voltage, resistance, percent, level)
- **Smoothing**: Moving average with configurable sample count
- **Level thresholds**: Customizable moisture level boundaries
- **Static functions**: Now class methods (e.g., `ResistiveSoilSensor::calculateSoilResistance()`)

## API Changes (Breaking)

| Old | New |
|-----|-----|
| `PULL_UP` | `PullMode::PULL_UP` |
| `Calcule_Rsol(v, voltage, pull)` | `ResistiveSoilSensor::calculateSoilResistance(v, voltage, pull, mode)` |
| `sensor.readDigitalOutput(pin)` | `sensor.setDigitalPin(pin)` then `sensor.readDigitalOutput()` |

## Publishing to GitHub

```bash
git add .
git commit -m "v1.1.0 - Add PULL_UP/PULL_DOWN support, SensorData, smoothing"
git push origin main
```

After push, the library will be available via:
- PlatformIO: `pio lib install ResistiveSoilSensor`
- Arduino IDE: Search "ResistiveSoilSensor"

## Current Version

- Header: 1.1.0 (in source code)
- `library.json`: 1.1.0

## Author

- FOURNET Olivier - olivier.fournet@free.fr
- GitHub: https://github.com/Fo170
