#include <avr/sleep.h>
#include <avr/wdt.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

volatile boolean f_wdt = 1;


#define ledRed 0
#define ledGrn 1
#define ledBlu 4
#define snsPhoto 5
#define anaPhoto 0
#define minSleep 8  // 5 minutes = 36
#define maxSleep 7  // 15 minutes = 112
#define minLight 100

unsigned int RGB[3];
unsigned int intFairy;
unsigned int chirpRGB[3];
unsigned int intPhotoReading = 1024;
unsigned int intLastReading = 1024;
unsigned int intSleepCount = 0;

void setup() {
  pinMode(ledRed, OUTPUT);
  pinMode(ledGrn, OUTPUT);
  pinMode(ledBlu, OUTPUT);
  pinMode(snsPhoto, INPUT);
  
  // Boot-up sequence
  red(16);
  delay(500);
  red(0); green(16);
  delay(500);
  green(0); blue(16);
  delay(500);
  blue(0);
  
  delay(1000);
  
  // 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
  // 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
  setup_watchdog(9);
}


void loop() {
  if (f_wdt==1) {  // wait for timed out watchdog / flag is set when a watchdog timeout occurs
    f_wdt=0;       // reset flag 

    if(intSleepCount<=0) {
      // Take a reading from the photocell
      intPhotoReading = analogRead(anaPhoto);
  
      // Only run if the last reading was still bright enough
      if(intLastReading>minLight)
        Fairy();
    
      // Store the last reading for the next loop
      intLastReading = intPhotoReading;
      intLastReading = 1000;
    
      // Set how long we're going to sleep
      intSleepCount = random(minSleep, maxSleep);
    } else
      intSleepCount--;
    
    // Nite nite!
    system_sleep();
  }
}


void Fairy() {
  // Get a reading from the photocell
  intFairy = random(0, 7);
  
  switch(intFairy) {
    case 0:  // Tinkerbell = 0, 255, 0
      RGB[0] = 0; RGB[1] = 255; RGB[2] = 0;
      break;
    case 1:  // Vidia = 128, 0, 255
      RGB[0] = 128; RGB[1] = 0; RGB[2] = 255;
      break;
    case 2:  // Rosetta = 255, 0, 0
      RGB[0] = 255; RGB[1] = 0; RGB[2] = 0;
      break;
    case 3:  // Silvermist = 0, 0, 255
      RGB[0] = 0; RGB[1] = 0; RGB[2] = 255;
      break;
    case 4:  // Iridessa = 255, 255, 0
      RGB[0] = 255; RGB[1] = 255; RGB[2] = 0;
      break;
    case 5:  // Fawn = 237, 120, 6
      RGB[0] = 237; RGB[1] = 120; RGB[2] = 6;
      break;
    case 6:  // Chloe = 175, 75, 148
      RGB[0] = 175; RGB[1] = 75; RGB[2] = 148;
      break;
    case 7:  // Periwinkle = 128, 128, 255
      RGB[0] = 128; RGB[1] = 128; RGB[2] = 255;
      break;
    default:
      RGB[0] = 128; RGB[1] = 128; RGB[2] = 128;
  }
  
  // Fade in
  fadeIn(RGB, 10, 0, 100);
  
  // Hold it for 2-5 seconds
  delay(random(2,5)*1000);
  
  // Talk a little bit
  chatter(RGB, random(3,8), 1, 100);
  
  // Fade out
  fadeOut(RGB, 10, 0, 100);
  red(0); green(0); blue(0);
  
  // Wait 1-2 seconds
  delay(random(1,2)*1000);
  
  // Chirp!
  chirpRGB[0] = round(RGB[0]/30);
  chirpRGB[1] = round(RGB[1]/30);
  chirpRGB[2] = round(RGB[2]/30);
  chirp(chirpRGB, random(2,3), 200);
  
  // Silence!
  red(0); green(0); blue(0);
}


void fadeIn(unsigned int arr[3], unsigned int intDelay, unsigned int intMin, unsigned int intMax) {
  for(unsigned int i=intMin; i<=intMax; i++) {
    red(i*round(arr[0]/100));
    green(i*round(arr[1]/100));
    blue(i*round(arr[2]/100));
    
    delay(intDelay);
  }
}


void fadeOut(unsigned int arr[3], unsigned int intDelay, unsigned int intMin, unsigned int intMax) {
  for(unsigned int i=intMax; i>intMin; i--) {
    red(i*round(arr[0]/100));
    green(i*round(arr[1]/100));
    blue(i*round(arr[2]/100));
    
    delay(intDelay);
  }
}


void chirp(unsigned int arr[3], unsigned int intCount, unsigned int intDelay) {
  red(0); green(0); blue(0);
  delay(intDelay);
  
  for(unsigned int i = 0; i<intCount; i++) {
    red(arr[0]); green(arr[1]); blue(arr[2]);
    delay(intDelay);
  
    red(0); green(0); blue(0);
    delay(intDelay);
  }
}


void chatter(unsigned int arr[3], unsigned int intCount, unsigned int intDelay1, unsigned int intDelay2) {
  unsigned int chat[3];
  
  for(unsigned int i=0; i<intCount; i++) {
    fadeOut(arr, intDelay1, 20, 100);
    fadeIn(arr, intDelay1, 20, 100);
    delay(intDelay2);
  }
}


void red(unsigned int val) {
  analogWrite(ledRed, val);
}


void green(unsigned int val) {
  analogWrite(ledGrn, val);
}


void blue(unsigned int val) {
  analogWrite(ledBlu, val);
}


// set system into the sleep state 
// system wakes up when wtchdog is timed out
void system_sleep() {
  cbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter OFF

  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();

  sleep_mode();                        // System sleeps here

  sleep_disable();                     // System continues execution here when watchdog timed out 
  sbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter ON
}

// 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
// 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
void setup_watchdog(int ii) {

  byte bb;
  int ww;
  if (ii > 9 ) ii=9;
  bb=ii & 7;
  if (ii > 7) bb|= (1<<5);
  bb|= (1<<WDCE);
  ww=bb;

  MCUSR &= ~(1<<WDRF);
  // start timed sequence
  WDTCR |= (1<<WDCE) | (1<<WDE);
  // set new watchdog timeout value
  WDTCR = bb;
  WDTCR |= _BV(WDIE);
}
  
// Watchdog Interrupt Service / is executed when watchdog timed out
ISR(WDT_vect) {
  f_wdt=1;  // set global flag
}
