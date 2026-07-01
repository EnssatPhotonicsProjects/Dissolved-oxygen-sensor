#include "complex.h"

// In order to print complex structure
void print_complex(Complex x){
  SerialUSB.print(x.real, 8);
  if (x.imag >= 0) SerialUSB.print("+");
  SerialUSB.print(x.imag, 8);
  SerialUSB.print("i");
}

// Addition
Complex operator+(const Complex& a, const Complex& b){
    return{a.real+b.real, a.imag+b.imag};
}

// Soustraction
Complex operator-(const Complex& a, const Complex& b){
    return{a.real-b.real, a.imag-b.imag};
}

// Multiplication : (a+bi)*(c+di) = (ac-bd)+(ad+bc)i
Complex operator*(const Complex& a, const Complex& b){
    return{(a.real*b.real) - (a.imag*b.imag), (a.real*b.imag) + (a.imag*b.real)};
}

// Division : (a+bi) / (c+di)
Complex operator/(const Complex& a, const Complex& b){
    float denominator = (b.real*b.real) + (b.imag*b.imag);
    if (denominator == 0.0f){   // Avoid crash 
        return {0.0f, 0.0f}; 
    }
    return{((a.real*b.real)+(a.imag*b.imag)) / denominator, ((a.imag*b.real)-(a.real*b.imag)) / denominator};
}