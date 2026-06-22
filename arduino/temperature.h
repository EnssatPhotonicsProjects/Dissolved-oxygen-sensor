// 

#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

bool init_temperature();
float read_temperature();
float read_temperature_retry(uint n_retry);

#endif