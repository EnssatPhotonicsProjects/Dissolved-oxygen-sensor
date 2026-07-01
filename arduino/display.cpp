#include "display.h"
#define OLED_I2C_ADDRESS_WIRE 0x3C       // 7 bits -> Wire1
#define OLED_I2C_ADDRESS_U8G2 (0x3C * 2) // 8 bits -> U8g2

U8G2_SSD1306_128X64_NONAME_F_2ND_HW_I2C u8g2(U8G2_R0, /*reset=*/ U8X8_PIN_NONE);

// In order to init display
bool init_display() {
    u8g2.setI2CAddress(OLED_I2C_ADDRESS_U8G2);

    if (!u8g2.begin()) {
        SerialUSB.println("Error : Failed to init display.");
        return false;
    }
    else {
        u8g2.enableUTF8Print();
        // First sreen
        u8g2.clearBuffer();
        u8g2.setFont(u8g_font_7x14);
        u8g2.drawStr(0, 26, "   CONNEXION TO   ");
        u8g2.drawStr(0, 42, "   THE OXYMETER   ");
        u8g2.sendBuffer();
        delay(2500);
        // Second screen ...
        u8g2.clearBuffer();
        u8g2.drawStr(0, 10, "Display : OK");
        u8g2.sendBuffer();

        SerialUSB.println("2. init_display OK");
        return true;
    }
}

// In order to display results
bool update_display(float temperature, float o2_estimated_percent, float o2_sv_percent) {
    // Security ping
    Wire1.beginTransmission(OLED_I2C_ADDRESS_WIRE);
    byte error = Wire1.endTransmission();
    if (error != 0) {
        SerialUSB.println("Error : Failed to display results.");
        return false; 
    }

    // Write on screen -> results
    u8g2.clearBuffer();
    u8g2.setFont(u8g_font_7x14);
    u8g2.drawStr(0, 10, "     OXYMETER     ");

    u8g2.setCursor(0, 26);
    u8g2.print("Temp : ");
    u8g2.print(temperature, 2);
    u8g2.print(" °C");

    u8g2.setCursor(0, 42);
    u8g2.print("O2 Est : ");
    u8g2.print(o2_estimated_percent, 2);
    u8g2.print(" %");

    u8g2.setCursor(0, 58);
    u8g2.print("O2 S-V : ");
    u8g2.print(o2_sv_percent, 2);
    u8g2.print(" %"); 

    u8g2.sendBuffer(); 

    SerialUSB.println("Successful display of results.");
    return true; 
}

// In order to display measuring waiting screen
bool show_measuring(int percent) {
    // Security ping
    Wire1.beginTransmission(OLED_I2C_ADDRESS_WIRE);
    if (Wire1.endTransmission() != 0) {
        return false; 
    }

    // Write on screen -> text and progress bar
    u8g2.clearBuffer();
    u8g2.setFont(u8g_font_7x14);

    u8g2.drawUTF8(15, 25, " Measuring...");
    u8g2.drawFrame(14, 40, 100, 12); // x=14, y=40, L=100, l=12
    int fill_width = (percent * 96) / 100; 
    if (fill_width > 0) {
        u8g2.drawBox(16, 42, fill_width, 8);
    }

    u8g2.sendBuffer(); 
    return true;
}