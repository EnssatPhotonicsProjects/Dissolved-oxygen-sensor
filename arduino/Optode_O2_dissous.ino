/**
 * @file Optode_O2_dissous.ino
 * @brief Main execution routine for the open-source dissolved oxygen sensor.
 ----------------------------------------------------------------------------
 * * This file acts as the system orchestrator. It initializes all hardware 
 * modules (OLED, MicroSD, Temperature, and Synchronous Detection). In the 
 * main loop, it triggers the measurement cycle, compensates the raw signal 
 * for background noise, and extracts the fluorescence phase shift. Finally, 
 * it calculates the O2 concentration using both a polynomial calibration 
 * model and the Stern-Volmer equation, logging the results in real-time.
 ----------------------------------------------------------------------------
 * Go to tools.h and tools.cpp to calibrate the Oxymeter /!\
 */

#include "tools.h"
#include "complex.h"
#include "display.h"
#include "temperature.h"
#include "microSD_card.h"
#include "synchronous_detection.h"

//const Complex F0 = {0.01513908, 0.01901101};

// In order to test and prepare the system
void setup() {
  SerialUSB.begin(115200);
  delay(1000);

  SerialUSB.println(" ");
  SerialUSB.println("-------- CONNEXION OXYMETER --------");
  SerialUSB.println("1. SerialUSB OK");
  delay(100);

  init_display();
  delay(100);
  
  init_temperature();
  delay(100);

  init_sd_card();
  delay(100);

  init_synchronous_detection();
  delay(2500); // To have time to read inits

  SerialUSB.println("6. Entering void loop().");
  delay(100);
}

// In order to measure, calculate, print results and log results
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

  // [3] Display and logging data
  update_display(temperature, o2_estimated_percent, o2_sv_percent);
  log_data(temperature, theta, o2_estimated_percent, o2_sv_percent);

/*   // Measurement and calculation of Fourier complex
  Complex fourier = synchronous_detection(5.0);
  SerialUSB.print("fourier = ");
  print_complex(fourier);
  SerialUSB.print(" V -> ");
  Complex corrected = (fourier-F0);
  SerialUSB.print("corrected = ");
  print_complex(corrected);
  SerialUSB.println(" V");

  // Measurement of temperature
  float temperature = read_temperature_retry(3);
  SerialUSB.print("temperature = ");
  SerialUSB.print(temperature, 2);
  SerialUSB.print(" °C; ");

  // Calculation of theta
  float theta = atan2(corrected.imag, corrected.real)+(144)/180.0*3.14159265;
  theta = theta*180.0/3.14159265;
  SerialUSB.print("theta = ");
  SerialUSB.print(theta, 2);
  SerialUSB.print(" °; ");

  // Manual polynomial fit based on ref optode
  float o2_estimated_percent = min(max(approximation(theta, temperature), 0.0), 105.0);
  SerialUSB.print("estimated_o2_percent = ");
  SerialUSB.print(o2_estimated_percent, 2);
  SerialUSB.print(" %; ");

  // Eq Stern-Volmer
  float tau = tan(theta/180.0*3.14159265)/(2.0*3.14159265*5000.0); // 5000 -> FREQUENCY
  float tau0 = 56.5e-6;
  float K_sv = 0.0214;
  float o2_sv_percent = ((tau0/tau)-1.0)/K_sv;
  SerialUSB.print("sternvolm_o2_percent = ");
  SerialUSB.print(o2_sv_percent, 2);
  SerialUSB.println(" %");

  // Write and display results
  update_display(temperature, o2_estimated_percent, o2_sv_percent);
  log_data(temperature, theta, o2_estimated_percent, o2_sv_percent); */

  delay(2500); // To have time to read results
}