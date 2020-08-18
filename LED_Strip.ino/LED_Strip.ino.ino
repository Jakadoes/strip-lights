#include <FastLED.h>
#include "IRremote.h"
//----NOTES----
/*
  IR: The IR receiver stores all information passed to it before it is unloaded. this script's
  control structure gives full access to LED setting and the CheckForModeChange() method is
  used to yield control to the IR receiver to unload it. therefore to avoid multiple signals
  being stacked and therefore intepreted as garbage, the user must press the button once only
  every couple seconds or more yield methods are needed in the script to unload it
  more often.

*/
//----END NOTES----
//*-----( Declare objects, variables and pins )-----*/
//pins
#define LED_PIN     7
int IRData = 11;

//**MODE 4 VARIABLES **
int sunPos = 0; //sun starts in center
int nearRayDist = 2;
long sunTime = 2304000;
int sunsetPurpleDist = 36;
bool timeSettingMode = false;

//objects
#define NUM_LEDS    49//set number of LEDs in the strip
CRGB leds[NUM_LEDS];
IRrecv irrecv(IRData);     // create instance of 'irrecv'
decode_results results;      // create instance of 'decode_results'
int r, g, b;

//*****PARAMETERS*****
int Mode = 6;//set starting mode
bool ON = true;
int ModeAlreadyRan = 0; //used to make intro of mode run only once

//*****END PARAMETERS*****
//****FUNCTIONS****
//process IR information
void translateIR() // takes action based on IR code received
{
  switch (results.value)
  {
    case 0xFFA25D:
      Serial.println("POWER");
      PowerButton();
      break;
    case 0xFFE21D: Serial.println("FUNC/STOP"); break;
    case 0xFF629D: Serial.println("VOL+"); break;
    case 0xFF22DD: Serial.println("FAST BACK");    break;
    case 0xFF02FD: Serial.println("PAUSE");    break;
    case 0xFFC23D: Serial.println("FAST FORWARD");   break;
    case 0xFFE01F: Serial.println("DOWN");
      sunPos -= 3;
      break;
    case 0xFFA857: Serial.println("VOL-");    break;
    case 0xFF906F: Serial.println("UP");
      sunPos += 3;
      break;
    case 0xFF9867: Serial.println("EQ");    break;
    case 0xFFB04F: Serial.println("ST/REPT"); break;
    case 0xFF6897:
      Serial.println("0");
      Mode = 0;
      break;
    case 0xFF30CF:
      Serial.println("1");
      Mode = 1;
      break;
    case 0xFF18E7:
      Serial.println("2");
      Mode = 2;
      break;
    case 0xFF7A85:
      Serial.println("3");
      Mode = 3;
      break;
    case 0xFF10EF: Serial.println("4");
      Serial.println("4");
      Mode = 4;
      break;
    case 0xFF38C7: Serial.println("5");    break;
    case 0xFF5AA5: Serial.println("6");    break;
    case 0xFF42BD: Serial.println("7");    break;
    case 0xFF4AB5: Serial.println("8");    break;
    case 0xFF52AD: Serial.println("9");    break;
    case 0xFFFFFFFF: Serial.println(" REPEAT"); break;

    default:
      Serial.println(" other button   ");
  }// End Case

  delay(500); // Do not get immediate repeat
} //end translate IR

//**MODE 4 FUNCTIONS (variables are above^^)**
CRGB sunRiseColour = CRGB(252, 50, 3);
CRGB sunSetColour = CRGB(252, 90, 3);
CRGB sunNormalColour = CRGB(255, 180, 5);
CRGB sunColour = sunRiseColour;
CRGB riseToNormalColour = CRGB(sunNormalColour.red - sunRiseColour.red, sunNormalColour.green - sunRiseColour.green, sunNormalColour.blue - sunRiseColour.blue);
float riseToNormalMag = sqrt((float) pow(riseToNormalColour.red, 2)  + (float) pow(riseToNormalColour.green, 2) +  (float) pow(riseToNormalColour.blue, 2));
CRGB riseToNormalDir = CRGB( (float) riseToNormalColour.red / (riseToNormalMag), (float)riseToNormalColour.green / (riseToNormalMag) ,  (float)riseToNormalColour.blue / (riseToNormalMag));
CRGB nearRayColourSunrise = CRGB(250, 25, 5);
CRGB nearRayColourSunset = CRGB(250, 100, 50);
CRGB farRayColourSunset = CRGB(33, 8, 3);//dim orange
CRGB skyColour = CRGB(25, 209, 255);
CRGB sunsetPurpleColour = CRGB(20, 3, 20);

int CycleSun(int sunPos) {//function that is called at the end of every sun cycle
  if (sunPos < 5)
    sunColour = CRGB(sunColour.red + (float) riseToNormalDir.red / 3, sunColour.green + (float) riseToNormalDir.green / 3, sunColour.blue + (float) riseToNormalDir.blue / 3 );
    else 
  {
    sunColour = sunNormalColour;
  }
  sunPos = sunPos + 1;
  if (sunPos > NUM_LEDS || sunPos <0) {
    sunPos = 0;
  }
  return sunPos;

}

