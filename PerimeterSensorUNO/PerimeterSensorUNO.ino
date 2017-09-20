#include "LCD4Bit_mod.h"
#include "MatchedFilter.h"

//#define SHOWONLCD 1
//#define PRINTCORRELATIONSIGNAL 1
#define PRINTRECEIVEDSIGNAL 1
//#define PRINTQUALITY 1


// 1 when ADC interrupt should fill array MatchedFilter.empfangssignal[]. Interrupt sets this to 0 when array is filled.
volatile int readFlag = 0;
// Index für MatchedFilter.empfangssignal[] array. Wird in der ADC Intertuptroutine verwendet um das Empfangsarray zu füllen.
volatile int16_t empfangsSigIndex = 0; //Index of empfangssignal Array must set zero when start sampling => set readFlag=1

TMatchedFilter MatchedFilter;


// Define various ADC prescaler
const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);


//create object to control an LCD.
//number of lines in display USED=1
#ifdef SHOWONLCD
  LCD4Bit_mod lcd = LCD4Bit_mod(1);
#endif

//=============================================//
// Initialization
void setup() {

  Serial.begin(115200);
  //Serial.begin(19200);

  /*
    http://www.mikrocontroller.net/articles/AVR-Tutorial:_ADC
    PC5 (ADC5/SCL/PCINT13)
    PC4 (ADC4/SDA/PCINT12)
    PC3 (ADC3/PCINT11)
    PC2 (ADC2/PCINT10)= A2
    PC1 (ADC1/PCINT9) = A1
    PC0 (ADC0/PCINT8) = A0
   */


  byte channel = 5; //A5

  /*  REFS0 : VCC use as a ref, IR_AUDIO : channel selection, ADEN : ADC Enable, ADSC : ADC Start, ADATE : ADC Auto Trigger Enable, ADIE : ADC Interrupt Enable,  ADPS : ADC Prescaler  */
  // free running ADC mode, f = ( 16MHz / prescaler ) / 13 cycles per conversion
  ADMUX = _BV(REFS0) | (channel & 0x07);// | _BV(ADLAR);

  // Clear ADTS2..0 in ADCSRB (0x7B) to set trigger mode to free running.
  // This means that as soon as an ADC has finished, the next will be
  // immediately started.
  ADCSRB &= B11111000;
  //ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((channel >> 3) & 0x01) << MUX5);

  // you can choose a prescaler from above.
  // PS_16, PS_32, PS_64 or PS_128
  ADCSRA = _BV(ADSC) | _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | PS_32 ;  // prescaler 32 : 38462 Hz  //prescaler 64 : 19231 Hz // prescaler 128 : 9615 Hz


  // disable digital buffers (reduces noise/capacity)
  // For analog input pins, the digital input buffer should be disabled at all times. An analog signal
  // level close to VCC/2 on an input pin can cause significant current even in active mode. Digital
  // input buffers can be disabled by writing to the Digital Input Disable Registers (DIDR1 and DIDR0)
  DIDR0 |= (1 << channel);


  // Enable global interrupts
  // AVR macro included in <avr/interrupts.h>, which the Arduino IDE
  // supplies by default.
  sei();

  // From now on, the ADC will always sample data in free running mode. When one value is sampled, the ISR will be called.
  // Set ADSC in ADCSRA (0x7A) to start the ADC conversion
  //ADCSRA |= B01000000;

  pinMode(3, OUTPUT);
  pinMode(13, OUTPUT); // LED

  delay(5);

#ifdef SHOWONLCD
  lcd.init();
  //optionally, now set up our application-specific display settings, overriding whatever the lcd did in lcd.init()
  //lcd.commandWrite(0x0F);//cursor on, display on, blink on.  (nasty!)
  lcd.clear();
  lcd.printIn("KEYPAD testing... ");
#endif  
}


//=============================================//
// Processor loop
void loop() {


  // Start reading samples => Fill Array MatchedFilter.empfangssignal[empfangsSigIndex].
  empfangsSigIndex = 0;
  readFlag = 1;
  // Check to see if all samples are read
  while (readFlag == 1) {
  }

  // Vor Aufruf muss der ADC Interrupt das Array MatchedFilter.empfangssignal[empfangsSigIndex] füllen.
  MatchedFilter.calculate();


// show on lcd every 20 loops
// -------------------------------------
#ifdef SHOWONLCD
    char buf[10];
    lcd.clear();

    lcd.cursorTo(1, 0);  //line=1, x=0
    itoa(MatchedFilter.filterQuality * 10.0, buf, 10);
    lcd.printIn(buf);

    lcd.cursorTo(1, 8);  //line=1, x=8
    itoa( MatchedFilter.magnitude, buf, 10);
    lcd.printIn(buf);
    delay (1000);
#endif  


//#define PRINTRESULTS
#ifdef PRINTRESULTS
  Serial.print( MatchedFilter.filterQuality) ;
  Serial.print(',') ;
  Serial.println(MatchedFilter.magnitude) ;
#endif


#ifdef PRINTCORRELATIONSIGNAL
  for (int i = 0; i < CORELLATION_ARRAY_SIZE; i++)
  {
    Serial.println(MatchedFilter.correlationsignal[i]) ;
  }
  // 10x0 ausgeben, um auf Gaphen die Sampleabschnitte zu sehen.
  for(int j = 0;j<10;j++){
        Serial.println(0);
  }
#endif

#ifdef PRINTRECEIVEDSIGNAL
  for (int i = 0; i < CORELLATION_ARRAY_SIZE; i++)
  {
    Serial.println(MatchedFilter.empfangssignal[i]) ;
  }
  // 10x0 ausgeben, um auf Gaphen die Sampleabschnitte zu sehen.
  for(int j = 0;j<10;j++)
  {
        Serial.println(0);
  }
#endif

#ifdef PRINTQUALITY
    Serial.println(MatchedFilter.filterQuality) ;
#endif

}

//=============================================//
// Interrupt service routine for the ADC completion
// Der Interrupt wird immer ausgelöst: Freerunning.
// Das Einfügen der Messwerte in das MatchedFilter.empfangssignal[] Array wird mit
//     empfangsSigIndex = 0;
//     readFlag = 1;
// gestartet.
// Wenn Array gefüllt ist, wird readFlag = 0 gesetzt.

ISR(ADC_vect) {

  //volatile uint16_t analogVal;
  volatile uint16_t analogVal;
  // Must read low first
  //analogVal = ADCL | (ADCH << 8);
  analogVal = ADC;

  //analogVal = ADCH;  // Es werden nur 8 Bit verwendet, da hohe samplerate.

  // Toggle port 3 to see interrupt on oszi
  //PORTD ^= ( 1 << PD3 );

  if (readFlag == 1) {
    if (empfangsSigIndex < EMPF_ARRAY_SIZE)
    {
      MatchedFilter.empfangssignal[empfangsSigIndex] = (int16_t) analogVal;
      empfangsSigIndex++;
    }
    else
    {
      // Done reading
      readFlag = 0;
    }
  }

  // Not needed because free-running mode is enabled.
  // Set ADSC in ADCSRA (0x7A) to start another ADC conversion
  // ADCSRA |= B01000000;
}





