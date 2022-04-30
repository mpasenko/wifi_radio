bool redraw = true;
uint8_t globalMenuPointer = 1;
uint8_t menuPointer = 1;
uint8_t pointerFrom = 0;
uint8_t pointerTo = 7;

uint8_t globalWiFiPointer;

const static char symbols[] = "1234567890aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxWyYzZ";
char pwd[32] = "";
int pwdPointer = 0;

void getMenuItem(uint8_t num, const char* const* menu, char* menuItem) {
  strcpy_P(menuItem, (char*)pgm_read_dword(&(menu[num])));
}

void printPointer() {
  oled.rect(0, pointerFrom * 8, 7, pointerTo * 8 + 8, OLED_CLEAR);
  oled.setCursor(0, menuPointer);
  oled.print(">");
  // Можно еще в конце
  /*oled.setCursor(20, pointer);
    oled.print("<");*/
  // Указатель в виде прямоугольной рамки со скругленными углами
  /*oled.roundRect(0, pointer * 8, 127, (pointer * 8) + 6);*/
  // Указатель в виде битмапа (картинки)
  /*oled.drawBitmap(0, pointer * 8, ptr_bmp, 10, 8);*/
  oled.update();
}

void printMenu(char* menuHeader, uint8_t menuLen, const char* const* menu) {
  if (redraw) {
    redraw = false;
    clearMenu();
    oled.setCursor(10, 0);
    oled.print(menuHeader);
    for (uint8_t i = 0; i < constrain(menuLen, 0, 7); i++) {
      char item[21] = "";
      uint8_t itemNum = globalMenuPointer - menuPointer + i;
      getMenuItem(itemNum, menu, item);
      oled.setCursor(8, i + 1);
      oled.print(item);
    }
    printPointer();
    oled.update();
  }
}

void printSettingsMenu() {
  if (redraw) {
    pointerFrom = 1;
    pointerTo = 7;
  }
  printMenu("Settings", SETTINGS_MENU_ITEMS, settingsMenu);

  if (enc.left()) {
    if (menuPointer > 1) {
      menuPointer--;
      globalMenuPointer--;
      printPointer();
    }
    //    if (menuPointer > 1 ) {
    //      menuPointer--;
    //      printPointer();
    //    } else {
    //      redraw = true;
    //    }
    //    if (globalMenuPointer > 1) {
    //      globalMenuPointer--;
    //    }
  }

  if (enc.right()) {
    if (menuPointer < SETTINGS_MENU_ITEMS) {
      menuPointer++;
      globalMenuPointer++;
      printPointer();
    }
    //    if (menuPointer < 7) {
    //      menuPointer++;
    //      printPointer();
    //    } else {
    //      redraw = true;
    //    }
    //    if (globalMenuPointer < SETTINGS_MENU_ITEMS - 1) {
    //      globalMenuPointer++;
    //    }
  }

  if (enc.click()) {
    switch (globalMenuPointer) {
      case 1:
        OS.stop(SETTINGS_MENU_JOB);
        OS.attach(SETTINGS_MENU_JOB, connectToWifi, 10);
        clearMenu();
        oled.update();
        redraw = true;
        break;
      case 2:
        OS.stop(SETTINGS_MENU_JOB);
        OS.attach(SETTINGS_MENU_JOB, setContrast, 10);
        clearMenu();
        oled.update();
        redraw = true;
        break;
    }
  }

  if (enc.held()) {
    OS.stop(SETTINGS_MENU_JOB);
    redraw = true;
    clearMenu();
    oled.update();
    settingsMenuMode = false;
  }
}

