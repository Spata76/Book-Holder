/* 
  IR Breakbeam sensor demo!
*/
#define  testButtonPin 3
#define LEDPin 5
#define SENSORPIN 4
#include <FastLED.h>
#define NUM_LEDS 8
CRGB leds[NUM_LEDS];


int sensorState = 0, sensorLastState=0;         // variable for reading the IR Beam Break status
int winStreak = 0;  // -7 to 7
int testModeOn = 0; 
int removedThresholdTestMode = 2; int countsPerDayTestMode = 10;   //when test mode it turned on, the a "day" is 10 sec, a "reading session" is 2 sec
int removedCounter =0; int removedThreshold = 967;    //2 for testing 967 for real use (15 min of reading)
int clockCounter =0;  long countsPerDay = 92860;        //10 for testing, 92860 for real use
int dayCounter =0;
int delay1 =1000;
int i =0;
int didIReadToday = 0;                // 0 is no, 1 is yes
int PRPin =A0; float PRVal;                              //Photoresistor
int PRThresholdHigh = 200; int PRThresholdLow = 90;   //LEDs will be full prightness over high threshold, dimmed between, off below low threshold
float percentBrightness;
int brightness = 1000;
float percentDayGone;

void setup() {
  LEDS.addLeds<WS2811,LEDPin, RGB>(leds, NUM_LEDS);
  FastLED.clear(); FastLED.show(); // reset LEDs
  // initialize the sensor pin as an input:
  pinMode(SENSORPIN, INPUT);  
  pinMode(PRPin, INPUT);   
  pinMode(testButtonPin, INPUT);
  digitalWrite(SENSORPIN, HIGH); // turn on the pullup
  testModeOn = digitalRead(testButtonPin);
  if (testModeOn ==1){                                  //if test mode is on, use very short times
    removedThreshold = removedThresholdTestMode;
    countsPerDay = countsPerDayTestMode;    
  }
  Serial.begin(9600);
}

void loop(){
sensorState = digitalRead(SENSORPIN);
PRVal =analogRead(PRPin);
percentBrightness = 1-(PRThresholdHigh-PRVal)/PRThresholdHigh;
brightness = percentBrightness * 128;
percentDayGone = clockCounter/ (float)countsPerDay;

if(didIReadToday ==1){leds[0] = CRGB::Green;}
if(didIReadToday ==0 && percentDayGone > 0.5){leds[0] = CRGB::Yellow;}

// check if the sensor beam is broken.  sensorState is LOW: Book is present
if (sensorState == LOW) {     
  
  //FastLED.show(); dont think I need this line
  if (removedCounter > removedThreshold && didIReadToday == 0)  {  //sucessfully ready book, NO DOUBLE DIPPING ALLOWED!!
    didIReadToday =1;
    if(winStreak > 0){winStreak ++;} else {winStreak = 1;}
  }
removedCounter =0;
} 

  
  else {  //Book removed
    // turn LED off:
    removedCounter ++;
    
  }


clockCounter ++;

//  Display winStreak (or lose streak) as that many green or red LEDs
if (winStreak == 0){
  for (i=1; i<8;i++){leds[i] = CRGB::Black;}
}
if (winStreak >= 1 && winStreak <=7) {                  //if winstreak is 1 to 7 make that many green lights
  for (i=1; i<8;i++){leds[i] = CRGB::Black;}
  for (i=1; i<winStreak+1;i++){leds[i] = CRGB::Green;}  
}
if (winStreak >=8){                                     //if winStreak >= 8, make 7 green lights 
  for (i=1; i<8;i++){leds[i] = CRGB::Black;}
  for (i=1; i<8;i++){leds[i] = CRGB::Green;}
}
if (winStreak <= -1 && winStreak >=-7) {                  //if winstreak is -7 to -1 make that many red lights :(
  for (i=1; i<8;i++){leds[i] = CRGB::Black;}
  for (i=1; i<abs(winStreak-1);i++){leds[i] = CRGB::Red;}  
}
if (winStreak <=-8){                                     //if winStreak <= 8, make 7 red lights :(
  for (i=1; i<8;i++){leds[i] = CRGB::Black;}
  for (i=1; i<8;i++){leds[i] = CRGB::Red;}
}

if (PRVal < PRThresholdLow){FastLED.setBrightness(0);}
if (PRVal >= PRThresholdLow && PRVal <= PRThresholdHigh){FastLED.setBrightness(brightness);}
if (PRVal > PRThresholdHigh){FastLED.setBrightness(255);}

FastLED.show();
Serial.print("Clock Counter = "); Serial.print(clockCounter);
Serial.print("  Removed Counter = "); Serial.print(removedCounter);
Serial.print("  Day Counter = "); Serial.print(dayCounter); 
Serial.print("  Winstreak = "); Serial.print(winStreak);
Serial.print("  PR Value = "); Serial.print(PRVal);
Serial.print("  Percent Brightness = "); Serial.print(percentBrightness);
Serial.print("  Brightness = "); Serial.print(brightness);
Serial.print("  day Gone = "); Serial.print(percentDayGone);
Serial.print("  Test Mode = "); Serial.println(testModeOn);

if (clockCounter == countsPerDay) {                 //end of day opperations
  if (didIReadToday == 0){             
    if (winStreak >0){                
      winStreak = -1;} else{
      winStreak --;
    }
    }
didIReadToday =0 ;clockCounter = 0; dayCounter ++;  //end of day clean up
FastLED.clear();


}

delay(delay1);
}
