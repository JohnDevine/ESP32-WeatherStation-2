/**
 * @file main.c
 * @brief Main application entry point
 * @version 0.1
 * @date 2025-08-24
 * @author john.h.devine@gmail.com
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "version.h"

void app_main() {
    // Start your application here
    printf("Gidday Mate\n");
    
    // Display version information
    ESP_LOGI("app_init", "Application information:");
    ESP_LOGI("app_init", "Project name:     %s", PROJECT_NAME);
    ESP_LOGI("app_init", "App version:      %s", PROJECT_VERSION);
    ESP_LOGI("app_init", "Compile time:     %s %s", PROJECT_BUILD_DATE, PROJECT_BUILD_TIME);
    
    // Alternatively, you could use: print_version_info();
}
