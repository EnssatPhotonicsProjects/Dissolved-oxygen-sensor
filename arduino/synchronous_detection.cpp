#include "synchronous_detection.h"
#include "display.h"
#define DAC_RESOLUTION 12
const float FREQUENCY = 5000.0;
const int POINTS_PER_PERIOD = 16;
const float SAMPLE_RATE = FREQUENCY * POINTS_PER_PERIOD;

const float Vmin = 0.9;
const float Vmax = 1.5;
const float Vref_min = 0.5;
const float Vref_max = 2.65;
const int MAX_DAC = (1 << DAC_RESOLUTION) - 1;
const int TRIG_SCALE = 2000;

// Global variables
volatile uint16_t outTable[POINTS_PER_PERIOD];
volatile int32_t sinTable_Int[POINTS_PER_PERIOD];
volatile int32_t cosTable_Int[POINTS_PER_PERIOD];
volatile uint8_t tableIndex = 0;

volatile int64_t avrg_r_accum = 0;
volatile int64_t avrg_i_accum = 0;
volatile uint32_t target_points = 0;
volatile uint32_t current_points = 0;
volatile bool collection_active = false;
volatile bool collection_done = false;
uint16_t inputSample_Sonde = 0;

// In order to start synchronous detection
void start_synchronous_detection() {
    dacc_enable_channel(DACC, 0); // Réveille le DAC
    volatile uint32_t dummy = TC0->TC_CHANNEL[0].TC_SR;
    (void)dummy;
    NVIC_ClearPendingIRQ(TC0_IRQn);
    NVIC_EnableIRQ(TC0_IRQn);
    TC_Start(TC0, 0); // Lance le Timer (la LED s'allume et clignote)
}

// In order to stop synchronous detection
void stop_synchronous_detection() {
    TC_Stop(TC0, 0); // Coupe le Timer
    NVIC_DisableIRQ(TC0_IRQn);
    volatile uint32_t dummy = TC0->TC_CHANNEL[0].TC_SR;
    (void)dummy;
    NVIC_ClearPendingIRQ(TC0_IRQn);
    
    // Extinction propre de la LED
    dacc_write_conversion_data(DACC, 0); 
    dacc_disable_channel(DACC, 0); 
}

// In order to init synchronous detection
bool init_synchronous_detection() {
    if (DACC == NULL || ADC == NULL || TC0 == NULL) {
        u8g2.drawStr(0, 58, "Synchro Detec : X");
        u8g2.sendBuffer();

        SerialUSB.println("Error : Failed to init sync detec.");
        return false;
    }

    // Signal tables
    for (int i = 0; i < POINTS_PER_PERIOD; i++) {
        float theta = 2.0 * PI * i / POINTS_PER_PERIOD;
        float s = sin(theta);
        float c = cos(theta);

        float V = Vmin + (Vmax - Vmin) * (0.5 * s + 0.5);
        outTable[i] = (uint16_t)(MAX_DAC * (V - Vref_min) / (Vref_max - Vref_min));

        sinTable_Int[i] = (int32_t)(s * TRIG_SCALE);
        cosTable_Int[i] = (int32_t)(c * TRIG_SCALE);
    }

    // Material configuration
    pmc_enable_periph_clk(ID_DACC);
    dacc_reset(DACC);
    dacc_set_transfer_mode(DACC, 0);
    dacc_set_channel_selection(DACC, 0);
    dacc_enable_channel(DACC, 0);

    pmc_enable_periph_clk(ID_ADC);
    adc_init(ADC, SystemCoreClock, ADC_FREQ_MAX, ADC_STARTUP_FAST);
    adc_configure_timing(ADC, 0, ADC_SETTLING_TIME_3, 1);
    adc_enable_channel(ADC, ADC_CHANNEL_7);
    adc_disable_interrupt(ADC, 0xFFFFFFFF);
    adc_start(ADC);

    pmc_enable_periph_clk(ID_TC0);
    uint32_t rc = 42000000 / SAMPLE_RATE;
    TC_Configure(TC0, 0, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK1);
    TC_SetRC(TC0, 0, rc);

    TC0->TC_CHANNEL[0].TC_IER = TC_IER_CPCS;
    TC0->TC_CHANNEL[0].TC_IDR = ~TC_IER_CPCS;
    
    NVIC_SetPriority(TC0_IRQn, 8);
    
    stop_synchronous_detection();

    u8g2.drawStr(0, 58, "Synchro Detec : OK");
    u8g2.sendBuffer();

    SerialUSB.println("5. init_sync_detec OK");
    return true;
}

// In order to measure in loop
Complex synchronous_detection(float average_duration) {   
    target_points = (uint32_t)ceil(average_duration*FREQUENCY)*POINTS_PER_PERIOD;

    if (target_points == 0) return {0.0, 0.0};

    avrg_r_accum = 0;
    avrg_i_accum = 0;
    current_points = 0;
    collection_done = false;

    // 1. Allumage LED et Timer
    start_synchronous_detection();
    collection_active = true;

    // 2. Attente de la fin de la collecte par l'interruption
    SerialUSB.print("Measuring : ");
    uint32_t last_display_update = millis();
    while (!collection_done) {
        uint32_t now = millis();
        // On rafraîchit la barre de progression toutes les 500 ms
        if (now - last_display_update > 500) {
            float ratio = (float)current_points/(float)target_points;
            int percent = (int)(ratio*100.0);
            
            SerialUSB.print(percent);
            SerialUSB.print(" % ");
            
            //show_measuring(percent);
            
            last_display_update = now;
        }
    }
    SerialUSB.println("-> Done");

    // 3. Extinction LED et Timer
    stop_synchronous_detection();

    // 4. Calcul mathématique
    float conversion_factor = (2.0 * 3.3) / ((float)target_points * TRIG_SCALE * 4095.0);
    Complex result;
    result.real = avrg_r_accum * conversion_factor;
    result.imag = avrg_i_accum * conversion_factor;

    SerialUSB.println(inputSample_Sonde);
    return result;
}



// === Routine d'Interruption (ISR) ===
void TC0_Handler() {
    volatile uint32_t status = TC0->TC_CHANNEL[0].TC_SR;
    (void)status;

    // Écriture immédiate du DAC (Allume la LED suivant la sinusoïde)
    DACC->DACC_CDR = outTable[tableIndex];

    // Lecture ADC
    uint16_t inputSample = ADC->ADC_CDR[7];
    ADC->ADC_CR = ADC_CR_START;
    inputSample_Sonde = inputSample;
    // Accumulation des données
    if (collection_active) {
        avrg_r_accum += (int64_t)inputSample * cosTable_Int[tableIndex];
        avrg_i_accum += (int64_t)inputSample * sinTable_Int[tableIndex];
        
        current_points++;
        if (current_points >= target_points) {
            collection_active = false;
            collection_done = true; // Signale la fin à la boucle while
        }
    }

    // Gestion de l'index de la sinusoïde
    tableIndex++;
    if (tableIndex >= POINTS_PER_PERIOD) {
        tableIndex = 0;
    }
}