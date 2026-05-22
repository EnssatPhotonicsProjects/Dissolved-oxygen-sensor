#include "synchronous_detection.h"
#include "temperature.h"
#include "complex.h"

const Complex SIGNAL_ZERO = {0.00455241, 0.00509683};
const Complex SIGNAL_ONE = {-0.30569851, -0.28863692};

const Complex F0 = {0.00674127, 0.00161394};


float approximation(float theta, float temp) {
    float p = -1.5272554042e+04;
    p += (8.1836636109e+04) * theta;
    p += (1.9136823848e+03) * temp;
    p += (-8.5902214727e+04) * pow(theta, 2);
    p += (-1.4795950625e+04) * theta * temp;
    p += (-3.0583962937e+05) * pow(theta, 3);
    p += (4.7073632627e+04) * pow(theta, 2) * temp;
    p += (9.9169344938e+05) * pow(theta, 4);
    p += (-7.8973876407e+04) * pow(theta, 3) * temp;
    p += (-1.1863421751e+06) * pow(theta, 5);
    p += (7.3693877465e+04) * pow(theta, 4) * temp;
    p += (6.6651777450e+05) * pow(theta, 6);
    p += (-3.6265683118e+04) * pow(theta, 5) * temp;
    p += (-1.4668345339e+05) * pow(theta, 7);
    p += (7.3541142002e+03) * pow(theta, 6) * temp;
    return p;
}
float solve(Complex fourier, float temp){
    fourier = fourier - F0;
    float theta = atan2(fourier.imag, fourier.real);
    return approximation(theta, temp);
}

void setup() {
  SerialUSB.begin(115200);
  while (!SerialUSB); // Wait for native USB to connect
  SerialUSB.println("Serial ok");
  delay(100);
  init_synchronous_detection();
  SerialUSB.println("init sync detec ok");
  delay(100);
  init_temperature();
  SerialUSB.println("init temp ok");
  delay(100);
  start_synchronous_detection();
  delay(100);
}

Complex corrected_signal(Complex x) {
    return (x-SIGNAL_ZERO)/(SIGNAL_ONE-SIGNAL_ZERO);
}
float emis_time(Complex x) {
    return x.imag/x.real/6.283185307/5000;
}
void print_complex(Complex x){
  SerialUSB.print(x.real, 8);
  if (x.imag >= 0) SerialUSB.print("+");
  SerialUSB.print(x.imag, 8);
  SerialUSB.print("i");
}
void loop() {
  delay(500);
  Complex fourier = synchronous_detection(10.0);

  SerialUSB.print("fourier=");
  print_complex(fourier);
  SerialUSB.println(" V");

  // Complex corrected = (fourier-SIGNAL_ZERO)/(SIGNAL_ONE-SIGNAL_ZERO);
  Complex corrected = (fourier-F0);

  // float theta = atan2(corrected.imag, corrected.real)+(110.0+30.5)/180.0*3.14;
  float theta = atan2(corrected.imag, corrected.real)+(110.0+31.5)/180.0*3.14;

  // theta = theta + (110.0+30.5)/180.0*3.14;
  SerialUSB.print("theta=");
  SerialUSB.print(theta*180.0/3.14, 4);
  SerialUSB.println("");

  float temperature = read_temperature_retry(10);

  SerialUSB.print("temperature=");
  SerialUSB.print(temperature, 4);
  SerialUSB.println(" °C");

  float estimated_percent = min(max(approximation(theta, temperature), 0.0), 105.0);

  SerialUSB.print("estimated_percent=");
  SerialUSB.print(estimated_percent, 4);
  SerialUSB.println(" %");

  // SerialUSB.print(" ; corrected: ");
  // Complex corrected = corrected_signal(result);
  // print_complex(corrected);


  // SerialUSB.print(" ; emis_time: ");
  // float etime = emis_time(corrected);
  // SerialUSB.print(etime*1000000.0, 4);
  // SerialUSB.print(" us");
  // SerialUSB.print(" ; phase: ");
  // float phase = (180.0f / PI) * atan2(corrected.imag, corrected.real);
  // SerialUSB.print(phase, 8);

  // SerialUSB.print(" ; phase_diff: ");
  // float diff = phase - PHASE_REF;
  // while (diff <= -180.0f) diff += 360.0f;
  // while (diff > 180.0f) diff -= 360.0f;
  // SerialUSB.print(diff, 8);
}