/**
 * @file CMakeLists.txt
 * @brief Build configuration for ESP32MetricsTest
 * @author john.h.devine@gmail.com
 * @version 1.0.0
 * @date August 22, 2025
 */

# ESP32MetricsTest component test app

idf_component_register(SRCS "ESP32MetricsTest.c"
                       INCLUDE_DIRS "."
                       REQUIRES ESP32Metrics)
