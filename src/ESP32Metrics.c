/**
 * @file ESP32Metrics.c
 * @brief Implementation of ESP32Metrics class
 * 
 * This file implements the ESP32Metrics class for gathering system metrics
 * from an ESP32 processor.
 *
 * @version 1.0.0
 * @date August 22, 2025
 */

#include "ESP32Metrics.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "esp_sleep.h"
#include "driver/temp_sensor.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <math.h>

// Static instance of the metrics structure
static ESP32Metrics_t s_metrics = {0};
static const char* TAG = "ESP32Metrics";

// Static mapping of reset reasons to strings
static const char* s_reset_reason_strings[] = {
    "Unknown",                  // ESP_RST_UNKNOWN
    "Power-on reset",           // ESP_RST_POWERON
    "External pin reset",       // ESP_RST_EXT
    "Software reset",           // ESP_RST_SW
    "Watchdog reset",           // ESP_RST_PANIC
    "Interrupt watchdog reset", // ESP_RST_INT_WDT
    "Task watchdog reset",      // ESP_RST_TASK_WDT
    "Other watchdog reset",     // ESP_RST_WDT
    "Brownout reset",           // ESP_RST_BROWNOUT
    "SDIO reset",               // ESP_RST_SDIO
    "Deepsleep reset",          // ESP_RST_DEEPSLEEP
    "Bootstrapping reset"       // ESP_RST_BOOTLOADER
};

// Static function prototypes
static void update_reboot_reason(void);

esp_err_t ESP32Metrics_Init(void) {
    if (s_metrics.is_initialized) {
        return ESP_OK; // Already initialized
    }

    // Initialize counters
    s_metrics.brownout_count = 0;
    s_metrics.error_count = 0;
    
    // Get initial uptime
    s_metrics.last_uptime = esp_timer_get_time() / 1000; // Convert to ms
    
    // Initialize temperature sensor if available
    esp_err_t ret = temp_sensor_config_t config = TEMPERAUTRE_SENSOR_CONFIG_DEFAULT(-10, 80);
    if (ret == ESP_OK) {
        ret = temp_sensor_start();
        if (ret == ESP_OK) {
            s_metrics.temp_sensor_enabled = true;
            temp_sensor_read_celsius(&s_metrics.last_temperature);
        } else {
            s_metrics.temp_sensor_enabled = false;
            s_metrics.last_temperature = NAN;
            ESP_LOGW(TAG, "Failed to start temperature sensor: %d", ret);
        }
    } else {
        s_metrics.temp_sensor_enabled = false;
        s_metrics.last_temperature = NAN;
        ESP_LOGW(TAG, "Failed to configure temperature sensor: %d", ret);
    }
    
    // Check for brownout as the reset reason
    update_reboot_reason();
    if (s_metrics.reboot_reason.code == ESP_RST_BROWNOUT) {
        s_metrics.brownout_count = 1;
    }
    
    // Initialize CPU usage measurement
    s_metrics.prev_total_run_time = 0;
    s_metrics.prev_idle_time = 0;
    
    s_metrics.is_initialized = true;
    
    ESP_LOGI(TAG, "ESP32Metrics initialized (v%s)", ESP32METRICS_VERSION);
    return ESP_OK;
}

esp_err_t ESP32Metrics_Deinit(void) {
    if (!s_metrics.is_initialized) {
        return ESP_OK; // Already deinitialized
    }
    
    // Disable temperature sensor if it was enabled
    if (s_metrics.temp_sensor_enabled) {
        temp_sensor_stop();
        s_metrics.temp_sensor_enabled = false;
    }
    
    s_metrics.is_initialized = false;
    ESP_LOGI(TAG, "ESP32Metrics deinitialized");
    return ESP_OK;
}

const char* ESP32Metrics_GetVersion(void) {
    return ESP32METRICS_VERSION;
}

