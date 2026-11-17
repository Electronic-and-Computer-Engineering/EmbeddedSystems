/*
 * halADC12.h
 *
 * Vorgegeben. Die Struktur ist der Vertrag zwischen ADC, DMA und Driver Layer:
 * der ADC fuellt ADC12MEM0..3, die DMA kopiert daraus nach ADCBuffer,
 * die DMA-ISR mittelt nach ADCFiltered.
 */

#ifndef HAL_HALADC12_H_
#define HAL_HALADC12_H_

#include <inttypes.h>

#define ADC_CHANNELS        4           // A0..A3, eine Sequenz
#define ADC_MAX_VALUE       4095        // 12 Bit
#define ADC_VREF_MV         2500        // interne Referenz, 2,5 V

// Reihenfolge in der Sequenz und damit auch im Puffer
#define ADC_CH_DIST_RIGHT   0           // A0
#define ADC_CH_DIST_LEFT    1           // A1
#define ADC_CH_DIST_FRONT   2           // A2
#define ADC_CH_VBAT         3           // A3

typedef struct {
    union {
        unsigned char R;
        struct {
            unsigned char ADCrdy:1;     // 1 = seit dem letzten Lesen kam ein neuer Satz
            unsigned char dummy:7;
        } B;
    } Status;

    unsigned int ADCBuffer[ADC_CHANNELS];       // schreibt die DMA
    unsigned int ADCFiltered[ADC_CHANNELS];     // gemittelt in der DMA-ISR

} ADC12Com;

void halADC12Init(void);
unsigned int halADC12GetValue(unsigned char channel);

#endif /* HAL_HALADC12_H_ */
