# AGENTS.md - Instructions for OpenCode working in this repository

## Repository Overview

This is the **Resistive_Sensor** Arduino library - a resistive soil moisture sensor library for PlatformIO and Arduino IDE.

## Key Files

- `src/ResistiveSoilSensor.h` + `src/ResistiveSoilSensor.cpp` - Main library source
- `library.json` - PlatformIO/library manager metadata
- `library.properties` - Arduino IDE library metadata
- `README.md` - Full documentation
- `platformio.ini` - PlatformIO environments

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
│   ├── ResistiveSoilSensor.h
│   └── ResistiveSoilSensor.cpp
├── examples/
│   ├── Basic/
│   └── StaticFunctions/
├── library.json
├── library.properties
├── README.md
├── platformio.ini
└── keywords.txt
```

## Publishing to GitHub

```bash
git init
git add .
git commit -m "Initial release v1.0.0"
git remote add origin https://github.com/Fo170/Resistive_Sensor.git
git branch -M main
git push -u origin main
```

After push, the library will be available via:
- PlatformIO: `pio lib install Resistive_Sensor`
- Arduino IDE: Search "Resistive_Sensor"