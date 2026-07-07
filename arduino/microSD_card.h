/**
 * @file microSD_card.h
 * @brief Data logging module using a MicroSD card shield.
 ----------------------------------------------------------------------------
 * * This module initializes the SD card interface and automatically creates 
 * an incrementally named text file per session. It safely writes timestamps, 
 * temperature, phase shift, and O2 concentrations in a tab-separated format.
 ----------------------------------------------------------------------------
 * @note Hardware Reference : MicroSD Card Shield | Whadda - WPI304N
 */

#ifndef MICROSD_CARD_H
#define MICROSD_CARD_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "display.h"

bool init_sd_card();
bool log_data(float temperature, float theta, float o2_estimated_percent, float o2_sv_percent);

#endif