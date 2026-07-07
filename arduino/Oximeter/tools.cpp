#include "tools.h"

// Calibration of the OXIMETER 
  /* -> enter your values */
const Complex F0 = {0.01513908, 0.01901101}; // Background noise of the circuit
const float offset = 144.0;  // Offset in phase 

// Calibration of the Oxygen Sensor Spot 
  /* -> enter your values */
const float TAU0 = 56.5e-6;
const float K_SV = 0.0214;

// In order to calculate phase shift
float calculate_theta(Complex fourier) {
    Complex corrected = fourier - F0;
    SerialUSB.print("fourrier corrected = "); print_complex(corrected); SerialUSB.println(" V");

    // Calculate and adding offset
    float theta_rad = atan2(corrected.imag, corrected.real) + (offset/180.0*PI); 
    float theta_deg = theta_rad*180.0/PI;
    SerialUSB.print("theta = "); SerialUSB.print(theta_deg, 2); SerialUSB.print(" ° ; ");

    return theta_deg;
}

// In order to estimate o2 percent with calibrated polynomial model
float calculate_o2_polynomial(float theta, float temperature) {
    float theta_rad = theta/180.0*PI;
    
    float p = -1.5272554042e+04;
    p += (8.1836636109e+04)  * theta_rad;
    p += (1.9136823848e+03)  * temperature;

    p += (-8.5902214727e+04) * pow(theta_rad, 2);
    p += (-1.4795950625e+04) * theta_rad*temperature;

    p += (-3.0583962937e+05) * pow(theta_rad, 3);
    p += (4.7073632627e+04)  * pow(theta_rad, 2)*temperature;

    p += (9.9169344938e+05)  * pow(theta_rad, 4);
    p += (-7.8973876407e+04) * pow(theta_rad, 3)*temperature;

    p += (-1.1863421751e+06) * pow(theta_rad, 5);
    p += (7.3693877465e+04)  * pow(theta_rad, 4)*temperature;

    p += (6.6651777450e+05)  * pow(theta_rad, 6);
    p += (-3.6265683118e+04) * pow(theta_rad, 5)*temperature;

    p += (-1.4668345339e+05) * pow(theta_rad, 7);
    p += (7.3541142002e+03)  * pow(theta_rad, 6)*temperature;
    
    // Result between 0% et 105%
    p = min(max(p, 0.0f), 105.0f);
    SerialUSB.print("esti_o2_percent = "); SerialUSB.print(p, 2); SerialUSB.print(" % ; ");
    
    return p;
}

// In order to estimate o2 percent with Stern-Volmer physic equation
float calculate_o2_stern_volmer(float theta) {
    float theta_rad = theta/180.0*PI;
    float tau = tan(theta_rad) / (2.0*PI*FREQUENCY);
    
    // Inverse Stern-Volmer equation
    float o2_sv_percent = ((TAU0/tau)-1.0) / K_SV;
    SerialUSB.print("sv_o2_percent = "); SerialUSB.print(o2_sv_percent, 2); SerialUSB.println(" %");

    return o2_sv_percent;
}
