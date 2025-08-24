# ESP32 Weather Station Testing Documentation

## Overview

This project uses a comprehensive testing framework for ensuring code reliability and quality.

For complete documentation on the testing system, please refer to the detailed guide:

**[→ Comprehensive Testing Documentation](test/README.md)**

## Quick Links

- [Test Directory](test/)
- [ESP32MetricsTest Example](test/ESP32MetricsTest/)
- [PlatformIO Test Documentation](https://docs.platformio.org/en/latest/advanced/unit-testing/index.html)

## Basic Test Commands

```bash
# Run all tests
pio test

# Run a specific test
pio test -e esp32doit-devkit-v1 -f ESP32MetricsTest

# Run tests with verbose output
pio test -v
```

---

*This file serves as a pointer to the main testing documentation in the test directory.*
