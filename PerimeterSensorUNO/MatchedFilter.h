#ifndef MATCHEDFILTER_H
#define MATCHEDFILTER_H

#include <Arduino.h>


#define EMPF_ARRAY_SIZE 192 // 2x96
#define SENDER_ARRAY_SIZE   96
#define CORELLATION_ARRAY_SIZE (EMPF_ARRAY_SIZE - SENDER_ARRAY_SIZE + 1)


class TMatchedFilter
{
  public:
    //Ergebnis per Aufruf von calculate()
    int16_t magnitude;     // Amplitude des Korrelationssignals
    float filterQuality;   // Verhältnis maxAmplitude zu minAmplitude immer Positive maximal 3.2

    // Signal Aarrays
    volatile int16_t empfangssignal[EMPF_ARRAY_SIZE];  // Wird von ADC Interrupt ausgefüllt. Erst dann kann calculate oder convolve aufgerufen werden
    int8_t sendersignal[SENDER_ARRAY_SIZE];            // Statisches array. Wird nicht verändert. Enthält den Barker, Chrip, Arduion ... code der über die Schleife gesendet wird. Allerdings wird hier das empfangene Signal ohne Störungen eingegeben.
    int16_t correlationsignal[CORELLATION_ARRAY_SIZE]; // Wird von convolve ausgefüllt. Wird normalerweise nicht  benötigt, da convolve quality und sum ermittelt, was zur Bestimmung für innerhalb und ausserhalb der Schleife benötigt wird.

    TMatchedFilter();
    void calculate();

  private:

    void convolve();

};




#endif
