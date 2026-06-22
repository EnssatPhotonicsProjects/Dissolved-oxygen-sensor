#include "display.h"
#define OLED_I2C_ADDRESS 0x3C

U8G2_SSD1306_128X64_NONAME_F_2ND_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// In order to init display
bool init_display() {
    if (u8g2.begin()) {
        u8g2.enableUTF8Print();

        u8g2.clearBuffer();
        u8g2.setFont(u8g_font_7x14);
        u8g2.drawStr(0, 26, "   CONNEXION TO   ");
        u8g2.drawStr(0, 42, "   THE OXYMETER   ");
        u8g2.sendBuffer();
        delay(2500);
        u8g2.clearBuffer();
        u8g2.drawStr(0, 10, "Display : OK");
        u8g2.sendBuffer();

        SerialUSB.println("2. init_display OK");
        return true;
    } else {
        SerialUSB.println("Error : Failed to init display.");
        return false;
    }
}

// In order to display results
bool update_display(float temperature, float o2_estimated_percent, float o2_sv_percent) {
    
    // Security ping
    Wire1.beginTransmission(OLED_I2C_ADDRESS);
    byte error = Wire1.endTransmission();
    if (error != 0) {
        SerialUSB.println("Error : Failed to display results.");
        return false; 
    }

    // Write on screen
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
    SerialUSB.println("Succeed to display results.");
    return true; 
}

// In order to 
bool show_measuring(int percent) {
    // Security ping
    Wire1.beginTransmission(OLED_I2C_ADDRESS);
    if (Wire1.endTransmission() != 0) {
        return false; 
    }

    // Write on screen
    u8g2.clearBuffer();
    u8g2.setFont(u8g_font_7x14);

    u8g2.drawUTF8(15, 25, " Measuring...");

    u8g2.drawFrame(14, 40, 100, 12); // (x=14, y=40, L=100, l=12)
    int fill_width = (percent * 96) / 100; 
    if (fill_width > 0) {
        u8g2.drawBox(16, 42, fill_width, 8);
    }

    u8g2.sendBuffer(); 
    return true;
}