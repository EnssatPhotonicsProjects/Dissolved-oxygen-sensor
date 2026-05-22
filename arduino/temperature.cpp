#include "temperature.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2 

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void init_temperature() {
    sensors.begin();
    sensors.setResolution(12); // Haute précision
    
    // TRÈS IMPORTANT : Permet au code de ne pas bloquer pendant le calcul
    sensors.setWaitForConversion(false); 
}

float read_temperature() {
    // 1. On coupe le sifflet du Timer de façon absolue.
    NVIC_DisableIRQ(TC0_IRQn); 

    // 2. On FORCE le reboot logiciel du capteur à chaque lecture. 
    // Si l'ADC a créé un parasite électromagnétique qui l'a fait bugger, ça le réveille.
    sensors.begin();
    sensors.setResolution(12); // Haute précision
    
    // 3. On repasse en mode bloquant. Pas de delay() intelligent. 
    // On oblige la bibliothèque à rester concentrée sur le capteur pendant 750ms.
    sensors.setWaitForConversion(true); 

    // 4. On lance la mesure (le code va bloquer ici)
    sensors.requestTemperatures(); 
    
    // 5. On arrache la donnée
    float temp = sensors.getTempCByIndex(0);
    
    // 6. On vide le tampon du timer pour éviter le crash, et on le rallume
    volatile uint32_t dummy = TC0->TC_CHANNEL[0].TC_SR;
    (void)dummy;
    NVIC_ClearPendingIRQ(TC0_IRQn);
    NVIC_EnableIRQ(TC0_IRQn);
    
    return temp;
}
float old_temp = 0.0;
float read_temperature_retry(uint n_retry) {
    float temp = -127;
    int retries = 0;
    while (temp <= -100.0 && retries < n_retry) {
        delay(1000);
        //SerialUSB.print("try: ");
        //SerialUSB.println(retries+1);
        temp = read_temperature();
        retries++;
    }
    if (temp <= -100){
        //SerialUSB.println("fail :(");
        temp = old_temp;
    }
    old_temp = temp;
    return temp;
}