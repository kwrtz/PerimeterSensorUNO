# PerimeterSensorUNO
This is a perimeter receiver for an Arduino UNO.

Following perimtersignal must be used for using this receiver:

int8_t sigcode[] = { 1,1,-1,-1,1,-1,1,-1,-1,1,-1,1,1,-1,-1,1,-1,-1,1,-1,-1,1,1,-1 };

You can use the UNO with the Arduino LCD KeyPad Shield to show the amplitude and quality of the signal on the LCD.

The code calculates the matched filter in the time domain.
This is an easy implementation of the matched filter.

At the beginning of the code you can select what you want to see on the output.
You can use SHOWONLCD in parallel to the other. But then the results will updated only once every second

//#define SHOWONLCD 1

#define PRINTCORRELATIONSIGNAL 1

//#define PRINTRECEIVEDSIGNAL 1

//#define PRINTQUALITY 1
