/**
 * @file Oximeter.ino
 * @brief Main execution routine for the open-source dissolved oxygen sensor.
 * ----------------------------------------------------------------------------
 * * This file acts as the system orchestrator. It initializes all hardware 
 * modules (OLED, MicroSD, Temperature, and Synchronous Detection). In the 
 * main loop, it handles:
 * [1] Acquisition: Triggers the measurement cycle and reads sensors.
 * [2] Math translation: Delegates the calculation of phase shift (theta) 
 * and O2 concentrations to the external mathematical module tools.cpp
 * [3] Output: Displays and logs the processed data in real-time.
 * ----------------------------------------------------------------------------
 * @attention To calibrate the OXIMETER (background noise F0, phase offset 
 * or Stern-Volmer constants), edit the variables inside tools.cpp!
 */

#include "tools.h"
#include "complex.h"
#include "display.h"
#include "temperature.h"
#include "microSD_card.h"
#include "synchronous_detection.h"


// In order to test and prepare the system
void setup() {
  SerialUSB.begin(115200);
  delay(3000);

  SerialUSB.println("-------- CONNEXION OXIMETER --------");
  delay(100);

  init_display();
  delay(100);
  
  init_temperature();
  delay(100);

  init_sd_card();
  delay(100);

  init_synchronous_detection();
  delay(3000); // To have time to read inits
}

// In order to measure, calculate, display results and log results
void loop() {
  // Waiting screen for measuring
  SerialUSB.println("-------------- Mesure --------------");
  show_measuring(0);

  // [1] Acquisition -> synchonous detection.cpp + temperature.cpp
  Complex fourier = synchronous_detection(5.0);
  float temperature = read_temperature_retry(3);

  // [2] Mathematical translations -> tools.cpp
  float theta = calculate_theta(fourier);
  float o2_estimated_percent = calculate_o2_polynomial(theta, temperature);
  float o2_sv_percent = calculate_o2_stern_volmer(theta);

  // [3] Display and logging data -> display.cpp + microSD_card.cpp
  update_display(temperature, o2_estimated_percent, o2_sv_percent);
  log_data(temperature, theta, o2_estimated_percent, o2_sv_percent);

  delay(3000); // To have time to read results
}