# Use this command to create a new project from scratch

1. Change the name "Learn-Unity" to the name of your project.
2. It will create the project in /Volumes/Work01/Developer/ESP32Projects.
3. It requires the template project (this project) in /Volumes/Work01/Developer/ESP32Projects/Template
4. Copy and paste the below to a github agent mode prompt to run.
5. If you are going to use doxygen check out the Doxyfile and mod it to suit your project
8. look in vscode-extensions.txt for the extensions I use and may be needed to use this template.

---
``` prompt
create an application uses a bme680 connected to an ESP32. Use the documentation on the BME689 in /Volumes/Work01/Developer/ESP32Projects/Learn-Unity/Hardware to create a suitable device driver. Do not use a library.  It connects to a local network. It takes the temperature from the sensor and sends it to an MQTT server. It does this every 5 seconds. The development environment is a Mac 16 Gb running vsc with platformio and uses the espressif if framework. It does NOT use the arduino framework. Use the unity framework from espressif . Create separate files for each component such as temperature.c and temperature.h for temperature related functions. Add documentation suitable for doxygen documentation generation. Generate all the necessary files for a unity test environment. NOTE: USE THE main.cpp IN SRC FOR THE ENTRY POINT, NOT CREATE A NEW main.c
```
# ESP32 Project with Unity Testing Framework

This project uses the ESP-IDF framework with Unity testing integration for ESP32 development.

## Environment Setup

### ESP-IDF Version
This project is configured to use **ESP-IDF v5.4.0** (PlatformIO package version 3.50400.0). This version is selected for compatibility with Apple Silicon Macs.

```bash
# Command to show available ESP-IDF package information:
pio pkg show framework-espidf

# Command to check which ESP-IDF version is being used in your build:
pio run -e esp32doit-devkit-v1 -v | grep "framework-espidf"

# To check the version file directly:
cat ~/.platformio/packages/framework-espidf@3.50400.0/version.txt
```

### Apple Silicon (M1/M2/M3) Compatibility Note
If you're using an Apple Silicon Mac (darwin_arm64 architecture), the project is configured to use ESP-IDF version 5.4.0 (PlatformIO package 3.50400.0), which is compatible with the darwin_arm64 architecture.

When attempting to explicitly pin ESP-IDF versions in `platformio.ini` (e.g., using `platform_packages = framework-espidf @ ^5.4.0`), you may encounter build errors on Apple Silicon Macs. Instead, we recommend letting PlatformIO use its default compatible version.

To check the actual running ESP-IDF version (rather than just available packages), use:

```bash
# Look for the -DIDF_VER flag in the verbose build output
pio run -e esp32doit-devkit-v1 -v | grep DIDF_VER
```

