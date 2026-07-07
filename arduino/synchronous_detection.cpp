#include "synchronous_detection.h"
#define DAC_RESOLUTION 12

// Parameters for emitting signal
const float FREQUENCY = 5000.0;
const int POINTS_PER_PERIOD = 16;
const float SAMPLE_RATE = FREQUENCY * POINTS_PER_PERIOD;
const float Vmin = 0.9;
const float Vmax = 1.5;
const float Vref_min = 0.55; // 3.3*(1/6) = 0.55 V, limitation between 0 and 3.3V
const float Vref_max = 2.75; // 3.3*(5/6) = 2.75 V
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
    dacc_enable_channel(DACC, 0); // DAC on 

    volatile uint32_t dummy = TC0->TC_CHANNEL[0].TC_SR;
    (void)dummy;
    NVIC_ClearPendingIRQ(TC0_IRQn);
    NVIC_EnableIRQ(TC0_IRQn);

    TC_Start(TC0, 0); // Launch Timer -> LED on
}

// In order to stop synchronous detection
void stop_synchronous_detection() {
    TC_Stop(TC0, 0); // Break Timer

    NVIC_DisableIRQ(TC0_IRQn);
    volatile uint32_t dummy = TC0->TC_CHANNEL[0].TC_SR;
    (void)dummy;
    NVIC_ClearPendingIRQ(TC0_IRQn);

    dacc_write_conversion_data(DACC, 0); // LED off
    dacc_disable_channel(DACC, 0); // DAC off
}

// In order to init synchronous detection
bool init_synchronous_detection() {
    if (DACC == NULL || ADC == NULL || TC0 == NULL) {
        u8g2.drawStr(0, 58, "Synchro Detec : X");
        u8g2.sendBuffer();

        SerialUSB.println("Error : Failed to init sync detec.");
        return false;
    }

    // Creation of signal tables
    for (int i = 0; i < POINTS_PER_PERIOD; i++) {
        float theta = 2.0 * PI * i / POINTS_PER_PERIOD;
        float s = sin(theta);
        float c = cos(theta);

        float V = Vmin + (Vmax-Vmin) * (0.5*s+0.5);
        outTable[i] = (uint16_t)(MAX_DAC*(V-Vref_min) / (Vref_max-Vref_min));

        sinTable_Int[i] = (int32_t)(s*TRIG_SCALE);
        cosTable_Int[i] = (int32_t)(c*TRIG_SCALE);
    }

    // Material configuration -> DAC, ADC, Timer
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
    uint32_t rc = 42000000/SAMPLE_RATE;
    TC_Configure(TC0, 0, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK1);
    TC_SetRC(TC0, 0, rc);
    TC0->TC_CHANNEL[0].TC_IER = TC_IER_CPCS;
    TC0->TC_CHANNEL[0].TC_IDR = ~TC_IER_CPCS;
    
    NVIC_SetPriority(TC0_IRQn, 8);
    
    // End of init
    //stop_synchronous_detection();

    u8g2.drawStr(0, 58, "Synchro Detec : OK");
    u8g2.sendBuffer();

    SerialUSB.println("4. init_sync_detec OK");
    return true;
}

// In order to measure in loop
Complex synchronous_detection(float average_duration) { 
    // Number of measurement points with the given delta t and signal parameters 
    target_points = (uint32_t)ceil(average_duration*FREQUENCY)*POINTS_PER_PERIOD;
    SerialUSB.print("target_points = "); SerialUSB.print(target_points); 
    SerialUSB.print(" in "); SerialUSB.print(average_duration, 1); SerialUSB.println(" s");
    if (target_points == 0) return {0.0, 0.0};

    // Useful variables
    avrg_r_accum = 0;
    avrg_i_accum = 0;
    current_points = 0;
    collection_done = false;

    // LED on and Timer on -> beginning of collection
    start_synchronous_detection();
    collection_active = true;
    SerialUSB.print("Measuring : ");
    
    // Waiting for the collection to finish via the ISR interrupt
    uint32_t last_display_update = millis();
    while (!collection_done) {
        uint32_t now = millis();
        // Progress bar refreshed every 500 ms
        if (now - last_display_update > 500) {
            float ratio = (float)current_points/(float)target_points;
            int percent = (int)(ratio*100.0);
            
            SerialUSB.print(percent); SerialUSB.print(" % ");

            show_measuring(percent);
            last_display_update = now;
        }
    }

    // LED off and Timer off -> end of collection
    stop_synchronous_detection();
    SerialUSB.println("-> Done");

    // Results
    // Conversion to true voltage (*3.3/4095), averaging (*target_points), s/c scale cancellation (*TRIG_SCALE), synch demodulation (*2.0)
    float conversion_factor = (2.0*3.3) / ((float)target_points*TRIG_SCALE*4095.0);
    Complex result;
        result.real = avrg_r_accum * conversion_factor;
        result.imag = avrg_i_accum * conversion_factor;

    SerialUSB.print("ADC read = "); SerialUSB.println(inputSample_Sonde); // Debug ADC read
    SerialUSB.print("fourier = "); print_complex(result); SerialUSB.println(" V");

    return result;
}


// In order to interrupt (ISR)
void TC0_Handler() {
    volatile uint32_t status = TC0->TC_CHANNEL[0].TC_SR;
    (void)status;

    // DAC on -> signal modulation
    DACC->DACC_CDR = outTable[tableIndex];

    // ADC read
    uint16_t inputSample = ADC->ADC_CDR[7];
    ADC->ADC_CR = ADC_CR_START;
    inputSample_Sonde = inputSample; // Debug ADC read

    // Accumulate data, real and imag
    if (collection_active) {
        avrg_r_accum += (int64_t)inputSample * cosTable_Int[tableIndex];
        avrg_i_accum += (int64_t)inputSample * sinTable_Int[tableIndex];
        
        current_points++;
        if (current_points >= target_points) {
            collection_active = false;
            collection_done = true; // End for the while loop
        }
    }

    // Managing signal Index
    tableIndex++;
    if (tableIndex >= POINTS_PER_PERIOD) {
        tableIndex = 0;
    }
}