//****END FUNCTIONS****

void setup() {
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  pinMode(3, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println("IR Receiver Button Decode");
  irrecv.enableIRIn(); // Start the receiver
  
  //**ENABLE THIS TO ALLOW FOR USER MODE CHANGE**
  //attachInterrupt(digitalPinToInterrupt(3), ChangeMode, FALLING);//make pin 3 an interrupt
  //DO NOT USE INTERRUPT until there is a 1K resistor or greater available
  //read this to understand issues with interrupt better:https://github.com/FastLED/FastLED/wiki/Interrupt-problems
}//end setup()

void loop() {
  //Check for IR signal
  if (irrecv.decode(&results)) // have we received an IR signal?
  {
    translateIR();
    irrecv.resume(); // receive the next value
  }
  if (ON) {
    //Mode 6 - rainbow
    //TotalRequiredPositions = 1275 (5*255)
    //clear previous lights
    if(Mode == 6){
      if (ModeAlreadyRan == 0) {
        for (int i = 0 ; i < NUM_LEDS ; i++) {
          leds[i] = CRGB(0,0,0);
        }
        FastLED.show();
        CheckForModeChange(100);
        ModeAlreadyRan = 1;
      }
      int colourCal = 20; //add artificial scaling for tweaking
      int lightDelta = 1275/NUM_LEDS; 
      int leds_stages = NUM_LEDS/5; 
      leds[0] = CRGB(255,0,0);
      //follow colour picker hue, discretized 
      for (int i = 1 ; i < NUM_LEDS  ; i++) {
        if(i< leds_stages * 1 && !(leds[i-1].g > 255- lightDelta)){ //dont overflow colour mutation 
        leds[i] = CRGB(leds[i-1].r,leds[i-1].g + lightDelta,leds[i-1].b);
        }
        else if(i<leds_stages * 2 && !(leds[i-1].r < lightDelta)){
        leds[i] = CRGB(leds[i-1].r - lightDelta,leds[i-1].g,leds[i-1].b);
        }
        else if(i<leds_stages * 3 && !(leds[i-1].b > 255- lightDelta)){
        leds[i] = CRGB(leds[i-1].r,leds[i-1].g,leds[i-1].b + lightDelta);
        }
        else if(i<leds_stages * 4 && !(leds[i-1].g < lightDelta)){
        leds[i] = CRGB(leds[i-1].r,leds[i-1].g - lightDelta,leds[i-1].b);
        }
        else if(i<leds_stages * 7 && !(leds[i-1].r > 255- lightDelta)){
        leds[i] = CRGB(leds[i-1].r + lightDelta,leds[i-1].g ,leds[i-1].b);
        }
      }
      FastLED.show();
      CheckForModeChange(100);
    }
    //Mode 5 - Lightning mqueen
    if (Mode == 5){
      //clear previous lights
      if (ModeAlreadyRan == 0) {
        for (int i = 0 ; i < NUM_LEDS ; i++) {
          leds[i] = CRGB(0,0,0);
        }
        FastLED.show();
        CheckForModeChange(100);
        ModeAlreadyRan = 1;
      }
      //make 3 cars on starting line
      int startline_i = 10;
      leds[startline_i] = CRGB(255,0,0);
      leds[startline_i - 1] = CRGB(0,255,0);
      leds[startline_i - 2] = CRGB(0,0,255);
      FastLED.show();
      CheckForModeChange(100);

      //count down to race start
      int countdown_i = 20;
      leds[countdown_i] = CRGB(255,200,0);
      leds[countdown_i + 1] = CRGB(255,200,0);
      leds[countdown_i + 2] = CRGB(255,200,0);
      FastLED.show();
      CheckForModeChange(1000);
      leds[countdown_i] = CRGB(0,0,0);
      FastLED.show();
      CheckForModeChange(1000);
      leds[countdown_i + 1] = CRGB(0,0,0);
      FastLED.show();
      CheckForModeChange(1000);
      leds[countdown_i + 2] = CRGB(0,0,0);
      FastLED.show();
      CheckForModeChange(1000);
      //race starts, loop for 200 laps 
      for (int i = 0 ; i < NUM_LEDS ; i++) {
        
      }



    }
    //Mode 4 - sashas's VISION - sunrise / set
    if (Mode == 4) {
      if (ModeAlreadyRan == 0) {

        //ModeAlreadyRan = 1;
      }

      //PHASE 1: SUN ENTRACE
      if (sunPos < 5) {
        for (int i = 0 ; i < 49 ; i++) {
          //set sun
          if (i == sunPos)
            leds[i] = sunColour;
          //set near rays
          else if (abs(sunPos - i) <= nearRayDist) {
            leds[i] = nearRayColourSunrise;
          }
          //set darkness
          else
            leds[i] = CRGB(0, 0, 0);
        }//end phase 1
      }
      //PHASE 2: Sky FILL (gradient)
      else if (sunPos < 10) {
        for (int i = 0 ; i < 49 ; i++) {
          if (i == sunPos)
            leds[i] = sunColour;
          else if (abs(sunPos - i) <= nearRayDist) {
            leds[i] = nearRayColourSunrise;

          }
          else if (abs(sunPos - i) >= sunsetPurpleDist)
          {
            leds[i] = sunsetPurpleColour;
          }
          else
            leds[i] = farRayColourSunset;

        }//end phase 2
      }
      //PHASE 3: DAYTIME
      else if (sunPos < (NUM_LEDS * 3) / 4) {
        for (int i = 0 ; i < 49 ; i++) {
          if (i == sunPos)
            leds[i] = sunColour;
          else
            leds[i] = skyColour;
          ModeAlreadyRan = 1;        }//end phase 3
      }
      //PHASE 4: SUNSET
      else if (sunPos < (NUM_LEDS - 5)) {
        //set colours
        for (int i = 0 ; i < 49 ; i++)
        {
          if (i == sunPos)
            leds[i] = sunSetColour;
          else if (abs(sunPos - i) <= nearRayDist)
          {
            leds[i] = nearRayColourSunrise;
          }
          else if (abs(sunPos - i) >= sunsetPurpleDist)
          {
            leds[i] = sunsetPurpleColour;
          }
          else
            leds[i] = farRayColourSunset;
        }
      }//end phase 4
      //PHASE 5: SUN exit
      else
      {
        for (int i = 0 ; i < 49 ; i++)
        {
          //set sun
          if (i == sunPos)
            leds[i] = sunSetColour;
          //set near rays
          else if (abs(sunPos - i) <= nearRayDist)
          {
            leds[i] = nearRayColourSunrise;
          }
          //set darkness
          else
            leds[i] = CRGB(0, 0, 0);
        }//end phase 5
      }

      FastLED.show();
      sunPos = CycleSun(sunPos);//move sun
      int posholder = sunPos; //check for change during wait
      for (int i = 0; i < sunTime/1000 ; i++) {
        CheckForModeChange(1000);
        if (posholder != sunPos) {
          break;
        }
      }
    }//end mode 4

    //Mode 3 - Valentines day mode
    if (Mode == 3) {
      if (ModeAlreadyRan == 0) {
        //go to center
        for (int i = 0; i <= 49 / 2; i++) {
          //red going forward
          if (i - 1 >= 0)
            leds[i - 1] = CRGB(0, 0, 0);
          leds[i] = CRGB(255, 0, 0);
          FastLED.show();


          //white moving backwards
          if (i + 1 <= 49)
            leds[49 - i + 1] = CRGB(0, 0, 0);
          leds[49 - i] = CRGB::White;
          FastLED.show();
          CheckForModeChange(200);
        }
        CheckForModeChange(2000);
        ModeAlreadyRan = 1; //do not loop

        //colour explosion
        for (int i = 49 / 2; i <= 49; i++)
        {
          if (i % 2 == 1) {
            leds[i] = CRGB::Red;
            leds[49 - i] = CRGB::Red;
            FastLED.show();
            CheckForModeChange(20);
          }
          else
          {
            leds[i] = CRGB::White;
            leds[49 - i] = CRGB::White;
            FastLED.show();
            CheckForModeChange(20);
          }
        }
      }
    }
    //Mode 2 - dim mode
    if (Mode == 2) {

      for (int i = 0; i <= 49; i += 9)
      {
        leds[i] = CRGB(65, 15, 0);
        FastLED.show();

      }
      CheckForModeChange(100);
    }
    //Mode 1 - pink lights
    if (Mode == 1) {

      CheckForModeChange(0);
      for (int i = 0; i <= 49; i++)
      {
        leds[i] = CRGB::Fuchsia;
        FastLED.show();
        CheckForModeChange(100);
      }
    }

    //Mode 0 - flashing lights
    if (Mode == 0) {
      leds[40] = CRGB(0, 0, 0);
      leds[39] = CRGB(0, 0, 0);
      FastLED.show();
      r = 150, g = 40, b = 0;
      for (int i = 3; i <= 49; i++)
      {
        //create pellets
        for (int i = 3; i <= 40; i += 3) {
          leds[i] = CRGB(15, 5, 0);
        }

        //race along track
        leds[i - 2] = CRGB(0, 0, 0);
        leds[i - 1] = CRGB(25, 0, 0);
        leds[i] = CRGB(r, g, b);
        FastLED.show();
        CheckForModeChange(100);
        g -= 1;
        b += 5;
      }
    }//end mode 0
  }//end if(ON)
}//end loop

void CheckForModeChange(int t_delay) {

  if (irrecv.decode(&results)) // have we received an IR signal?
  {
    translateIR();
    irrecv.resume(); // receive the next value
  }
  //dont use for now
  /*
    if(digitalRead(3) == HIGH){
    Mode +=1;
    if(Mode >2)
      Mode = 0;


    }
  */
  delay(t_delay);
}

void PowerButton() {
  ON ^= 1;
  Serial.println(ON);
  for (int i = 0; i <= NUM_LEDS; i++) {
    leds[i] = CRGB(0, 0, 0);
    FastLED.show();
  }
}