void connectToWifi() {
  static uint8_t networksCount;
  static bool redrawNetworks;
  if (redraw) {
    pointerFrom = 2;
    pointerTo = 7;
    oled.setCursor(10, 0);
    oled.print("Wi-Fi");
    oled.setCursor(0, 1);
    oled.print("Scanning...");
    oled.update();
    globalWiFiPointer = 0;
    menuPointer = 2;
    networksCount = WiFi.scanNetworks();

    oled.setCursor(0, 1);
    char nFoundStr[20];
    sprintf(nFoundStr, "%d network(s) found", constrain(networksCount, 0, 99));
    oled.print(nFoundStr);
    
    redraw = false;
    redrawNetworks = true;
  }

  if (redrawNetworks)  {
    oled.rect(0, 16, 127, 63, OLED_CLEAR);
    uint8_t k = constrain(networksCount, 1, 6);
    for (int i = 0; i < k; i++) {
      oled.setCursor(8, i + 2);
      oled.print(WiFi.SSID(globalWiFiPointer - menuPointer + i + 2));
    }
    printPointer();
    redrawNetworks = false;
  }

  if (enc.left()) {
    if (menuPointer > 2 ) {
      menuPointer--;
      printPointer();
    } else if (globalWiFiPointer > 0) {
      redrawNetworks = true;
    }
    if (globalWiFiPointer > 0) {
      globalWiFiPointer--;
    }
  }

  if (enc.right()) {
    if (menuPointer < 7) {
      menuPointer++;
      printPointer();
    } else if (globalWiFiPointer < networksCount - 1) {
      redrawNetworks = true;
    }
    if (globalWiFiPointer < networksCount - 1) {
      globalWiFiPointer++;
    }
  }

  if (enc.click()) {
    OS.stop(SETTINGS_MENU_JOB);
    OS.attach(SETTINGS_MENU_JOB, setPwd, 10);
    clearMenu();
    oled.update();
    menuPointer = 1;
    redraw = true;
  }

  if (enc.held()) {
    OS.stop(SETTINGS_MENU_JOB);
    OS.attach(SETTINGS_MENU_JOB, printSettingsMenu, 10);
    clearMenu();
    oled.update();
    menuPointer = 1;
    redraw = true;
  }
}

void setContrast() {
  if (redraw) {
    clearMenu();
    oled.setScale(1);
    oled.setCursor(10, 0);
    oled.print("Contrast");
    uint8_t x;
    if (oledContrast == 0) {
      x = 49;
    } else if (oledContrast == 100) {
      x = 33;
    } else {
      x = 41;
    }
    oled.setScale(3);
    oled.setCursorXY(x, 25);
    oled.print(oledContrast);
    oled.print("%");
    oled.update();
    redraw = false;
  }

  if (enc.left()) {
    if (oledContrast > 0) {
      oledContrast -= 10;
      oled.setContrast(oledContrast * 2.5);
      redraw = true;
    }
  }

  if (enc.right()) {
    if (oledContrast < 100) {
      oledContrast += 10;
      oled.setContrast(oledContrast * 2.5);
      redraw = true;
    }
  }

  if (enc.click()) {
    OS.stop(SETTINGS_MENU_JOB);
    OS.attach(SETTINGS_MENU_JOB, printSettingsMenu, 10);
    oled.setScale(1);
    clearMenu();
    oled.update();
    redraw = true;
  }
}

