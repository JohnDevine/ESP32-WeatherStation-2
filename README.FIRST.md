# Creating a New ESP32 Project from This Template

This guide walks you through creating a new ESP32 project using this template with GitHub Copilot.

## Prerequisites

1. GitHub account with repository creation permissions
2. VSCode with GitHub Copilot extension installed
3. PlatformIO extension installed in VSCode
4. This template project located at `/Volumes/Work01/Developer/ESP32Projects/ESP32-BaseTemplate`

## Step-by-Step Instructions

1. **Replace "PROJECT_NAME" and "YOUR_USERNAME" in the prompt below** with your desired project name
2. Copy the entire prompt and paste it to GitHub Copilot in **Agent mode**
3. Follow the instructions that Copilot provides

## Important Post-Creation Steps

1. **Make an initial commit** after project creation to avoid build errors
2. Review `Doxyfile` if you plan to use Doxygen for documentation
3. Check `vscode-extensions.txt` for recommended VSCode extensions
4. See example prompts in your new project's `/prompts` directory

## Project Creation Prompt

Copy and paste the text below into GitHub Copilot **Agent mode** (not chat mode), after replacing PROJECT_NAME and YOUR_USERNAME with your actual project name and github user name:

```
I need assistance creating a new VSCode project with PlatformIO for an ESP32 device. Please follow these steps carefully:

1. Create a new GitHub repository:
   - Repository name: PROJECT_NAME
   - Make it public
   - Initialize with a README

2. Clone the repository locally:
   git clone https://github.com/YOUR_USERNAME/PROJECT_NAME.git /Volumes/Work01/Developer/ESP32Projects/PROJECT_NAME

3. Initialize as a PlatformIO project:
   - Create a new PlatformIO project in the same directory
   - Board: ESP32 DOIT DevKit V1
   - Framework: ESP-IDF

4. Copy template files (excluding .git):
   rsync -av --exclude='.git' /Volumes/Work01/Developer/ESP32Projects/ESP32-BaseTemplate/ /Volumes/Work01/Developer/ESP32Projects/PROJECT_NAME/

5. Update project name in platformio.ini:
   sed -i '' 's/-D PROJECT_NAME=\"${PIOENV}\"/-D PROJECT_NAME=\"PROJECT_NAME\"/g' /Volumes/Work01/Developer/ESP32Projects/PROJECT_NAME/platformio.ini

6. Update project name in CMakeLists.txt:
   sed -i '' 's/project(Template)/project(PROJECT_NAME)/g' /Volumes/Work01/Developer/ESP32Projects/PROJECT_NAME/CMakeLists.txt

7. Commit and push changes:
   cd /Volumes/Work01/Developer/ESP32Projects/PROJECT_NAME
   git add .
   git commit -m "Initial Commit"
   git push origin main

Please help me implement this project setup according to these steps. First, analyze the approach and let me know what you'll do before proceeding.
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

