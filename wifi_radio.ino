#include <GyverOS.h>
#include <GyverOLED.h>
#include <EncButton2.h>
#include "ESP8266WiFi.h"

#define EB_CLICK 200

#define MAIN_JOB 0
#define SETTINGS_MENU_JOB 1
#define CHECK_WIFI_CONNECTION_JOB 2
#define CHECK_BATTERY_JOB 3

GyverOS<5> OS;
GyverOLED<SSH1106_128x64> oled(0x3C);
EncButton2<EB_ENCBTN, EB_TICK> enc(INPUT_PULLUP, D5, D6, D7);

uint8_t oledContrast = 10;
uint8_t batCharge = 6;



//----------------MENU SECTION-----------------
#define SETTINGS_MENU_ITEMS 2
bool settingsMenuMode;

const char wifi_connection_menu[] PROGMEM = "WiFi connection";
const char screen_menu[] PROGMEM = "Screen";

const char* const settingsMenu[] PROGMEM =
{
  wifi_connection_menu,
  screen_menu
};

bool connected;

void setup() {
  Serial.begin(9600);

  oled.init();
  oled.clear();
  oled.setContrast(oledContrast  * 2.5);
  oled.update();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  OS.attach(MAIN_JOB, mainJob, 100);
  OS.attach(SETTINGS_MENU_JOB, printSettingsMenu, 10);
  OS.attach(CHECK_WIFI_CONNECTION_JOB, checkWiFiConnection, 1000);
  // OS.attach(CHECK_BATTERY_JOB, checkBattery, 606000);
  
  OS.stop(SETTINGS_MENU_JOB);
  // OS.exec(CHECK_BATTERY_JOB);
}

void printYesNoWindow(char* message, const char* opt1, const char* opt2, const char* opt3, uint8_t selected) {
  uint8_t messageLen = strlen(message);
  if (messageLen > 17) {
    *(message + 14) = '.';
    *(message + 15) = '.';
    *(message + 16) = '.';
    *(message + 17) = '\0';
    messageLen = 17;
  }
  uint8_t messageTab = (129 - messageLen * 6) / 2;
  oled.rect(10, 30, 117, 60, OLED_CLEAR);
  oled.rect(10, 30, 117, 60, OLED_STROKE);
  oled.setCursor(messageTab, 4);
  oled.print(message);
  oled.setCursor(12, 6);

  uint8_t yesNoSpace = 0;
  if (opt2 != "" || opt3 != "") {
    Serial.println("1");
    yesNoSpace = 12;
    if (opt2 != "" && opt3 != "") {
      Serial.println("2");
      yesNoSpace = 24;
    }
  }

  uint8_t optLen = strlen(opt1) + strlen(opt2) + strlen(opt3);
  uint8_t optTab = (129 - yesNoSpace - optLen * 6) / 2;

  oled.setCursor(optTab, 6);
  if (selected == 1) {
    oled.invertText(true);
  }
  oled.print(opt1);
  oled.invertText(false);
  oled.print("  ");
  if (selected == 2) {
    oled.invertText(true);
  }
  oled.print(opt2);
  oled.invertText(false);
  oled.print("  ");
  if (selected == 3) {
    oled.invertText(true);
  }
  oled.print(opt3);
  oled.invertText(false);
  oled.update();
}

void loop() {
  OS.tick();
  enc.tick();
}

void mainJob() {

  if (enc.held()) {
    if (!settingsMenuMode) {
      settingsMenuMode = true;
      OS.start(SETTINGS_MENU_JOB);
    }
  }

  if (enc.click()) {
    if (!settingsMenuMode) {
      oled.clear();
      oled.update();
    }
  }
}
