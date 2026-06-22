#ifndef SYNCHRONOUS_DETECTION_H
#define SYNCHRONOUS_DETECTION_H

#include <Arduino.h>
#include "complex.h"

bool init_synchronous_detection();
Complex synchronous_detection(float average_duration);

#endif