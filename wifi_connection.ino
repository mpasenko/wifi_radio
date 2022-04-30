const static uint8_t icons_8x8[][8] PROGMEM = {
  {0x02, 0x09, 0x25, 0x95, 0x95, 0x25, 0x09, 0x02}, // 0 WiFi 100
  {0x00, 0x08, 0x24, 0x94, 0x94, 0x24, 0x08, 0x00}, // 1 WiFi 75
  {0x00, 0x00, 0x20, 0x90, 0x90, 0x20, 0x00, 0x00}, // 2 WiFi 50
  {0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00}  // 3 WiFi 25
};

void checkWiFiConnection() {
  if (connected) {
    // Draw WiFi icon
    clearWiFiIcon();
    oled.setCursor(104, 0);
    drawIcon8x8(convertRssiToIconIndex(WiFi.RSSI()));
    oled.update();
  }
  if (myXor(WiFi.status() == WL_CONNECTED, connected)) {
    if (WiFi.status() == WL_CONNECTED) {
      connected = true;
    } else {
      clearWiFiIcon();
      oled.update();
      connected = false;
    }
  }
}

uint8_t convertRssiToPercent(const int32_t rssi) {
  return constrain(2 * (rssi + 100), 1, 100);
}

uint8_t convertRssiToIconIndex(const int32_t rssi) {
  uint8_t wifiPercent = convertRssiToPercent(rssi);
  if (wifiPercent >= 70) {
    return 0;
  } else if (wifiPercent < 70 && wifiPercent >= 50) {
    return 1;
  } else if (wifiPercent < 50 && wifiPercent >= 25) {
    return 2;
  } else {
    return 3;
  }
  
}

void clearWiFiIcon() {
  oled.rect(104, 0, 114, 8, OLED_CLEAR);
}

void clearBatteryIcon() {
  oled.rect(115, 0, 127, 8, OLED_CLEAR);
}

void checkBattery() {
  clearBatteryIcon();
  oled.setCursor(115, 0);
  oled.drawByte(0b00111100);                            // Пипка
  oled.drawByte(0b00111100);                            // 2 штуки
  oled.drawByte(0b11111111);                            // Передняя стенка
  for (uint8_t i = 0; i < 10; i++) {                    // 12 градаций
    if (i < 10 - batCharge) {
      oled.drawByte(0b10000001);                        // Рисуем пустые
    } else {
      oled.drawByte(0b11111111);                        // Рисуем полные
    }
  } oled.drawByte(0b11111111);                          // Задняя стенка
  oled.update();
}

void drawIcon8x8(byte index) {
  size_t s = sizeof icons_8x8[index];//можна так, а можна просто 8 
  for(unsigned int i = 0; i < s; i++) {
    oled.drawByte(pgm_read_byte(&(icons_8x8[index][i])));
  }
}