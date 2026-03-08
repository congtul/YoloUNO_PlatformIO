#include "global.h"
float glob_temperature = 0;
float glob_humidity = 0;
float ai_result = 0;

boolean led_enable = true;
boolean alarm_warning = false;

String WIFI_SSID;
String WIFI_PASS;
String CORE_IOT_TOKEN;
String CORE_IOT_SERVER;
String CORE_IOT_PORT;

String ssid = "ESP32-YOUR NETWORK HERE!!!";
String password = "12345678";
String wifi_ssid = "gerkenna";
String wifi_password = "tritintu";
boolean isWifiConnected = false;
SemaphoreHandle_t xBinarySemaphoreInternet = xSemaphoreCreateBinary();