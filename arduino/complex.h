/**
 * @file complex.h
 * @brief Complex number arithmetic module for signal processing.
 -------------------------------------------------------------------------------
 * * This module defines a custom Complex structure and overloads fundamental 
 * mathematical operators (+, -, *, /). It facilitates the complex number 
 * calculations required for phase shift extraction in synchronous demodulation.
 -------------------------------------------------------------------------------
 */

#ifndef COMPLEX_H
#define COMPLEX_H

#include <Arduino.h>

struct Complex {float real; float imag;};
void print_complex(Complex x);

Complex operator+(const Complex& a, const Complex& b);
Complex operator-(const Complex& a, const Complex& b);
Complex operator*(const Complex& a, const Complex& b);
Complex operator/(const Complex& a, const Complex& b);

#endif