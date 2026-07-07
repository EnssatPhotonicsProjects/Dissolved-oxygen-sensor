/**
 * @file tools.h
 * @brief Utility and signal processing math module for the OXIMETER.
 ----------------------------------------------------------------------------
 * * This module stores global calibration constants (background noise,
 * Stern-Volmer coefficients) and performs all the mathematical translations 
 * from raw complex signals to physical oxygen concentrations.
 ----------------------------------------------------------------------------
 */

#ifndef TOOLS_H
#define TOOLS_H

#include <Arduino.h>
#include "complex.h"
#include "synchronous_detection.h"

float calculate_theta(Complex fourier);
float calculate_o2_polynomial(float theta, float temperature);
float calculate_o2_stern_volmer(float theta);

#endif
