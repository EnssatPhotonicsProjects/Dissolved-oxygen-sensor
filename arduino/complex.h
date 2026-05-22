#ifndef COMPLEX_H
#define COMPLEX_H

struct Complex {
    float real;
    float imag;
};

// Surcharge des opérateurs mathématiques basiques
Complex operator+(const Complex& a, const Complex& b);
Complex operator-(const Complex& a, const Complex& b);
Complex operator*(const Complex& a, const Complex& b);
Complex operator/(const Complex& a, const Complex& b);

#endif