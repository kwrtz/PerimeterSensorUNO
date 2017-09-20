#include "MatchedFilter.h"

//=============================================//
TMatchedFilter::TMatchedFilter(): sendersignal{   1, 1, 1, 1,   0, 0, 0, 0,  -1,-1,-1, -1,  0, 0, 0, 0,   1, 1, 1, 1,  -1,-1,-1,-1,  1, 1, 1, 1,  -1,-1,-1,-1,  0, 0, 0, 0,  1, 1, 1, 1,  -1,-1,-1,-1,  1, 1, 1, 1,   0, 0, 0, 0, -1,-1,-1,-1,  0, 0, 0, 0,  1, 1, 1, 1,  -1,-1,-1,-1,    0, 0, 0, 0,  1, 1, 1, 1,  -1,-1,-1,-1,    0, 0, 0, 0,  1, 1, 1, 1,   0, 0, 0, 0, -1,-1,-1,-1}{    //Arduino  - was als Eingangssignal gemessen wird => #define SENDER_ARRAY_SIZE   96 vierfach oversampling
  magnitude = 0;
  filterQuality = 0;
}


//=============================================//
void TMatchedFilter::calculate() {

  // Offset Calculation
  int32_t ADCSignalOffset = 512;// ADC zero offset
  for (int16_t j = 0;  j < EMPF_ARRAY_SIZE ; j++) {
      ADCSignalOffset +=  empfangssignal[j];
  }

  ADCSignalOffset = ADCSignalOffset / EMPF_ARRAY_SIZE;

  for (int16_t j = 0;  j < EMPF_ARRAY_SIZE ; j++) {
       empfangssignal[j] =  empfangssignal[j] - ADCSignalOffset;
  }
   
  convolve();
}

//=============================================//
// This function applies an incoming
// convolution operator (sendersignal) to an incoming set of
// data (empfangssignal) and deposits the filtered data in an
// output array (correlationssignal) whose reference is received as an
// incoming parameter.
// It calcualtes the positive or negative magnitude of the correlationsignal and determine the filter quality
// The correlationsignal is normally not used and if one need more variable space it can be comment.

void TMatchedFilter::convolve() {

  //long zeit = millis();

  int16_t sumMax = 0; // max correlation sum
  int16_t sumMin = 0; // min correlation sum

  int16_t sum = 0;
  for (int16_t i = 0; i < CORELLATION_ARRAY_SIZE; i++) {
    sum = 0;
    for (int16_t j = 0;  j < SENDER_ARRAY_SIZE ; j++) {
      sum +=  (int16_t)sendersignal[j]  * (volatile int16_t )empfangssignal[i + j];
    }//end inner loop
    correlationsignal[i] = sum;
    if (sum > sumMax) {
      sumMax = sum;
    }
    if (sum < sumMin) {
      sumMin = sum;
    }
    //Divide by the length of the operator
  }//end outer loop

  // normalize to 4095
  // Divide by the length of the operator = senderAnzEinsen
  //sumMin = ((float)sumMin) / ((float)(senderAnzEinsen * 127)) * 4095.0;
  //sumMax = ((float)sumMax) / ((float)(senderAnzEinsen * 127)) * 4095.0;


  if (sumMax > -sumMin) {
    filterQuality = ((float)sumMax) / ((float) - sumMin);
    magnitude = sumMax;
  } else {
    filterQuality = ((float) - sumMin) / ((float)sumMax);
    magnitude = sumMin;
  }

}//end convolve method









