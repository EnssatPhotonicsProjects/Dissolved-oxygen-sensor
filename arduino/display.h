// OLED Screen 128x64 | Midas - MDOB128064V2V-YI

#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

extern U8G2_SSD1306_128X64_NONAME_F_2ND_HW_I2C u8g2;

bool init_display();
bool update_display(float temperature, float o2_estimated_percent, float o2_sv_percent);
bool show_measuring(int percent);

#endif