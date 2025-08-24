# Use this command to create a new project from scratch

1. Change the name "Learn-Unity" to the name of your project.
2. It will create the project in /Volumes/Work01/Developer/ESP32Projects.
3. It requires the template project (this project) in /Volumes/Work01/Developer/ESP32Projects/Template
4. Copy and paste the below to a github agent mode prompt to run. (DO NOT RUN BEFORE doing 5. below!!!!!)
5. Change "Learn-Unity" to your project name.
6. Run the prompt in copilot.
7. If you are going to use doxygen check out the Doxyfile and mod it to suit your project
8. look in vscode-extensions.txt for the extensions I use and may be needed to use this template.

---
``` prompt
Create me a new project. It is a vsc project that uses platformio. I want it to start as a github repository called Learn-Unity. Create the repository and clone it to /Volumes/Work01/Developer/ESP32Projects. Then create a new platformio project over the top of this cloned project so that it has both git initialisation and platform initialisation.  The copy the contents of the folder /Volumes/Work01/Developer/ESP32Projects/Template (NOTE: CONTENTS NOT THE FOLDER ITSELF)  over the folder where GitHub repository was cloned. First, boost this prompt with the prompt boost tool> then show me what you propose doing. Do not do anything yet.
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

