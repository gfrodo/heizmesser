// This RTC clock example keeps the system mostly in shutdown mode and
//  only wakes up every 58 seconds for a brief period of time during
//  which the time and date are updated on the ink display.
//
// When started initially or via power button a full ink display refresh
//  is executed to clear the display.
// The current time and date are fetched via NTP and shown on the ink display.
// After waiting for the full minute the system is put into shutdown
//  mode for about 58 seconds.
// When the RTC timer expires (just befor the next minute change) the
//  system is powered on.
// The ink display is updated with the current time and date.
// Then the system goes back into shutdown mode for about 58 seconds and
//  the cycle begins anew.
// Every hour a full ink display refresh is executed to keep the ink
//  display crisp.
//
// Note: If WiFi connection fails - some fantasy time and date are used.
// Note: System will not enter shutdown mode while USB is connected.

#include <Arduino.h>
#include "M5CoreInk.h"
#include <WiFi.h>
#include "esp_adc_cal.h"
#include "time.h"
#include <M5_KMeter.h>
#include <PubSubClient.h>

const char* ssid             = "NETGEAR06";
const char* password         = "fancyorchestra441";//lk13-cs63-eylg-rwao";
const char* ntpServer        = "pool.ntp.org";
const char* mqtt_server      = "homeassistant.fritz.box";
const char* mqtt_id          = "ESPClient-temperatur";
const char* mqtt_user        = "mqtt";
const char* mqtt_password    = "mqtt";
const char* mqtt_topic       = "homeassistant/sensor/boiler_temp/state";
const long gmtOffset_sec     = 3600;
const int daylightOffset_sec = 3600;
const char* mqtt_config      = "{\"name\": \"boiler_temp\", \"device_class\": \"temperature\", \"unit_of_measurement\": \"°C\", \"state_topic\": \"homeassistant/sensor/boiler_temp/state\"}";
const char* mqtt_config_topic = "homeassistant/sensor/boiler_temp/config";

Ink_Sprite TimePageSprite(&M5.M5Ink);
M5_KMeter sensor;
WiFiClient espClient;
PubSubClient client(espClient);
int lastMeasurement = 0;


void printLocalTimeAndSetRTC() {
    struct tm timeinfo;

    if (getLocalTime(&timeinfo) == false) {
        Serial.println("Failed to obtain time");
        return;
    }
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

    RTC_TimeTypeDef time;
    time.Hours   = timeinfo.tm_hour;
    time.Minutes = timeinfo.tm_min;
    time.Seconds = timeinfo.tm_sec;
    M5.rtc.SetTime(&time);

    RTC_DateTypeDef date;
    date.Date  = timeinfo.tm_mday;
    date.Month = timeinfo.tm_mon + 1;
    date.Year  = timeinfo.tm_year + 1900;
    M5.rtc.SetDate(&date);
}

void drawWarning(const char *str) {
    M5.M5Ink.clear();
    TimePageSprite.clear(CLEAR_DRAWBUFF | CLEAR_LASTBUFF);
    //drawImageToSprite(76, 40, &warningImage, &TimePageSprite);
    int length = 0;
    while (*(str + length) != '\0') length++;
    TimePageSprite.drawString((200 - length * 8) / 2, 100, str, &AsciiFont8x16);
    TimePageSprite.pushSprite();
}

float getBatVoltage() {
    analogSetPinAttenuation(35, ADC_11db);
    esp_adc_cal_characteristics_t *adc_chars =
        (esp_adc_cal_characteristics_t *)calloc(
            1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12,
                             3600, adc_chars);
    uint16_t ADCValue = analogRead(35);

    uint32_t BatVolmV = esp_adc_cal_raw_to_voltage(ADCValue, adc_chars);
    float BatVol      = float(BatVolmV) * 25.1 / 5.1 / 1000;
    free(adc_chars);
    return BatVol;
}

void checkBatteryVoltage(bool powerDownFlag) {
    float batVol = getBatVoltage();
    Serial.printf("Bat Voltage %.2f\r\n", batVol);

    if (batVol > 3.2) return;

    drawWarning("Battery voltage is low");
    if (powerDownFlag == true) {
        M5.shutdown();
    }
    while (1) {
        batVol = getBatVoltage();
        if (batVol > 3.2) return;
    }
}

void getNTPTime() {
    // Try to connect for 10 seconds
    uint32_t connect_timeout = millis() + 10000;

    Serial.printf("Connecting to %s ", ssid);
    WiFi.begin(ssid, password);
    while ((WiFi.status() != WL_CONNECTED) && (millis() < connect_timeout)) {
        delay(500);
        Serial.print(".");
    }
    if (WiFi.status() != WL_CONNECTED) {
        // WiFi connection failed - set fantasy time and date
        RTC_TimeTypeDef time;
        time.Hours   = 6;
        time.Minutes = 43;
        time.Seconds = 50;
        //M5.rtc.SetTime(&time);

        RTC_DateTypeDef date;
        date.Date  = 4;
        date.Month = 12;
        date.Year  = 2020;
        //M5.rtc.SetDate(&date);
        return;
    }

    Serial.println("Connected");

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    printLocalTimeAndSetRTC();

    //WiFi.disconnect(true);
    //WiFi.mode(WIFI_OFF);
}

