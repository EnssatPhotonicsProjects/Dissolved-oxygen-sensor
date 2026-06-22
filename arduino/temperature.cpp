#include "temperature.h"
#include "display.h"
#define ONE_WIRE_BUS 2 

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

bool init_temperature() {
    sensors.begin();

    if (sensors.getDeviceCount()>0){
        sensors.setResolution(12); // 12 bits
        sensors.setWaitForConversion(false); 
        
        u8g2.drawStr(0, 26, "Temperature : OK");
        u8g2.sendBuffer();

        SerialUSB.println("3. init_temperature OK");
        return true;
    }
    else{
        u8g2.drawStr(0, 26, "Temperature : X");
        u8g2.sendBuffer();

        SerialUSB.println("Error : Failed to init temperature.");
        return false;
    }
}

float read_temperature() {
    // 1. On coupe le sifflet du Timer de façon absolue.
    NVIC_DisableIRQ(TC0_IRQn); 
    sensors.setWaitForConversion(true); 

    // 2. On lance la mesure (le code va bloquer ici)
    sensors.requestTemperatures(); 
    
    // 3. On arrache la donnée
    float temp = sensors.getTempCByIndex(0);
    
    // 4. On vide le tampon du timer pour éviter le crash, et on le rallume
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