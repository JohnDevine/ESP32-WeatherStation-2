# Best Practices for C Coding in Embedded Applications  
*(ESP-IDF on ESP32 with PlatformIO in VS Code)*

## Author Information

- **Author:** John Devine
- **Email:** john.h.devine@gmail.com
- **Copyright:** Always include this author information in file headers, particularly for Doxygen documentation.

---

## Development Environment

### Hardware
- **Primary Development Machine:** Mac mini M2 with 16GB memory and 512GB SSD
- **Mobile Development:** iPad M2 13 inch with Apple Pencil

### Software
- **IDE:** Visual Studio Code
- **Key Extensions:**
  - PlatformIO IDE (platformio.platformio-ide)
  - C/C++ Tools (ms-vscode.cpptools, ms-vscode.cpptools-extension-pack)
  - Build Tools (ms-vscode.cmake-tools, ms-vscode.makefile-tools)
  - Documentation (betwo.vscode-doxygen-runner)
  - AI Assistance (github.copilot, github.copilot-chat, chrisdias.promptboost)
  - Version Control (eamodio.gitlens)
  - Containers (ms-azuretools.vscode-docker)
  - Theme: Night Owl (sdras.night-owl)
  
### Versioning
- Always update version information in `include/version.h`
- Follow semantic versioning (MAJOR.MINOR.PATCH)
- Use scripts/version_manager.py for version management

---

## 1. Project & File Structure

- **Use PlatformIO’s default structure**:
  ```
  ├── include/        # Header files (.h)
  ├── src/            # Source files (.c)
  ├── components/     # Optional reusable libraries
  ├── test/           # Unit tests (Unity, Ceedling, etc.)
  ├── platformio.ini  # Build configuration
  ```

- **Use main.c as the main application file**
  - main.c follows standard C conventions for ESP-IDF.
  - ESP-IDF is C-oriented, so we use standard C code instead of C++.

- **Split code logically**:
  - `main.c` should only contain entry-point logic (`app_main()`).
  - Move functionality into separate `.c/.h` files under `src/` or `components/`.
  - give devices like sensors their own class

- **Headers in `/include`** should only expose **public APIs**, not internal details.

---

## 2. Function Headers at Top of File

Always **declare function prototypes** at the top of each `.c` file before `app_main()` or other code.  
This improves readability and avoids implicit declarations.

```c
// =============================
// Function Prototypes
// =============================
static void init_gpio(void);
static void wifi_connect(void);
static void handle_uart_rx(void);

// =============================
// Includes
// =============================
#include <stdio.h>
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_wifi.h"

// =============================
// Function Definitions
// =============================
static void init_gpio(void) {
    // Setup GPIOs
}

static void wifi_connect(void) {
    // Connect WiFi
}

static void handle_uart_rx(void) {
    // UART RX handling
}
```

---

## 3. Coding Style

- **Follow Espressif IDF style** (based on LLVM/Google C style):
  - 4 spaces per indent (no tabs).
  - Function names: `snake_case`.
  - Constants & macros: `UPPER_CASE`.
  - Structs/enums: `PascalCase`.
  - Keep functions short (< 50 lines when possible).
- **Comment every function** with Doxygen-style headers, including author information:

```c
/**
 * @file example.c
 * @brief Example file with documentation
 * 
 * @author john.h.devine@gmail.com
 * @date August 22, 2025
 */

/**
 * @brief Initialize GPIO pins for LED and Button
 *
 * @param led_pin GPIO number for LED
 * @param button_pin GPIO number for Button
 * @return void
 */
void init_gpio(int led_pin, int button_pin);
```

---

## 4. PlatformIO Configuration (`platformio.ini`)

- Always **pin ESP-IDF version** for reproducibility:

```ini
[env:esp32-devkit]
platform = espressif32
board = esp32dev
framework = espidf
platform_packages = 
    framework-espidf @ ^5.3.0   ; or ^5.5.0 if stable
monitor_speed = 115200
monitor_filters = esp32_exception_decoder
build_flags = 
    -Wall
    -Wextra
    -Werror
    -DDEBUG
```

- Enable **warnings-as-errors** (`-Werror`) to enforce clean builds.
- Use **exception decoder** for crash debugging in VS Code terminal.

---

## 5. Debugging & Logging

- Use **ESP-IDF logging library** (`ESP_LOGI`, `ESP_LOGW`, `ESP_LOGE`) instead of `printf`.
- Define logging tags per module:

```c
static const char *TAG = "WIFI";

ESP_LOGI(TAG, "Connecting to WiFi...");
ESP_LOGE(TAG, "Failed to connect, error=%d", err);
```

- Adjust log level in `menuconfig` or `sdkconfig.defaults`.

---

## 6. Memory & Performance

- Use **static allocation** where possible (avoid malloc in real-time tasks).
- Prefer **FreeRTOS queues, semaphores, and tasks** over busy-wait loops.
- Keep ISR routines minimal — offload work to tasks.
- Use `ESP_ERROR_CHECK()` for all IDF API calls:

```c
ESP_ERROR_CHECK(esp_wifi_start());
```

---

## 7. Testing & Simulation

- Write **unit tests** in `/test` using Unity framework.
- Mock hardware drivers for host-based testing where possible.
- Use `pio test` for automated testing.

---

## 8. Git & CI Integration

- Add `.gitignore` with:
  ```
  .pio/
  .vscode/
  sdkconfig
  sdkconfig.old
  ```
- Use **CI pipelines** (GitHub Actions, GitLab CI) to:
  - Build firmware
  - Run unit tests
  - Check coding style (`clang-format`)

---

## 9. VS Code Productivity

- Install extensions:
  - `PlatformIO IDE`
  - `C/C++` (IntelliSense, formatting)
  - `CMake Tools` (optional for native IDF builds)
- Use **code snippets** for function headers, logging, FreeRTOS tasks.

---

## 10. Summary Checklist

✅ Headers at top of `.c` file  
✅ Small, well-commented functions  
✅ Pin ESP-IDF version in `platformio.ini`  
✅ Use `ESP_LOGx` instead of `printf`  
✅ Check all API calls with `ESP_ERROR_CHECK`  
✅ Use static memory allocation where possible  
✅ Write unit tests for critical logic  
✅ Enforce clean builds with `-Wall -Wextra -Werror`  

---