void mqtt_update() {
    // Loop until we're reconnected
    if (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect(mqtt_id, mqtt_user, mqtt_password)) {
            Serial.println("connected");
            // Once connected, publish an announcement...
            client.publish(mqtt_config_topic, mqtt_config, true);
            client.publish(mqtt_topic, String(sensor.getTemperature()).c_str());
            // ... and resubscribe
            //client.subscribe("inTopic");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
        }
    }
    client.loop();
}

void drawTimeAndDate(RTC_TimeTypeDef time, RTC_DateTypeDef date, float Temp, int e_temp, int e_wifi, int e_mqtt) {
    char buf[64];

    snprintf(buf, 60, "%02d:%02d", time.Hours, time.Minutes);
    TimePageSprite.drawString(4, 30, buf, &AsciiFont8x16);
    snprintf(buf, 60, "%02d.%02d.%02d", date.Date, date.Month,
             date.Year - 2000);
    TimePageSprite.drawString(4, 50, buf, &AsciiFont8x16);
    snprintf(buf, 60, "Temp:");
    TimePageSprite.drawString(4, 80, buf, &AsciiFont24x48);
    snprintf(buf, 60, "%.1f C", Temp);
    TimePageSprite.drawString(4, 130, buf, &AsciiFont24x48);
    sprintf(buf, "Lipo:%.2fV", getBatVoltage());
    TimePageSprite.drawString(10, 2, buf, &AsciiFont8x16);
    sprintf(buf, "temp:%d, wifi:%d, mqtt:%d", e_temp, e_wifi, e_mqtt);
    TimePageSprite.drawString(4, 180, buf, &AsciiFont8x16);
}

RTC_TimeTypeDef g_time;
RTC_DateTypeDef g_date;

void setup() {
    // Check power on reason before calling M5.begin()
    //  which calls Rtc.begin() which clears the timer flag.
    Wire1.begin(21, 22);
    uint8_t data = M5.rtc.ReadReg(0x01);
    M5.begin();
    Wire.begin(SDA, SCL, 400000l);
    sensor.begin();
    sensor.setSleepTime(5);

    // Green LED - indicates ESP32 is running
    digitalWrite(LED_EXT_PIN, LOW);

    if (M5.M5Ink.isInit() == false) {
        Serial.printf("Ink Init failed");
        while (1) delay(100);
    }

    Serial.println("Power on by: power button");
    M5.M5Ink.clear();
    TimePageSprite.creatSprite(0, 0, 200, 200);
    TimePageSprite.clear(CLEAR_DRAWBUFF | CLEAR_LASTBUFF);// );
    // Fetch current time from Internet
    getNTPTime();
    M5.rtc.GetTime(&g_time);
    M5.rtc.GetDate(&g_date);
    sensor.update();
    drawTimeAndDate(g_time, g_date, sensor.getTemperature(), sensor.getError(), WiFi.status(), client.connected());
    TimePageSprite.pushSprite();
    client.setServer(mqtt_server, 1883);
    //client.setCallback(callback);

}

void loop() {
    
    Serial.println("loop");
    M5.rtc.GetTime(&g_time);
    M5.rtc.GetDate(&g_date);
    mqtt_update();
    
    // Wait until full minute, also continue if full minute passed but still within first 10s
    while (g_time.Seconds > 10) {
        M5.rtc.GetTime(&g_time);
        delay(200);
        M5.update();      
        if (M5.BtnPWR.wasPressed()) {
            Serial.printf("Btn %d was pressed, shutdown\r\n", BUTTON_EXT_PIN);
            Serial.flush();
            M5.shutdown();
        }
        mqtt_update();
    }
    M5.rtc.GetDate(&g_date);
    // Full refresh once per hour
    if (g_time.Minutes % 10 == 0) {// == FULL_REFRESH_MINUTE - 1) {
        M5.M5Ink.clear();
        TimePageSprite.clear(CLEAR_DRAWBUFF | CLEAR_LASTBUFF);
    }
    Serial.flush();
    //mainMbRead();

    // Draw new time and date
    sensor.update();
    drawTimeAndDate(g_time, g_date, sensor.getTemperature(), sensor.getError(), WiFi.status(), client.connected());
    TimePageSprite.pushSprite();
    if (client.connected()) {
        client.publish(mqtt_topic, String(sensor.getTemperature()).c_str());
        client.loop();
        lastMeasurement = g_time.Seconds;
    }
    checkBatteryVoltage(true);

    // Wait until 10s before full minute
    while (g_time.Seconds < 55) {
        M5.rtc.GetTime(&g_time);
        delay(200);
        M5.update();
        if (M5.BtnPWR.wasPressed()) {
            Serial.printf("Btn %d was pressed, shutdown\r\n", BUTTON_PWR_PIN);
            Serial.flush();
            M5.shutdown();
        }
        mqtt_update();
        if (client.connected() && g_time.Seconds >= lastMeasurement + 10) {
            sensor.update();
            client.publish(mqtt_topic, String(sensor.getTemperature()).c_str());
            client.loop();
            lastMeasurement = g_time.Seconds;
        }
    }
    return;
}
