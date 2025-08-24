# My ESP32 Development Environment

**Author:** john.h.devine@gmail.com  
**Date:** August 24, 2025

## Table of Contents

- [Hardware Environment](#hardware-environment)
- [Software Environment](#software-environment)
- [VS Code Extensions](#vs-code-extensions)
- [Project Structure](#project-structure)
- [GitHub Copilot Setup](#github-copilot-setup)
- [Versioning System](#versioning-system)
- [Testing Framework](#testing-framework)
- [Documentation Tools](#documentation-tools)
- [Local History](#local-history)

---

## Hardware Environment

### Primary Development Machine
- **Model:** Mac mini M2
- **Memory:** 16GB RAM
- **Storage:** 512GB SSD
- **Benefits:**
  - Excellent compilation speed
  - Sufficient memory for ESP-IDF toolchain and VS Code
  - Fast SSD for quick build times

### Mobile Development
- **Device:** iPad M2 13 inch
- **Accessories:** Apple Pencil
- **Uses:**
  - Review documentation on-the-go
  - Sketch hardware diagrams
  - Remote connection to development machine
  - Note-taking during debugging sessions
  - OTA platform for updating ESP32 firmware in the field
  - Updating ESP32 configuration via captive portal (triggered by pressing the boot button within 10 seconds of device startup)

---

## Software Environment

### Primary IDE
- **VS Code** with PlatformIO integration
- **Terminal:** zsh
- **Build System:** PlatformIO (with ESP-IDF framework)

### Other Development Tools
- **Version Control:** Git/GitHub
- **Documentation:** Doxygen
- **Local Document Server:** Custom Markdown server (`md_server.py`)

---

## VS Code Extensions

The development environment relies on several key VS Code extensions, which are listed in [MyExtensions.txt](./MyExtensions.txt). Key extensions include:

### Essential Extensions
- **PlatformIO IDE** (`platformio.platformio-ide`): ESP32 development platform
- **C/C++ Tools** (`ms-vscode.cpptools`, `ms-vscode.cpptools-extension-pack`): Code intelligence
- **Build Tools** (`ms-vscode.cmake-tools`, `ms-vscode.makefile-tools`): Build system integration
- **Documentation** (`betwo.vscode-doxygen-runner`): Generate documentation

### Productivity Enhancers
- **AI Assistance** (`github.copilot`, `github.copilot-chat`, `chrisdias.promptboost`): AI code generation
- **Version Control** (`eamodio.gitlens`): Enhanced Git integration
- **Local History** (`xyz.local-history`): Track file changes

### Container Support
- **Docker** (`ms-azuretools.vscode-docker`, `ms-azuretools.vscode-containers`): Container support

### Visual Customization
- **Theme:** Night Owl (`sdras.night-owl`)

---

## Project Structure

The project follows a standard ESP32 development structure with several key additions:

### Standard PlatformIO Structure
- `include/` - Header files (.h)
- `src/` - Source files (.c)
- `lib/` - Libraries
- `test/` - Testing framework

### Custom Directories
- **MyProgrammingSetup/** - Development environment configuration
  - Contains IDE settings and extensions list
  - Documentation server (`DocumentServer/md_server.py`)
- **prompts/** - GitHub Copilot prompt templates
  - Contains example prompts and boost templates
  - `boost.copilot.txt` is used as a template for most prompts
- **.history/** - Automatic local file history (created by xyz.local-history extension)
  - Preserves file versions beyond Git commits
  - Useful for recovering from problematic changes
  - Critical backup when Copilot makes errors that Keep/Discard can't fix
- **Hardware/** - Hardware documentation
  - BME680 sensor specifications
  - ESP32 metrics and documentation
  - Reference material and datasheets
- **firmware/** - Built firmware files and deployment scripts
  - Latest compiled firmware binaries
  - Deployment utilities
  - `copy_firmware.py` script for firmware distribution

---

## GitHub Copilot Setup

This project has a customized GitHub Copilot configuration that enhances code generation for ESP32 development:

### Configuration Files
- **copilot.context.json** - JSON configuration with project settings
  - Defines coding style guidelines
  - Specifies project structure
  - Includes hardware/software environment details
- **.github/copilot-instructions.md** - Detailed programming instructions
  - Provides best practices for ESP32/ESP-IDF development
  - Sets conventions for documentation and error handling
  - Guides code generation toward project standards

### Prompt Templates
The `prompts/` directory contains templates for common code generation tasks:
- **boost.copilot.txt** - Enhances prompts before execution
- **example.copilot.txt** - Sample prompts for common tasks

> **Important:** Always review the context files (`copilot.context.json` and `.github/copilot-instructions.md`) when starting a new project or when Copilot is generating inconsistent code.

---

## Versioning System

The project implements a comprehensive versioning system described in [README.VERSIONING.md](../README.VERSIONING.md).

### Key Components
- **include/version.h** - Central version definitions
- **src/version.c** - Version information implementation
- **scripts/version_manager.py** - Version management utility

### Version Types
- **Project Version** - Overall firmware version (MAJOR.MINOR.PATCH)
- **File Versions** - Individual versions for each source file
- **Version Registry** - Centralized collection of all versions

### Usage
- All versions are visible to the linker
- Versions can be displayed at runtime for diagnostics
- File versions only change when specific files are modified
- Project version follows semantic versioning principles

> **Note:** Always update version information when making significant changes. Use the version management script to maintain consistency.

---

## Testing Framework

The project uses the PlatformIO Test Runner with Unity framework for unit testing, as detailed in [README.TESTING.md](../README.TESTING.md).

### Test Structure
- **test/** - Root directory for all test code
- **test/ESP32MetricsTest/** - Example test implementation
  - Contains test cases for ESP32Metrics library
  - Includes setup, test cases, and reporting

### Running Tests
```bash
# Run all tests
pio test

# Run specific test
pio test -e esp32dev -f ESP32MetricsTest
```

### Test Features
- Unit tests for critical functionality
- Automated test execution in CI/CD pipeline
- Hardware mocking for host-based testing
- Test result reporting

---

## Documentation Tools

The project includes several documentation tools:

### Doxygen
- Generates code documentation from source comments
- Configured in `Doxyfile`
- Run using the Doxygen Runner extension

### Markdown Document Server
- Custom server for viewing Markdown files
- Run using:
  ```bash
  ./MyProgrammingSetup/RunDocumentServer.sh
  ```
- Automatically formats and displays project documentation

### Hardware Documentation
- The `Hardware/` directory contains specifications and datasheets
- Includes ESP32 metrics and sensor information

---

## Local History

The `.history/` folder is maintained by the `xyz.local-history` extension:

### Benefits
- **Automatic Versioning:** Creates a new version every time you save a file
- **Recovery:** Recover from mistakes even if you didn't commit to Git
- **Safety Net:** Particularly valuable when experimenting with Copilot-generated code
- **Beyond Git:** Preserves changes that would be lost between commits

### Usage
- Right-click on a file > Local History > Compare with Local History
- Select a previous version to compare or restore
- Versions are organized by date and time

> **Pro Tip:** The Local History feature has saved countless hours when Copilot makes significant errors that the Keep/Discard functionality doesn't adequately address.

---

_This README was last updated on August 24, 2025_
