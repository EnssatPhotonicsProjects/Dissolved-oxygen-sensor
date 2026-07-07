#include "temperature.h"
#define ONE_WIRE_BUS 2 

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float old_temp = 0.0;

// In order to init temperature
bool init_temperature() {
    sensors.begin();

    if (sensors.getDeviceCount()>0){
        sensors.setResolution(12); // 12 bits
        sensors.setWaitForConversion(false); 
        
        u8g2.drawStr(0, 26, "Temperature : OK");
        u8g2.sendBuffer();

        SerialUSB.println("2. init_temperature OK");
        return true;
    }
    else{
        u8g2.drawStr(0, 26, "Temperature : X");
        u8g2.sendBuffer();

        SerialUSB.println("Error : Failed to init temperature.");
        return false;
    }
}

// In order to measure temperature
float read_temperature() {
    // Turn off timer's beep
    NVIC_DisableIRQ(TC0_IRQn); 
    sensors.setWaitForConversion(true); 

    // Start measurement
    sensors.requestTemperatures(); 
    
    // Extract data
    float temp = sensors.getTempCByIndex(0);
    
    volatile uint32_t dummy = TC0->TC_CHANNEL[0].TC_SR;
    (void)dummy;
    NVIC_ClearPendingIRQ(TC0_IRQn);
    NVIC_EnableIRQ(TC0_IRQn);
    
    return temp;
}

// In order to be able to retry 
float read_temperature_retry(uint n_retry) {
    float temp = -127;
    int retries = 0;

    while (temp <= -100.0 && retries < n_retry) {
        delay(500);
        SerialUSB.print("temperature try : "); SerialUSB.print(retries+1);

        temp = read_temperature();
        retries++;
    }

    if (temp <= -100){
        //SerialUSB.println("fail :(");
        temp = old_temp;
    }

    old_temp = temp;
    SerialUSB.print(" -> temperature = "); SerialUSB.print(temp, 2); SerialUSB.println(" °C");
    
    return temp;
}