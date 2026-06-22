#include "complex.h"

// Addition
Complex operator+(const Complex& a, const Complex& b){
    return{a.real + b.real, a.imag + b.imag};
}

// Soustraction
Complex operator-(const Complex& a, const Complex& b){
    return{a.real - b.real, a.imag - b.imag};
}

// Multiplication : (a + bi) * (c + di) = (ac - bd) + (ad + bc)i
Complex operator*(const Complex& a, const Complex& b){
    return{
        (a.real * b.real) - (a.imag * b.imag),
        (a.real * b.imag) + (a.imag * b.real)
    };
}

// Division : (a + bi) / (c + di)
Complex operator/(const Complex& a, const Complex& b){
    float denominator = (b.real * b.real) + (b.imag * b.imag);
    
    // Avoid crash Due
    if (denominator == 0.0f){
        return {0.0f, 0.0f}; 
    }
    
    return{
        ((a.real * b.real) + (a.imag * b.imag)) / denominator,
        ((a.imag * b.real) - (a.real * b.imag)) / denominator
    };
}