#include "task_wifi.h"

void startAP()
{
    WiFi.mode(WIFI_AP);
    Serial.print("Starting AP with SSID: ");
    Serial.println(String(SSID_AP));
    Serial.print("Using PASS: ");
    Serial.println(String(PASS_AP));
    bool ok = WiFi.softAP(String(SSID_AP), String(PASS_AP));
    Serial.print("softAP() returned: ");
    Serial.println(ok ? "true" : "false");
    delay(200); // allow AP stack to initialize
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());
}

void startSTA()
{
    if (WIFI_SSID.isEmpty())
    {
        vTaskDelete(NULL);
    }

    WiFi.mode(WIFI_STA);

    if (WIFI_PASS.isEmpty())
    {
        WiFi.begin(WIFI_SSID.c_str());
    }
    else
    {
        WiFi.begin(WIFI_SSID.c_str(), WIFI_PASS.c_str());
    }

    while (WiFi.status() != WL_CONNECTED)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    //Give a semaphore here
    xSemaphoreGive(xBinarySemaphoreInternet);
}

bool Wifi_reconnect()
{
    const wl_status_t status = WiFi.status();
    if (status == WL_CONNECTED)
    {
        return true;
    }
    startSTA();
    return false;
}
