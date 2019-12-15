#include <ESP8266WiFi.h>
#include <time.h>
#include <string.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "WiFiManager.h"         //https://github.com/tzapu/WiFiManager
#include<stdlib.h> //itoa所需要的库函数
/**
   The MIT License (MIT)
   https://thingpulse.com
*/
#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"        // legacy: #include "SSD1306.h"
//#include "images.h"             // Optionally include custom images
SSD1306Wire display(0x3c, 2, 0);   // ADDRESS, SDA, SCL
// SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_128_32);  // ADDRESS, SDA, SCL, OLEDDISPLAY_GEOMETRY  -  Extra param required for 128x32 displays.

// Initialize the OLED display using SPI:
// D5 -> CLK
// D7 -> MOSI (DOUT)
// D0 -> RES
// D2 -> DC
// D8 -> CS
// SSD1306Spi display(D0, D2, D8);  // RES, DC, CS

// WiFi Config
#define APMODE true
#define WIFI_SSID     "chinaunion"
#define WIFI_PASSWORD "chinaunion"

// NTP Config
int timezone = 8 * 3600;//中国+8
int dst = 0;

void setup() {
  display.init();
  display.flipScreenVertically();
  display.setFont(Dialog_plain_16);//Dialog_plain_8,Dialog_plain_16,Dialog_plain_32,Dialog_plain_48
  display.setTextAlignment(TEXT_ALIGN_LEFT);//TEXT_ALIGN_CENTER，TEXT_ALIGN_RIGHT，TEXT_ALIGN_CENTER_BOTH

  if (APMODE) {
    display.clear();
    display.drawString(0, 0, "wifi:ClockAP");
    display.display();
    WiFiManager wifiManager;
    wifiManager.autoConnect("ClockAP");
  } else {
    display.clear();
    display.drawString(0, 0, "conn...");
    display.display();
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  }

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  display.clear();
  display.drawString(0, 0, "getting time");
  display.display();

  // Get time from NTP server
  configTime(timezone, dst, "ntp.ku.ac.th", "fw.eng.ku.ac.th", "ilm.live.rmutt.ac.th");
  while (!time(nullptr))
  {
    delay(500);
  }
  display.setFont(Dialog_plain_32);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
}


void loop() {
  Serial.begin(9600);
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  char local_time[5];
  strcpy (local_time, "");

  char hh[4];//这里如果写2，就无法获取正确的小时。
  char mm[2];

  itoa(p_tm->tm_hour, hh, 10);

  itoa(p_tm->tm_min, mm, 10);

  if (strlen(hh) < 2) {
    strcat (local_time, "0");
    strcat (local_time, hh);
  } else {
    strcat (local_time, hh);
  }
  strcat (local_time, ":");
  if (strlen(mm) < 2) {
    strcat (local_time, "0");
    strcat (local_time, mm);
  } else {
    strcat (local_time, mm);
  }

  display.clear();// clear the display
  display.drawString(64, 10, local_time);//字体设置剧中的时候，坐标x应是中心点，即128/2 如果x设置为0，则会显示不全。
  display.display();// write the buffer to the display
  delay(1000);
}
