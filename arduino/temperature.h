#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <Arduino.h>

// Initialise le bus 1-Wire et le capteur
void init_temperature();

// Demande et retourne la température en degrés Celsius
float read_temperature();
float read_temperature_retry(uint n_retry);
#endif