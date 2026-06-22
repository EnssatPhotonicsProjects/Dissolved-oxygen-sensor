// MICROSD CARD LOGGING SHIELD FOR ARDUINO (WPI304N)

#ifndef MICROSD_CARD_H
#define MICROSD_CARD_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

bool init_sd_card();
bool log_data(float temperature, float theta, float o2_estimated_percent, float o2_sv_percent);

#endif