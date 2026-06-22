// CREATION OF THE COMPLEX STRUCTURE

#ifndef COMPLEX_H
#define COMPLEX_H

struct Complex {
    float real;
    float imag;
};

Complex operator+(const Complex& a, const Complex& b);
Complex operator-(const Complex& a, const Complex& b);
Complex operator*(const Complex& a, const Complex& b);
Complex operator/(const Complex& a, const Complex& b);

#endif