#include <stdint.h>
#include <stdbool.h>
#include <inc/tm4c123gh6pm.h>


//Initialize the ADC for sampling
void adc_init(void);

//Read in values for sampling
uint32_t adc_read(void);
