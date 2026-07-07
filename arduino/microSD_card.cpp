#include "microSD_card.h"
#define CS_PIN 13

static char filename[15]; 
File dataFile;

// In order to init microSD card
bool init_sd_card() {
    pinMode(CS_PIN, OUTPUT);
    digitalWrite(CS_PIN, HIGH);
    delay(100);

    if (!SD.begin(CS_PIN)) {
        u8g2.drawStr(0, 42, "MicroSD card : X");
        u8g2.sendBuffer();

        SerialUSB.println("Error : Failed to init microSD card.");
        return false;
    }

    u8g2.drawStr(0, 42, "MicroSD card : OK");
    u8g2.sendBuffer();

    SerialUSB.print("3. init_sd_card OK -> ");
    
    // File name generator
    int file_index = 0;
    while (true) {
        sprintf(filename, "m_%d.txt", file_index);
        if (!SD.exists(filename)) {
            break; // Name found
        }
        file_index++;
    }

    // File and header creation
    dataFile = SD.open(filename, FILE_WRITE);
    if (dataFile) {
        dataFile.println("Time(ms)\tTemperature(°C)\tTheta(°)\tEstimated_O2(%)\tSternVolmer_O2(%)");
        dataFile.close();
        SerialUSB.print("File created : [");
        SerialUSB.print(filename);
        SerialUSB.println("]");
        return true;
    } else {
        SerialUSB.println("Error : Failed to create file in microSD card.");
        return false;
    }
}

// In order to Write data
bool log_data(float temperature, float theta, float o2_estimated_percent, float o2_sv_percent) {
    // Open file
    dataFile = SD.open(filename, FILE_WRITE);

    // Write in file
    if(dataFile){
        //SerialUSB.print("Successful open : [");
        //SerialUSB.print(filename);
        //SerialUSB.println("]");  

        dataFile.print(millis());
        dataFile.print("\t"); // "\t" : tabulation as separator

        dataFile.print(temperature, 4);
        dataFile.print("\t");

        dataFile.print(theta, 4); 
        dataFile.print("\t");

        dataFile.print(o2_estimated_percent, 4);
        dataFile.print("\t");

        dataFile.println(o2_sv_percent, 4);

        SerialUSB.print("Successful write to : ["); SerialUSB.print(filename); SerialUSB.println("]");

        dataFile.flush(); 
        dataFile.close(); 
        return true;
    } else {
        SerialUSB.print("Error : Failed to open : ["); SerialUSB.print(filename); SerialUSB.println("]");
        SerialUSB.println("Error : Failed to write to the microSD card.");
        
        return false;
    }
}