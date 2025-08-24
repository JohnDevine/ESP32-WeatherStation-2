# ESP32Metrics Test

This directory contains test code for the ESP32Metrics library. The test application verifies all metrics functionality and reports test results via the serial console.

## Author

john.h.devine@gmail.com

## Version

1.0.0 - August 22, 2025

## Features Tested

- Initialization and deinitialization
- Version information
- CPU usage measurement
- System uptime
- WiFi signal strength
- WiFi connection status
- WiFi data rate
- Internal temperature
- Brownout event counting
- Reboot reason reporting
- Error counting
- System log level

## Running Tests

To run the tests:

1. Connect your ESP32 device
2. Flash this test application
3. Monitor the serial output
4. Review test results

## Test Results

The test application will output a summary of tests run, passed, and failed.

Example output:

```
I (5042) ESP32METRICS_TEST: ===================================
I (5042) ESP32METRICS_TEST: ESP32Metrics Test Summary
I (5052) ESP32METRICS_TEST: -----------------------------------
I (5052) ESP32METRICS_TEST: Total tests:  10
I (5062) ESP32METRICS_TEST: Tests passed: 10
I (5062) ESP32METRICS_TEST: Tests failed: 0
I (5072) ESP32METRICS_TEST: Success rate: 100.0%
I (5072) ESP32METRICS_TEST: ===================================
I (5082) ESP32METRICS_TEST: ALL TESTS PASSED
```

## Notes

- Some tests may show warnings if certain hardware features are not available
- WiFi tests may fail if WiFi is not connected
- Temperature sensor tests may fail if the device does not have a temperature sensor
