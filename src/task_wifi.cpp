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

    Serial.print("Connecting to SSID: ");
    Serial.println(WIFI_SSID);

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
        Serial.print(".");
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    Serial.println();
    Serial.println("WiFi Connected!");

    // 🔹 In IP của ESP
    Serial.print("ESP STA IP: ");
    Serial.println(WiFi.localIP());

    Serial.print("Gateway: ");
    Serial.println(WiFi.gatewayIP());

    Serial.print("Subnet: ");
    Serial.println(WiFi.subnetMask());

    // Give semaphore when internet ready
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