esp_err_t ESP32Metrics_GetCpuUsage(float* usage) {
    if (!s_metrics.is_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    if (usage == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    // Get task runtime statistics
    TaskStatus_t *task_stats = NULL;
    UBaseType_t task_count = uxTaskGetNumberOfTasks();
    uint32_t total_run_time = 0;
    
    // Allocate memory for task stats
    task_stats = malloc(sizeof(TaskStatus_t) * task_count);
    if (task_stats == NULL) {
        return ESP_ERR_NO_MEM;
    }
    
    // Get task stats
    task_count = uxTaskGetSystemState(task_stats, task_count, &total_run_time);
    
    // Find the idle task
    uint32_t idle_time = 0;
    for (UBaseType_t i = 0; i < task_count; i++) {
        if (strcmp(task_stats[i].pcTaskName, "IDLE") == 0) {
            idle_time = task_stats[i].ulRunTimeCounter;
            break;
        }
    }
    
    // Calculate CPU usage
    if (s_metrics.prev_total_run_time > 0) {
        uint32_t total_delta = total_run_time - s_metrics.prev_total_run_time;
        uint32_t idle_delta = idle_time - s_metrics.prev_idle_time;
        
        if (total_delta > 0) {
            *usage = 100.0f - ((float)idle_delta * 100.0f / (float)total_delta);
        } else {
            *usage = 0.0f;
        }
    } else {
        *usage = 0.0f; // First measurement
    }
    
    // Update previous values
    s_metrics.prev_total_run_time = total_run_time;
    s_metrics.prev_idle_time = idle_time;
    
    // Clean up
    free(task_stats);
    
    return ESP_OK;
}

esp_err_t ESP32Metrics_GetUptime(uint64_t* uptime) {
    if (!s_metrics.is_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    if (uptime == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    *uptime = esp_timer_get_time() / 1000; // Convert microseconds to milliseconds
    s_metrics.last_uptime = *uptime;
    
    return ESP_OK;
}

esp_err_t ESP32Metrics_GetWifiSignal(int8_t* rssi) {
    if (!s_metrics.is_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    if (rssi == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    wifi_ap_record_t ap_info;
    esp_err_t ret = esp_wifi_sta_get_ap_info(&ap_info);
    
    if (ret == ESP_OK) {
        *rssi = ap_info.rssi;
    } else {
        *rssi = 0;
    }
    
    return ret;
}

esp_err_t ESP32Metrics_GetWifiStatus(ESP32WifiStatus* status) {
    if (!s_metrics.is_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    if (status == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    wifi_mode_t mode;
    esp_err_t ret = esp_wifi_get_mode(&mode);
    
    if (ret != ESP_OK) {
        *status = ESP32_WIFI_NOT_INITIALIZED;
        return ret;
    }
    
    if (mode == WIFI_MODE_NULL || mode == WIFI_MODE_AP) {
        *status = ESP32_WIFI_DISCONNECTED;
        return ESP_OK;
    }
    
    // Check actual connection status
    wifi_ap_record_t ap_info;
    ret = esp_wifi_sta_get_ap_info(&ap_info);
    
    if (ret == ESP_OK) {
        *status = ESP32_WIFI_CONNECTED;
    } else if (ret == ESP_ERR_WIFI_NOT_CONNECT) {
        *status = ESP32_WIFI_DISCONNECTED;
    } else {
        *status = ESP32_WIFI_NOT_INITIALIZED;
    }
    
    return ESP_OK;
}

esp_err_t ESP32Metrics_GetTemperature(float* temp) {
    if (!s_metrics.is_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    if (temp == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    if (!s_metrics.temp_sensor_enabled) {
        *temp = NAN;
        return ESP_ERR_NOT_SUPPORTED;
    }
    
    esp_err_t ret = temp_sensor_read_celsius(temp);
    if (ret == ESP_OK) {
        s_metrics.last_temperature = *temp;
    } else {
        *temp = s_metrics.last_temperature;
    }
    
    return ret;
}

esp_err_t ESP32Metrics_GetBrownoutCount(uint32_t* count) {
    if (!s_metrics.is_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    if (count == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    *count = s_metrics.brownout_count;
    return ESP_OK;
}

esp_err_t ESP32Metrics_GetRebootReason(ESP32RebootReason* reason) {
    if (!s_metrics.is_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    if (reason == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    *reason = s_metrics.reboot_reason;
    return ESP_OK;
}

esp_err_t ESP32Metrics_GetWifiDataRate(float* rate) {
    if (!s_metrics.is_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    if (rate == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    wifi_ap_record_t ap_info;
    esp_err_t ret = esp_wifi_sta_get_ap_info(&ap_info);
    
    if (ret != ESP_OK) {
        *rate = 0.0f;
        return ret;
    }
    
    // Derive data rate from PHY mode, channel width, and RSSI
    // This is an approximation
    uint8_t primary_channel = ap_info.primary;
    wifi_second_chan_t secondary_channel = ap_info.second;
    wifi_phy_mode_t phy_mode;
    
    esp_wifi_sta_get_negotiated_phymode(&phy_mode);
    
    // Basic rate calculation based on PHY mode
    switch (phy_mode) {
        case WIFI_PHY_MODE_LR:
            *rate = 0.5f;
            break;
        case WIFI_PHY_MODE_HT:
            if (secondary_channel != WIFI_SECOND_CHAN_NONE) {
                *rate = 144.0f; // HT with 40MHz
            } else {
                *rate = 72.0f;  // HT with 20MHz
            }
            break;
        case WIFI_PHY_MODE_VHT:
            if (secondary_channel != WIFI_SECOND_CHAN_NONE) {
                *rate = 200.0f; // VHT with 40MHz
            } else {
                *rate = 96.0f;  // VHT with 20MHz
            }
            break;
        default: // WIFI_PHY_MODE_11B or others
            *rate = 11.0f;
            break;
    }
    
    // Adjust for signal strength
    // Simple linear adjustment - lose up to 50% of max rate at -90dBm
    float signal_factor = 1.0f - (float)fmax(-90 - ap_info.rssi, 0) / 40.0f;
    *rate *= fmax(signal_factor, 0.5f);
    
    return ESP_OK;
}

esp_err_t ESP32Metrics_GetLogLevel(esp_log_level_t* level) {
    if (!s_metrics.is_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    if (level == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    *level = esp_log_level_get(TAG);
    return ESP_OK;
}

esp_err_t ESP32Metrics_GetErrorCount(uint32_t* count) {
    if (!s_metrics.is_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    if (count == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    *count = s_metrics.error_count;
    return ESP_OK;
}

esp_err_t ESP32Metrics_IncrementErrorCount(void) {
    if (!s_metrics.is_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    s_metrics.error_count++;
    return ESP_OK;
}

esp_err_t ESP32Metrics_GetMetric(ESP32MetricType type, void* value) {
    if (!s_metrics.is_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    if (value == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    esp_err_t ret = ESP_OK;
    
    switch (type) {
        case ESP32_METRIC_CPU_USAGE:
            ret = ESP32Metrics_GetCpuUsage((float*)value);
            break;
        case ESP32_METRIC_UPTIME:
            ret = ESP32Metrics_GetUptime((uint64_t*)value);
            break;
        case ESP32_METRIC_WIFI_SIGNAL:
            ret = ESP32Metrics_GetWifiSignal((int8_t*)value);
            break;
        case ESP32_METRIC_WIFI_STATUS:
            ret = ESP32Metrics_GetWifiStatus((ESP32WifiStatus*)value);
            break;
        case ESP32_METRIC_TEMPERATURE:
            ret = ESP32Metrics_GetTemperature((float*)value);
            break;
        case ESP32_METRIC_BROWNOUT_COUNT:
            ret = ESP32Metrics_GetBrownoutCount((uint32_t*)value);
            break;
        case ESP32_METRIC_REBOOT_REASON:
            ret = ESP32Metrics_GetRebootReason((ESP32RebootReason*)value);
            break;
        case ESP32_METRIC_WIFI_DATA_RATE:
            ret = ESP32Metrics_GetWifiDataRate((float*)value);
            break;
        case ESP32_METRIC_LOG_LEVEL:
            ret = ESP32Metrics_GetLogLevel((esp_log_level_t*)value);
            break;
        case ESP32_METRIC_ERROR_COUNT:
            ret = ESP32Metrics_GetErrorCount((uint32_t*)value);
            break;
        default:
            ret = ESP_ERR_INVALID_ARG;
    }
    
    return ret;
}

// Private helper functions

static void update_reboot_reason(void) {
    esp_reset_reason_t reason = esp_reset_reason();
    s_metrics.reboot_reason.code = reason;
    
    // Map reason code to text
    if (reason < sizeof(s_reset_reason_strings) / sizeof(s_reset_reason_strings[0])) {
        s_metrics.reboot_reason.reason = s_reset_reason_strings[reason];
    } else {
        s_metrics.reboot_reason.reason = "Unknown reason";
    }
}
