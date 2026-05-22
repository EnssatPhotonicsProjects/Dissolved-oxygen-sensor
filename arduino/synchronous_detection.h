#ifndef SYNCHRONOUS_DETECTION_H
#define SYNCHRONOUS_DETECTION_H

#include <Arduino.h>
#include "complex.h"
// Structure to return the complex Fourier coefficients


// --- Core API ---
void init_synchronous_detection();
void start_synchronous_detection();
void stop_synchronous_detection();
Complex synchronous_detection(float average_duration);

#endif