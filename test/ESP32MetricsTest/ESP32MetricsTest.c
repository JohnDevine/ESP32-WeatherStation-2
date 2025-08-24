/**
 * @file ESP32MetricsTest.c
 * @brief Test application for ESP32Metrics library
 * 
 * This file contains a comprehensive test suite for the ESP32Metrics library,
 * testing each metric type and the API functionality.
 *
 * @version 1.0.0
 * @date August 22, 2025
 * @author john.h.devine@gmail.com
 * 
 * Copyright (c) 2025 John Devine
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "ESP32Metrics.h"
#include "version.h"

// Define version information for this test application
#define ESP32METRICS_TEST_VERSION_MAJOR 1
#define ESP32METRICS_TEST_VERSION_MINOR 0
#define ESP32METRICS_TEST_VERSION_PATCH 0
#define ESP32METRICS_TEST_VERSION "1.0.0"

// Register test application version
REGISTER_VERSION(ESP32MetricsTest, ESP32METRICS_TEST_VERSION, __DATE__);

static const char* TAG = "ESP32METRICS_TEST";

// Test result counters
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

// Test helper macros
#define TEST_ASSERT(condition, message) do { \
    if (!(condition)) { \
        ESP_LOGE(TAG, "FAIL: %s (Line %d)", message, __LINE__); \
        tests_failed++; \
        return false; \
    } \
    return true; \
} while(0)

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

// Individual test functions
static bool test_init_deinit(void) {
    esp_err_t ret = ESP32Metrics_Init();
    TEST_ASSERT(ret == ESP_OK, "ESP32Metrics_Init should return ESP_OK");
    
    ret = ESP32Metrics_Deinit();
    TEST_ASSERT(ret == ESP_OK, "ESP32Metrics_Deinit should return ESP_OK");
    
    // Re-initialize for subsequent tests
    ret = ESP32Metrics_Init();
    TEST_ASSERT(ret == ESP_OK, "ESP32Metrics_Init (2nd time) should return ESP_OK");
    
    return true;
}

static bool test_version(void) {
    const char* version = ESP32Metrics_GetVersion();
    TEST_ASSERT(version != NULL, "Version string should not be NULL");
    TEST_ASSERT(strcmp(version, ESP32METRICS_VERSION) == 0, 
                "Version string should match ESP32METRICS_VERSION");
    
    ESP_LOGI(TAG, "ESP32Metrics version: %s", version);
    
    return true;
}

static bool test_cpu_usage(void) {
    float usage = 0;
    esp_err_t ret = ESP32Metrics_GetCpuUsage(&usage);
    
    TEST_ASSERT(ret == ESP_OK, "ESP32Metrics_GetCpuUsage should return ESP_OK");
    TEST_ASSERT(usage >= 0.0f && usage <= 100.0f, 
                "CPU usage should be between 0 and 100");
    
    ESP_LOGI(TAG, "CPU Usage: %.1f%%", usage);
    
    // Test invalid parameter
    ret = ESP32Metrics_GetCpuUsage(NULL);
    TEST_ASSERT(ret == ESP_ERR_INVALID_ARG, 
                "Should return ESP_ERR_INVALID_ARG with NULL parameter");
    
    return true;
}

static bool test_uptime(void) {
    uint64_t uptime = 0;
    esp_err_t ret = ESP32Metrics_GetUptime(&uptime);
    
    TEST_ASSERT(ret == ESP_OK, "ESP32Metrics_GetUptime should return ESP_OK");
    TEST_ASSERT(uptime > 0, "Uptime should be greater than 0");
    
    ESP_LOGI(TAG, "Uptime: %llu ms", uptime);
    
    // Test again after a delay to verify it's increasing
    vTaskDelay(pdMS_TO_TICKS(1000));
    uint64_t uptime2 = 0;
    ret = ESP32Metrics_GetUptime(&uptime2);
    
    TEST_ASSERT(ret == ESP_OK, "Second call to ESP32Metrics_GetUptime should return ESP_OK");
    TEST_ASSERT(uptime2 > uptime, "Uptime should increase over time");
    ESP_LOGI(TAG, "Uptime after delay: %llu ms (delta: %llu ms)", 
             uptime2, uptime2 - uptime);
    
    return true;
}

static bool test_wifi_metrics(void) {
    // This test may fail if WiFi is not connected - that's OK for testing
    int8_t rssi = 0;
    ESP32WifiStatus status;
    float data_rate = 0;
    
    // Test WiFi status
    esp_err_t ret = ESP32Metrics_GetWifiStatus(&status);
    TEST_ASSERT(ret == ESP_OK, "ESP32Metrics_GetWifiStatus should return ESP_OK");
    
    const char* status_str;
    switch (status) {
        case ESP32_WIFI_CONNECTED:     status_str = "Connected"; break;
        case ESP32_WIFI_DISCONNECTED:  status_str = "Disconnected"; break;
        case ESP32_WIFI_CONNECTING:    status_str = "Connecting"; break;
        case ESP32_WIFI_DISCONNECTING: status_str = "Disconnecting"; break;
        default:                       status_str = "Not initialized"; break;
    }
    ESP_LOGI(TAG, "WiFi Status: %s", status_str);
    
    // Test RSSI - may return error if not connected
    ret = ESP32Metrics_GetWifiSignal(&rssi);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "WiFi Signal Strength: %d dBm", rssi);
        TEST_ASSERT(rssi < 0, "RSSI should be negative when connected");
    } else {
        ESP_LOGW(TAG, "WiFi Signal Strength: Not available (not connected)");
    }
    
    // Test data rate - may return error if not connected
    ret = ESP32Metrics_GetWifiDataRate(&data_rate);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "WiFi Data Rate: %.1f Mbps", data_rate);
        TEST_ASSERT(data_rate >= 0, "Data rate should be non-negative");
    } else {
        ESP_LOGW(TAG, "WiFi Data Rate: Not available (not connected)");
    }
    
    return true;
}

static bool test_temperature(void) {
    float temp = 0;
    esp_err_t ret = ESP32Metrics_GetTemperature(&temp);
    
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Internal Temperature: %.1f °C", temp);
        TEST_ASSERT(!isnan(temp), "Temperature should not be NaN");
        TEST_ASSERT(temp >= -10.0f && temp <= 80.0f, 
                    "Temperature should be in reasonable range");
    } else if (ret == ESP_ERR_NOT_SUPPORTED) {
        ESP_LOGW(TAG, "Temperature sensor not available on this device");
    } else {
        TEST_ASSERT(false, "Unexpected error from ESP32Metrics_GetTemperature");
    }
    
    return true;
}

static bool test_reboot_reason(void) {
    ESP32RebootReason reason;
    esp_err_t ret = ESP32Metrics_GetRebootReason(&reason);
    
    TEST_ASSERT(ret == ESP_OK, "ESP32Metrics_GetRebootReason should return ESP_OK");
    TEST_ASSERT(reason.reason != NULL, "Reboot reason string should not be NULL");
    
    ESP_LOGI(TAG, "Reboot Reason: %s (code: %d)", reason.reason, reason.code);
    
    return true;
}

static bool test_brownout_and_errors(void) {
    uint32_t brownout_count = 0;
    uint32_t error_count = 0;
    
    esp_err_t ret = ESP32Metrics_GetBrownoutCount(&brownout_count);
    TEST_ASSERT(ret == ESP_OK, "ESP32Metrics_GetBrownoutCount should return ESP_OK");
    ESP_LOGI(TAG, "Brownout Count: %u", brownout_count);
    
    ret = ESP32Metrics_GetErrorCount(&error_count);
    TEST_ASSERT(ret == ESP_OK, "ESP32Metrics_GetErrorCount should return ESP_OK");
    ESP_LOGI(TAG, "Initial Error Count: %u", error_count);
    
    // Test error count increment
    ret = ESP32Metrics_IncrementErrorCount();
    TEST_ASSERT(ret == ESP_OK, "ESP32Metrics_IncrementErrorCount should return ESP_OK");
    
    uint32_t new_error_count = 0;
    ret = ESP32Metrics_GetErrorCount(&new_error_count);
    TEST_ASSERT(ret == ESP_OK, "ESP32Metrics_GetErrorCount should return ESP_OK");
    TEST_ASSERT(new_error_count == error_count + 1, 
                "Error count should increment by 1");
    
    ESP_LOGI(TAG, "Error Count after increment: %u", new_error_count);
    
    return true;
}

static bool test_log_level(void) {
    esp_log_level_t level;
    esp_err_t ret = ESP32Metrics_GetLogLevel(&level);
    
    TEST_ASSERT(ret == ESP_OK, "ESP32Metrics_GetLogLevel should return ESP_OK");
    
    const char* level_str;
    switch (level) {
        case ESP_LOG_NONE:   level_str = "NONE"; break;
        case ESP_LOG_ERROR:  level_str = "ERROR"; break;
        case ESP_LOG_WARN:   level_str = "WARN"; break;
        case ESP_LOG_INFO:   level_str = "INFO"; break;
        case ESP_LOG_DEBUG:  level_str = "DEBUG"; break;
        case ESP_LOG_VERBOSE:level_str = "VERBOSE"; break;
        default:             level_str = "UNKNOWN"; break;
    }
    
    ESP_LOGI(TAG, "System Log Level: %s (%d)", level_str, level);
    
    return true;
}

static bool test_generic_api(void) {
    // Test the generic API for various metrics
    float cpu_usage = 0;
    uint64_t uptime = 0;
    int8_t rssi = 0;
    
    esp_err_t ret = ESP32Metrics_GetMetric(ESP32_METRIC_CPU_USAGE, &cpu_usage);
    TEST_ASSERT(ret == ESP_OK, "GetMetric(CPU_USAGE) should return ESP_OK");
    ESP_LOGI(TAG, "[Generic API] CPU Usage: %.1f%%", cpu_usage);
    
    ret = ESP32Metrics_GetMetric(ESP32_METRIC_UPTIME, &uptime);
    TEST_ASSERT(ret == ESP_OK, "GetMetric(UPTIME) should return ESP_OK");
    ESP_LOGI(TAG, "[Generic API] Uptime: %llu ms", uptime);
    
    // Test invalid metric type
    ret = ESP32Metrics_GetMetric(99, &uptime);
    TEST_ASSERT(ret == ESP_ERR_INVALID_ARG, 
                "GetMetric with invalid type should return ESP_ERR_INVALID_ARG");
    
    return true;
}

void print_test_summary(void) {
    ESP_LOGI(TAG, "===================================");
    ESP_LOGI(TAG, "ESP32Metrics Test Summary");
    ESP_LOGI(TAG, "-----------------------------------");
    ESP_LOGI(TAG, "Total tests:  %d", tests_run);
    ESP_LOGI(TAG, "Tests passed: %d", tests_passed);
    ESP_LOGI(TAG, "Tests failed: %d", tests_failed);
    ESP_LOGI(TAG, "Success rate: %.1f%%", 
             (tests_run > 0) ? ((float)tests_passed * 100.0f / tests_run) : 0);
    ESP_LOGI(TAG, "===================================");
    
    // Final result
    if (tests_failed == 0) {
        ESP_LOGI(TAG, "ALL TESTS PASSED");
    } else {
        ESP_LOGE(TAG, "SOME TESTS FAILED");
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "===================================");
    ESP_LOGI(TAG, "Starting ESP32Metrics Test v%s", ESP32METRICS_TEST_VERSION);
    ESP_LOGI(TAG, "Author: john.h.devine@gmail.com");
    ESP_LOGI(TAG, "Date: %s", __DATE__);
    ESP_LOGI(TAG, "===================================");
    
    // Allow console to catch up
    vTaskDelay(pdMS_TO_TICKS(500));
    
    // Run tests
    RUN_TEST(test_init_deinit, "Initialization & Deinitialization");
    RUN_TEST(test_version, "Version Information");
    RUN_TEST(test_cpu_usage, "CPU Usage");
    RUN_TEST(test_uptime, "System Uptime");
    RUN_TEST(test_wifi_metrics, "WiFi Metrics");
    RUN_TEST(test_temperature, "Temperature");
    RUN_TEST(test_reboot_reason, "Reboot Reason");
    RUN_TEST(test_brownout_and_errors, "Brownout & Error Counts");
    RUN_TEST(test_log_level, "Log Level");
    RUN_TEST(test_generic_api, "Generic API");
    
    // Print test summary
    print_test_summary();
    
    // Clean up
    ESP32Metrics_Deinit();
    
    ESP_LOGI(TAG, "Test complete. System will continue running...");
}