void setPwd() {
  static bool wait;
  static unsigned long pwdTmr;
  static uint8_t clicks;
  static bool yesNo;
  static uint8_t yesNoPointer;
  
  if (redraw) {
    redraw = false;
    yesNo = false;
    
    clearMenu();
    oled.setCursor(1, 1);
    oled.print("Enter the password.");

    oled.setCursor(1, 2);
    oled.print(pwd);
    oled.invertText(true);
    oled.print(symbols[pwdPointer]);
    oled.invertText(false);
    oled.update();
  }

  if (enc.left()) {
    if (!yesNo) {
      if (pwdPointer - 1 < 0) {
        pwdPointer = sizeof(symbols) - 2;
      } else {
        pwdPointer--;
      }
      oled.setCursor(1, 2);
      oled.print(pwd);
      oled.invertText(true);
      oled.print(symbols[pwdPointer]);
      oled.invertText(false);
      oled.update();
    } else {
      if (yesNoPointer > 1) {
        yesNoPointer--;
        printYesNoWindow("Are you sure?", "Yes", "No", "Exit", yesNoPointer);
      }
    }
  }

  if (enc.right()) {
    if (!yesNo) {
      if (pwdPointer + 1 >= sizeof(symbols) - 1) {
        pwdPointer = 0;
      } else {
        pwdPointer++;
      }
      oled.setCursor(1, 2);
      oled.print(pwd);
      oled.invertText(true);
      oled.print(symbols[pwdPointer]);
      oled.invertText(false);
      oled.update();
    } else {
      if (yesNoPointer < 3) {
        yesNoPointer++;
        printYesNoWindow("Are you sure?", "Yes", "No", "Exit", yesNoPointer);
      }
    }
  }

  if (enc.click()) {
    if (!yesNo) {
      if (!wait) {
        wait = true;
        pwdTmr = millis();
        clicks++;
      } else {
        clicks++;
      }
    } else {
      switch(yesNoPointer) {
        case 1 :
          WiFi.disconnect();
          connected = false;
          pwdPointer = 0;
          clearMenu();
          oled.setCursor(0, 0);
          oled.print("Connecting...");
          oled.update();
          WiFi.begin(WiFi.SSID(globalWiFiPointer), pwd);
          static unsigned long tmr = millis();
          while(millis() - tmr < 11000) {
            if (WiFi.status() == WL_CONNECTED) {
              oled.setCursor(0, 1);
              oled.print("DONE!!!");
              connected = true;
              OS.exec(CHECK_WIFI_CONNECTION_JOB);
              break;
            }
            if (millis() - tmr > 10000) {
              oled.setCursor(0, 1);
              oled.print("Cannot to connect");
              break;
            }
            delay(1000);
          }
          oled.update();
          delay(3000);
          OS.stop(SETTINGS_MENU_JOB);
          OS.attach(SETTINGS_MENU_JOB, printSettingsMenu, 10);
          clearMenu();
          oled.update();
          for (int i = 0; i < 32; i++) {
            pwd[i] = NULL;
          }
          redraw = true;
          break;
        case 2 :
          yesNo = false;
          redraw = true;
          break;
        case 3 :
          yesNo = false;
          pwdPointer = 0;
          OS.stop(SETTINGS_MENU_JOB);
          OS.attach(SETTINGS_MENU_JOB, printSettingsMenu, 10);
          clearMenu();
          oled.update();
          redraw = true;
          break;
      }
    }
  }
  
  if (!yesNo && wait && millis() - pwdTmr > 300) {
    if (clicks == 1 && strlen(pwd) < 32) {                  // Один клик
      pwd[strlen(pwd)] = symbols[pwdPointer];
      pwd[strlen(pwd) + 1] = NULL;
      oled.setCursor(1, 2);
      oled.print(pwd);
      oled.invertText(true);
      oled.print(symbols[pwdPointer]);
      oled.invertText(false);
      oled.update();
    } else if (clicks = 2 && strlen(pwd) > 0) {          // Два клика
      pwd[strlen(pwd) - 1] = NULL;
      clearMenu();
      oled.setCursor(1, 1);
      oled.print("Enter the password.");
      oled.setCursor(1, 2);
      oled.print(pwd);
      oled.invertText(true);
      oled.print(symbols[pwdPointer]);
      oled.invertText(false);
      oled.update();
    }
    clicks = 0;
    wait = false;
    pwdTmr = millis();
  }

  if (enc.held()) {
    if (!yesNo) {
      yesNo = true;
      yesNoPointer = 1;
      printYesNoWindow("Are you sure?", "Yes", "No", "Exit", yesNoPointer);
    }
  }
}

void clearMenu() {
  oled.rect(0, 0, 90, 8, OLED_CLEAR);
  oled.rect(0, 8, 127, 63, OLED_CLEAR);
}
