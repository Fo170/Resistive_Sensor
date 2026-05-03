# AGENTS.md - Instructions for OpenCode working in this repository

## Repository Overview

This is the **Resistive_Sensor** Arduino library - a resistive soil moisture sensor library for PlatformIO and Arduino IDE.

## Key Files

- `src/ResistiveSoilSensor.h` - Main library source (single header file)
- `library.json` - PlatformIO/library manager metadata
- `library.properties` - Arduino IDE library metadata
- `README.md` - Full documentation
- `platformio.ini` - PlatformIO environments
- `keywords.txt` - Arduino IDEkeywords

## How to Verify

1. **PlatformIO**: Run `pio lib install` then compile in each env:
   ```bash
   pio run -e uno
   pio run -e nodemcu
   pio run -e esp32dev
   ```

2. **Arduino**: Include library and compile a sketch using ResistiveSoilSensor.h

## Library Structure

```
Resistive_Sensor/
├── src/
│   └── ResistiveSoilSensor.h    (single file - no .cpp)
├── examples/
│   ├── Basic/Basic.ino
│   └── StaticFunctions/StaticFunctions.ino
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

```
VCC (3.3V/5V) --[Rd=510kΩ]-- A0 --[Rsol]-- GND

- Pull-up: 510kΩ (fixed on YL-69 module)
- Rsol: Soil resistance (varies with humidity)
- Dry soil → V_A0 ≈ VCC → 0% humidity
- Wet soil → V_A0 ≈ GND → 100% humidity
```

## Publishing to GitHub

```bash
git add .
git commit -m "Update description"
git push origin main
```

After push, the library will be available via:
- PlatformIO: `pio lib install Resistive_Sensor`
- Arduino IDE: Search "Resistive_Sensor"

## Current Version

- Header: 1.0.0 (in source code)
- `library.json`: 1.0.0
- Consider syncing versions before next release