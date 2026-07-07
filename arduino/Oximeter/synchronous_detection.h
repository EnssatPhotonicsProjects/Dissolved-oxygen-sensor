/**
 * @file synchronous_detection.h
 * @brief Signal processing module for the dissolved oxygen sensor.
 ---------------------------------------------------------------------------
 * * This module manages the high-speed hardware timer (TC0) to generate a 
 * modulated sine wave excitation on the DAC, while synchronously sampling 
 * the photodiode response on the ADC. It accumulates the signal multiplied 
 * by sine/cosine references to extract the pure signal phase and amplitude.
 ---------------------------------------------------------------------------
 */

#ifndef SYNCHRONOUS_DETECTION_H
#define SYNCHRONOUS_DETECTION_H

#include <Arduino.h>
#include "complex.h"
#include "display.h"

extern const float FREQUENCY;

bool init_synchronous_detection();
Complex synchronous_detection(float average_duration);

#endif
