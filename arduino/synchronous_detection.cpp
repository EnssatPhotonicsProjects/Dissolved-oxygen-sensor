#include "synchronous_detection.h"

// --- Configuration ---
#define DAC_RESOLUTION 12
const float FREQUENCY = 5000.0;
const int POINTS_PER_PERIOD = 16;
const float SAMPLE_RATE = FREQUENCY * POINTS_PER_PERIOD;

// DAC Target Voltages
const float Vmin = 0.9;
const float Vmax = 1.5;
const float Vref_min = 0.5;
const float Vref_max = 2.65;
const int MAX_DAC = (1 << DAC_RESOLUTION) - 1;

// Integer scaling for fast ISR math
const int TRIG_SCALE = 2000;

// --- Private Global Variables ---
volatile uint16_t outTable[POINTS_PER_PERIOD];
volatile int32_t sinTable_Int[POINTS_PER_PERIOD];
volatile int32_t cosTable_Int[POINTS_PER_PERIOD];

volatile uint8_t tableIndex = 0;

// Variables for dynamic accumulation
volatile int64_t avrg_r_accum = 0;
volatile int64_t avrg_i_accum = 0;
volatile uint32_t target_points = 0;
volatile uint32_t current_points = 0;
volatile bool collection_active = false;
volatile bool collection_done = false;

void init_synchronous_detection()
{
    // 1. Calculate tables
    for (int i = 0; i < POINTS_PER_PERIOD; i++)
    {
        float theta = 2.0 * PI * i / POINTS_PER_PERIOD;
        float s = sin(theta);
        float c = cos(theta);

        float V = Vmin + (Vmax - Vmin) * (0.5 * s + 0.5);
        outTable[i] = (uint16_t)(MAX_DAC * (V - Vref_min) / (Vref_max - Vref_min));

        sinTable_Int[i] = (int32_t)(s * TRIG_SCALE);
        cosTable_Int[i] = (int32_t)(c * TRIG_SCALE);
    }

    // 2. Configure DAC
    pmc_enable_periph_clk(ID_DACC);
    dacc_reset(DACC);
    dacc_set_transfer_mode(DACC, 0);
    dacc_set_channel_selection(DACC, 0);
    dacc_enable_channel(DACC, 0);

    // 3. Configure ADC
    pmc_enable_periph_clk(ID_ADC);
    adc_init(ADC, SystemCoreClock, ADC_FREQ_MAX, ADC_STARTUP_FAST);
    adc_configure_timing(ADC, 0, ADC_SETTLING_TIME_3, 1);
    adc_enable_channel(ADC, ADC_CHANNEL_7);
    adc_disable_interrupt(ADC, 0xFFFFFFFF);
    adc_start(ADC);

    // 4. Configure Timer (but do NOT start it yet)
    pmc_enable_periph_clk(ID_TC0);
    uint32_t rc = 42000000 / SAMPLE_RATE;
    TC_Configure(TC0, 0, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK1);
    TC_SetRC(TC0, 0, rc);

    TC0->TC_CHANNEL[0].TC_IER = TC_IER_CPCS;
    TC0->TC_CHANNEL[0].TC_IDR = ~TC_IER_CPCS;
    
    // On configure l'interruption dans le processeur UNE SEULE FOIS ici
    NVIC_SetPriority(TC0_IRQn, 15);
    NVIC_EnableIRQ(TC0_IRQn);
    
    // On lance le Timer pour toujours
    TC_Start(TC0, 0);
}

void start_synchronous_detection() {
    // 1. On s'assure que le tuyau est vide avant d'allumer l'eau
    volatile uint32_t dummy = TC0->TC_CHANNEL[0].TC_SR;
    (void)dummy;
    NVIC_ClearPendingIRQ(TC0_IRQn);
    
    // 2. On remet le courant sur l'horloge (TC_Start intègre un "Software Trigger" 
    // qui remet automatiquement le compteur à 0 de façon propre)
    TC_Start(TC0, 0);
}

void stop_synchronous_detection() {
    // 1. Coupe l'alimentation de l'horloge du Timer (il se fige sur place)
    TC_Stop(TC0, 0);
    
    // 2. On vide le tout dernier "tic" qui a pu se glisser à la milliseconde de l'arrêt
    volatile uint32_t dummy = TC0->TC_CHANNEL[0].TC_SR;
    (void)dummy;
    NVIC_ClearPendingIRQ(TC0_IRQn);
}

Complex synchronous_detection(float average_duration)
{
    // Calculate how many points we need based on the requested duration
    target_points = (uint32_t)ceil(average_duration * FREQUENCY) * POINTS_PER_PERIOD;

    if (target_points == 0)
    {
        return {0.0, 0.0};
    }

    // Reset accumulators and flags
    avrg_r_accum = 0;
    avrg_i_accum = 0;
    current_points = 0;
    collection_done = false;

    // Signal the ISR to start collecting data
    collection_active = true;

    // Chronomètre de sécurité

    while (!collection_done) {
    }

    // --- Convert raw integer math to Real Volts ---
    // The ADC reads 0-4095 for 0-3.3V.
    // We multiply by a lock-in factor of 2.0 to extract the amplitude.
    // Factor = (2.0 * 3.3V) / (target_points * TRIG_SCALE * 4095.0)

    float conversion_factor = (2.0 * 3.3) / ((float)target_points * TRIG_SCALE * 4095.0);

    Complex result;
    result.real = avrg_r_accum * conversion_factor;
    result.imag = avrg_i_accum * conversion_factor;

    return result;
}

// --- Interrupt Handler ---
void TC0_Handler() {
    volatile uint32_t status = TC0->TC_CHANNEL[0].TC_SR;
    (void)status;

    // Ecriture DAC
    DACC->DACC_CDR = outTable[tableIndex];

    // Lecture ADC
    uint16_t inputSample = ADC->ADC_CDR[7];
    ADC->ADC_CR = ADC_CR_START;

    // Accumulation
    if (collection_active) {
        avrg_r_accum += (int64_t)inputSample * cosTable_Int[tableIndex];
        avrg_i_accum += (int64_t)inputSample * sinTable_Int[tableIndex];
        
        current_points++;
        if (current_points >= target_points) {
            collection_active = false;
            collection_done = true;
        }
    }

    // Gestion index
    tableIndex++;
    if (tableIndex >= POINTS_PER_PERIOD) {
        tableIndex = 0;
    }
}