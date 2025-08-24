# ESP32 Testing Framework

**Author:** john.h.devine@gmail.com  
**Version:** 1.0.0  
**Date:** August 24, 2025

## Table of Contents

1. [Introduction](#introduction)
2. [Testing Environment](#testing-environment)
3. [Test Structure](#test-structure)
4. [Writing Tests](#writing-tests)
5. [Running Tests](#running-tests)
6. [Test Results](#test-results)
7. [Example Tests](#example-tests)
8. [Best Practices](#best-practices)
9. [Advanced Testing](#advanced-testing)
10. [Troubleshooting](#troubleshooting)

---

## Introduction

This document provides a comprehensive guide to the testing framework used in the ESP32 Weather Station project. Testing is an essential part of embedded systems development, helping to ensure code reliability, functionality, and quality.

### Why Testing Matters

- **Quality Assurance**: Identifies bugs and issues before deployment
- **Regression Prevention**: Ensures new features don't break existing functionality
- **Documentation**: Tests serve as functional specifications
- **Confidence**: Enables safe refactoring and continuous improvement

---

## Testing Environment

### Prerequisites

- **PlatformIO** installed and configured
- **ESP-IDF** framework (v5.4.0 or later)
- **Unity** test framework (included with PlatformIO)

### Tools Used

- **PlatformIO Test Runner**: Manages test execution
- **Unity**: C testing framework for assertions and test reporting
- **ESP-IDF Unity Extensions**: ESP32-specific test helpers

### Directory Structure

```
test/
├── README.md                 # This documentation
├── ESP32MetricsTest/         # Example test implementation
│   ├── CMakeLists.txt        # Build configuration for the test
│   ├── ESP32MetricsTest.c    # Test implementation
│   └── README.md             # Test-specific documentation
└── ...                       # Future test directories
```

---

## Test Structure

### Basic Test Architecture

The testing framework uses the following components:

1. **Test Runner**: The `app_main()` function that executes all tests
2. **Test Functions**: Individual test cases that verify specific functionality
3. **Assertions**: Validation points that verify expected behavior
4. **Test Results**: Summary of test execution outcomes

### Test Configuration

Each test module requires:

- A `CMakeLists.txt` file to define build settings
- A main C file with test cases and an `app_main()` function
- Optional README file explaining the test purpose and details

---

## Writing Tests

### Test Function Template

Each test function should follow this pattern:

```c
static bool test_function_name(void) {
    // 1. Setup test variables
    type variable = initial_value;
    
    // 2. Call the function being tested
    esp_err_t ret = function_to_test(&variable);
    
    // 3. Assert the results
    TEST_ASSERT(ret == ESP_OK, "function_to_test should return ESP_OK");
    TEST_ASSERT(variable == expected_value, "Variable should equal expected value");
    
    // 4. Log useful information
    ESP_LOGI(TAG, "Result: %d", variable);
    
    return true;  // Test passed
}
```

### Test Assertion Macro

```c
#define TEST_ASSERT(condition, message) do { \
    if (!(condition)) { \
        ESP_LOGE(TAG, "FAIL: %s (Line %d)", message, __LINE__); \
        tests_failed++; \
        return false; \
    } \
    return true; \
} while(0)
```

### Running a Test Function

```c
#define RUN_TEST(test_function, test_name) do { \
    ESP_LOGI(TAG, "Running test: %s", test_name); \
    tests_run++; \
    if (test_function()) { \
        ESP_LOGI(TAG, "PASS: %s", test_name); \
        tests_passed++; \
    } else { \
        ESP_LOGE(TAG, "FAIL: %s", test_name); \
    } \
    vTaskDelay(pdMS_TO_TICKS(100)); /* Small delay between tests */ \
} while(0)
```

---

## Running Tests

### Using PlatformIO

```bash
# Run all tests
pio test

# Run a specific test
pio test -e esp32doit-devkit-v1 -f ESP32MetricsTest

# Run tests with verbose output
pio test -v
```

### Using ESP-IDF Commands

```bash
# Navigate to the test directory
cd test/ESP32MetricsTest

# Build and flash
idf.py build flash monitor
```

### Automated Testing in CI

Tests can be integrated into continuous integration pipelines by adding:

```yaml
- name: Run PlatformIO Tests
  run: |
    pio test
```

---

## Test Results

### Console Output

Test execution produces console output showing individual test results and a summary:

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

### Exit Codes

- **0**: All tests passed
- **Non-zero**: One or more tests failed

### Common Test Outcomes

| Result | Meaning |
|--------|---------|
| `PASS` | All assertions succeeded |
| `FAIL` | One or more assertions failed |
| `ERROR` | Test could not be executed properly |

---

## Example Tests

### Basic Function Test

This example tests a simple initialization function:

```c
static bool test_init_deinit(void) {
    esp_err_t ret = ESP32Metrics_Init();
    TEST_ASSERT(ret == ESP_OK, "ESP32Metrics_Init should return ESP_OK");
    
    ret = ESP32Metrics_Deinit();
    TEST_ASSERT(ret == ESP_OK, "ESP32Metrics_Deinit should return ESP_OK");
    
    return true;
}
```

### Testing Return Values

```c
static bool test_version(void) {
    const char* version = ESP32Metrics_GetVersion();
    TEST_ASSERT(version != NULL, "Version string should not be NULL");
    TEST_ASSERT(strcmp(version, EXPECTED_VERSION) == 0, 
                "Version string should match expected value");
    
    return true;
}
```

### Testing Numeric Values

```c
static bool test_cpu_usage(void) {
    float usage = 0;
    esp_err_t ret = ESP32Metrics_GetCpuUsage(&usage);
    
    TEST_ASSERT(ret == ESP_OK, "ESP32Metrics_GetCpuUsage should return ESP_OK");
    TEST_ASSERT(usage >= 0.0f && usage <= 100.0f, 
                "CPU usage should be between 0 and 100");
    
    return true;
}
```

### Testing With Time Dependencies

```c
static bool test_uptime(void) {
    uint64_t uptime1 = 0;
    ESP32Metrics_GetUptime(&uptime1);
    
    // Wait and test again
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    uint64_t uptime2 = 0;
    ESP32Metrics_GetUptime(&uptime2);
    
    TEST_ASSERT(uptime2 > uptime1, "Uptime should increase over time");
    
    return true;
}
```

### Testing Optional Features

```c
static bool test_temperature(void) {
    float temp = 0;
    esp_err_t ret = ESP32Metrics_GetTemperature(&temp);
    
    if (ret == ESP_OK) {
        TEST_ASSERT(temp >= -10.0f && temp <= 80.0f, 
                    "Temperature should be in reasonable range");
    } else if (ret == ESP_ERR_NOT_SUPPORTED) {
        ESP_LOGW(TAG, "Temperature sensor not available on this device");
        // Not a failure, feature is optional
    } else {
        TEST_ASSERT(false, "Unexpected error from ESP32Metrics_GetTemperature");
    }
    
    return true;
}
```

---

## Best Practices

### Writing Effective Tests

1. **Single Responsibility**: Each test function should test one feature or behavior
2. **Independence**: Tests should not depend on each other
3. **Repeatability**: Tests should produce the same results on repeated runs
4. **Clear Naming**: Use descriptive test names that indicate what is being tested
5. **Good Assertions**: Make assertions specific with clear error messages
6. **Error Handling**: Test both success and failure cases

### Test Organization

1. **Group Related Tests**: Keep related tests in the same file
2. **Test Coverage**: Aim to test all functions and code paths
3. **Edge Cases**: Include tests for boundary conditions
4. **Error Cases**: Test how functions handle invalid inputs

### Logging

1. **Log Test Progress**: Use `ESP_LOGI` to show what is being tested
2. **Log Test Values**: Show relevant values to help with debugging
3. **Summarize Results**: Always include a summary of test results

---

## Advanced Testing

### Test Fixtures

For tests that need common setup and teardown:

```c
static void setUp(void) {
    // Initialize resources before each test
    ESP32Metrics_Init();
}

static void tearDown(void) {
    // Clean up resources after each test
    ESP32Metrics_Deinit();
}

// Then in your test runner:
setUp();
RUN_TEST(test_function, "Test Name");
tearDown();
```

### Mocking Hardware

For testing hardware-dependent code:

```c
// Create a mock implementation
static int8_t mock_wifi_rssi = -70;

esp_err_t mock_wifi_get_rssi(int8_t* rssi) {
    *rssi = mock_wifi_rssi;
    return ESP_OK;
}

// Use function pointers to replace real implementation
static bool test_with_mock(void) {
    // Save original function
    esp_err_t (*original_func)(int8_t*) = wifi_get_rssi_func;
    
    // Replace with mock
    wifi_get_rssi_func = mock_wifi_get_rssi;
    
    // Test with mock
    int8_t rssi;
    esp_err_t ret = ESP32Metrics_GetWifiSignal(&rssi);
    TEST_ASSERT(ret == ESP_OK && rssi == -70, "Should use mocked value");
    
    // Restore original function
    wifi_get_rssi_func = original_func;
    
    return true;
}
```

### Parameterized Tests

For testing with multiple inputs:

```c
typedef struct {
    const char* name;
    int input;
    int expected_output;
} test_case_t;

static bool run_parameterized_test(const test_case_t* test_case) {
    int result = function_under_test(test_case->input);
    TEST_ASSERT(result == test_case->expected_output, 
                "Test failed for input");
    return true;
}

// In your test runner:
test_case_t test_cases[] = {
    {"Zero case", 0, 0},
    {"Positive case", 5, 10},
    {"Negative case", -5, -10},
    {"Max case", INT_MAX, INT_MAX}
};

for (int i = 0; i < sizeof(test_cases) / sizeof(test_case_t); i++) {
    RUN_TEST_CASE(run_parameterized_test, &test_cases[i], test_cases[i].name);
}
```

---

## Troubleshooting

### Common Issues

1. **Build Errors**:
   - Check that the `CMakeLists.txt` includes all required components
   - Verify that test files are included in the build

2. **Flash Errors**:
   - Ensure ESP32 is connected and in bootloader mode
   - Check USB connection and port settings

3. **Runtime Errors**:
   - Monitor console output for exceptions
   - Check for stack overflows in test functions

4. **Test Failures**:
   - Review test assertions and expected values
   - Check if hardware features being tested are available

### Debugging Tests

1. **Enable Verbose Logging**:
   - Set `CONFIG_LOG_DEFAULT_LEVEL=5` for verbose output
   - Add more logging in test functions

2. **Using GDB**:
   - Configure GDB in platformio.ini:
     ```ini
     debug_tool = esp-prog
     debug_init_break = tbreak app_main
     ```
   - Run `pio debug` to start debugging

3. **Run Tests in Isolation**:
   - Test individual functions using `RUN_TEST` directly
   - Comment out other tests to isolate issues

---

_This document was last updated on August 24, 2025_
