#include "synchronous_detection.h"
#include "temperature.h"
#include "complex.h"
#include "microSD_card.h"
#include "display.h"

// Background noise of the circuit F0 
// fourier = 0.01513908+0.01901101i V
const Complex F0 = {0.01513908, 0.01901101};

// In order to test and setup the system
void setup() {
  SerialUSB.begin(115200);
  delay(100);

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
  delay(100);

  SerialUSB.println("6. Entering void loop().");
  delay(100);
}

// In order to estimate o2 percent
float approximation(float theta, float temperature) {
    theta = theta/180.0*3.14159265;
    float p = -1.5272554042e+04;
    p += (8.1836636109e+04) * theta;
    p += (1.9136823848e+03) * temperature;
    p += (-8.5902214727e+04) * pow(theta, 2);
    p += (-1.4795950625e+04) * theta * temperature;
    p += (-3.0583962937e+05) * pow(theta, 3);
    p += (4.7073632627e+04) * pow(theta, 2) * temperature;
    p += (9.9169344938e+05) * pow(theta, 4);
    p += (-7.8973876407e+04) * pow(theta, 3) * temperature;
    p += (-1.1863421751e+06) * pow(theta, 5);
    p += (7.3693877465e+04) * pow(theta, 4) * temperature;
    p += (6.6651777450e+05) * pow(theta, 6);
    p += (-3.6265683118e+04) * pow(theta, 5) * temperature;
    p += (-1.4668345339e+05) * pow(theta, 7);
    p += (7.3541142002e+03) * pow(theta, 6) * temperature;
    return p;
}

// In order to print complex 
void print_complex(Complex x){
  SerialUSB.print(x.real, 8);
  if (x.imag >= 0) SerialUSB.print("+");
  SerialUSB.print(x.imag, 8);
  SerialUSB.print("i");
}

// In order to mesure, calculate, print results and write results
void loop() {
  SerialUSB.println("-------------- Mesure --------------");

  // Measurement of temperature
  float temperature = read_temperature_retry(5);
  SerialUSB.print("temperature = ");
  SerialUSB.print(temperature, 4);
  SerialUSB.println(" °C");

  // Waiting screen for measuring
  show_measuring(0);

  // Measurement and calculation of Fourier complex
  Complex fourier = synchronous_detection(10.0);
  SerialUSB.print("fourier = ");
  print_complex(fourier);
  SerialUSB.print(" V -> ");
  Complex corrected = (fourier-F0);
  SerialUSB.print("corrected = ");
  print_complex(corrected);
  SerialUSB.println(" V");

  // Calculation of theta
  float theta = atan2(corrected.imag, corrected.real)+(144)/180.0*3.14159265;
  theta = theta*180.0/3.14159265;
  SerialUSB.print("theta = ");
  SerialUSB.print(theta, 4);
  SerialUSB.println(" °");

  // Manual polynomial fit based on ref optode
  float o2_estimated_percent = min(max(approximation(theta, temperature), 0.0), 200.0);
  SerialUSB.print("estimated_o2_percent = ");
  SerialUSB.print(o2_estimated_percent, 4);
  SerialUSB.println(" %");

  // Eq Stern-Volmer
  float tau = tan(theta/180.0*3.14159265)/(2.0*3.14159265*5000.0);
  float tau0 = 56.5e-6;
  float K_sv = 0.0214;
  float o2_sv_percent = ((tau0/tau)-1.0)/K_sv;
  SerialUSB.print("sternvolm_o2_percent = ");
  SerialUSB.print(o2_sv_percent, 4);
  SerialUSB.println(" %");

  // Write and display results
  update_display(temperature, o2_estimated_percent, o2_sv_percent);
  log_data(temperature, theta, o2_estimated_percent, o2_sv_percent);
  //delay(2500);
}