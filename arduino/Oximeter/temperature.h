/**
 * @file temperature.h
 * @brief Temperature measurement module using a OneWire digital sensor.
 ---------------------------------------------------------------------------------
 * * This module initializes and reads data from the waterproof temperature 
 * probe. It safely handles critical communication timing by temporarily disabling 
 * hardware interrupts during reads and includes a retry system for reliability.
 ---------------------------------------------------------------------------------
 * @note Hardware Reference : Temperature sensor | TRU COMPONENTS TC-9445340
 */

#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "display.h"

bool init_temperature();
float read_temperature();
float read_temperature_retry(uint n_retry);

#endif
