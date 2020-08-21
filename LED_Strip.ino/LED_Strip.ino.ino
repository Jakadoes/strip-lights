#include <FastLED.h>
#include "striplib.h"
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
#define LED_PIN 7
int IRData = 11;

//**MODE 4 VARIABLES **
int sunPos = 0; //sun starts in center
int nearRayDist = 2;
long sunTime = 2304000;
int sunsetPurpleDist = 36;
bool timeSettingMode = false;

//******Objects**********
#define NUM_LEDS 49 //set number of LEDs in the strip
CRGB leds[NUM_LEDS];
IRrecv irrecv(IRData);  // create instance of 'irrecv'
decode_results results; // create instance of 'decode_results'
int r, g, b;

//cars mode objects
CRGB tempColours4[3] = {CRGB(100, 100, 100), CRGB(0, 0, 0), CRGB(100, 100, 100)};
LightGroup finishLine(tempColours4, 3, 3, leds);
CRGB tempColours[1] = {CRGB(255, 0, 0)};
LightGroup mqueen(tempColours, 1, 0, leds);
CRGB tempColours2[1] = {CRGB(0, 200, 0)};
LightGroup chick(tempColours2, 1, 1, leds);
CRGB tempColours3[1] = {CRGB(0, 0, 200)}; //WARNING: only group array and object definitions together, otherwise there may be a weird error
LightGroup king(tempColours3, 1, 2, leds);

//*****End Objects*****

//*****PARAMETERS*****
int Mode = 5; //set starting mode
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
  case 0xFFE21D:
    Serial.println("FUNC/STOP");
    break;
  case 0xFF629D:
    Serial.println("VOL+");
    break;
  case 0xFF22DD:
    Serial.println("FAST BACK");
    break;
  case 0xFF02FD:
    Serial.println("PAUSE");
    break;
  case 0xFFC23D:
    Serial.println("FAST FORWARD");
    break;
  case 0xFFE01F:
    Serial.println("DOWN");
    sunPos -= 3;
    break;
  case 0xFFA857:
    Serial.println("VOL-");
    break;
  case 0xFF906F:
    Serial.println("UP");
    sunPos += 3;
    break;
  case 0xFF9867:
    Serial.println("EQ");
    break;
  case 0xFFB04F:
    Serial.println("ST/REPT");
    break;
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
  case 0xFF10EF:
    Serial.println("4");
    Mode = 4;
    break;
  case 0xFF38C7:
    Serial.println("5");
    Mode = 5;
    break;
  case 0xFF5AA5:
    Serial.println("6");
    Mode = 6;
    break;
  case 0xFF42BD:
    Serial.println("7");
    Mode = 7;
    break;
  case 0xFF4AB5:
    Serial.println("8");
    Mode = 8;
    break;
  case 0xFF52AD:
    Serial.println("9");
    break;
  case 0xFFFFFFFF:
    Serial.println(" REPEAT");
    break;

  default:
    Serial.println(" other button / couldnt read  ");
    Mode = Mode +1;
    break;
  } // End Case

  delay(500); // Do not get immediate repeat
} //end translate IR

//**MODE 4 FUNCTIONS (variables are above^^)**
CRGB sunRiseColour = CRGB(252, 50, 3);
CRGB sunSetColour = CRGB(252, 90, 3);
CRGB sunNormalColour = CRGB(255, 180, 5);
CRGB sunColour = sunRiseColour;
CRGB riseToNormalColour = CRGB(sunNormalColour.red - sunRiseColour.red, sunNormalColour.green - sunRiseColour.green, sunNormalColour.blue - sunRiseColour.blue);
float riseToNormalMag = sqrt((float)pow(riseToNormalColour.red, 2) + (float)pow(riseToNormalColour.green, 2) + (float)pow(riseToNormalColour.blue, 2));
CRGB riseToNormalDir = CRGB((float)riseToNormalColour.red / (riseToNormalMag), (float)riseToNormalColour.green / (riseToNormalMag), (float)riseToNormalColour.blue / (riseToNormalMag));
CRGB nearRayColourSunrise = CRGB(250, 25, 5);
CRGB nearRayColourSunset = CRGB(250, 100, 50);
CRGB farRayColourSunset = CRGB(33, 8, 3); //dim orange
CRGB skyColour = CRGB(25, 209, 255);
CRGB sunsetPurpleColour = CRGB(20, 3, 20);

int CycleSun(int sunPos)
{ //function that is called at the end of every sun cycle
  if (sunPos < 5)
    sunColour = CRGB(sunColour.red + (float)riseToNormalDir.red / 3, sunColour.green + (float)riseToNormalDir.green / 3, sunColour.blue + (float)riseToNormalDir.blue / 3);
  else
  {
    sunColour = sunNormalColour;
  }
  sunPos = sunPos + 1;
  if (sunPos > NUM_LEDS || sunPos < 0)
  {
    sunPos = 0;
  }
  return sunPos;
}

//****END FUNCTIONS****
//*************SETUP****************
void setup()
{
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  pinMode(3, INPUT_PULLUP);
  Serial.begin(9600);
  //Serial.println("IR Receiver Button Decode");
  irrecv.enableIRIn(); // Start the receiver

  //read this to understand issues with interrupt better:https://github.com/FastLED/FastLED/wiki/Interrupt-problems
} //************end setup()***********

void loop()
{
  //Check for IR signal
  if (irrecv.decode(&results)) // have we received an IR signal?
  {
    translateIR();
    irrecv.resume(); // receive the next value
  }

  if (ON)
  {
    //Mode 6 - rainbow
    //clear previous lights
    if (Mode == 6)
    {
      Rainbow();
    }
    //Mode 5 - Lightning mqueen
    else if (Mode == 5)
    {
      //clear previous lights
      if (ModeAlreadyRan == 0)
      {
        for (int i = 0; i < NUM_LEDS; i++)
        {
          leds[i] = CRGB(0, 0, 0);
        }
        FastLED.show();
        CheckForModeChange(100);
        ModeAlreadyRan = 1;
      }
      Cars();
    } //end mode 5
    //Mode 4 - sashas's VISION - sunrise / set
    else if (Mode == 4)
    {
      if (ModeAlreadyRan == 0)
      {

        //ModeAlreadyRan = 1;
      }

      //PHASE 1: SUN ENTRACE
      if (sunPos < 5)
      {
        for (int i = 0; i < 49; i++)
        {
          //set sun
          if (i == sunPos)
            leds[i] = sunColour;
          //set near rays
          else if (abs(sunPos - i) <= nearRayDist)
          {
            leds[i] = nearRayColourSunrise;
          }
          //set darkness
          else
            leds[i] = CRGB(0, 0, 0);
        } //end phase 1
      }
      //PHASE 2: Sky FILL (gradient)
      else if (sunPos < 10)
      {
        for (int i = 0; i < 49; i++)
        {
          if (i == sunPos)
            leds[i] = sunColour;
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

        } //end phase 2
      }
      //PHASE 3: DAYTIME
      else if (sunPos < (NUM_LEDS * 3) / 4)
      {
        for (int i = 0; i < 49; i++)
        {
          if (i == sunPos)
            leds[i] = sunColour;
          else
            leds[i] = skyColour;
          ModeAlreadyRan = 1;
        } //end phase 3
      }
      //PHASE 4: SUNSET
      else if (sunPos < (NUM_LEDS - 5))
      {
        //set colours
        for (int i = 0; i < 49; i++)
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
      } //end phase 4
      //PHASE 5: SUN exit
      else
      {
        for (int i = 0; i < 49; i++)
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
        } //end phase 5
      }

      FastLED.show();
      sunPos = CycleSun(sunPos); //move sun
      int posholder = sunPos;    //check for change during wait
      for (int i = 0; i < sunTime / 1000; i++)
      {
        CheckForModeChange(1000);
        if (posholder != sunPos)
        {
          break;
        }
      }
    } //end mode 4

    //Mode 3 - Canada day mode
    else if (Mode == 3)
    {
      CanadaDay();
    }
    //Mode 2 - dim mode
    else if (Mode == 2)
    {
      DimMode();
    }
    //Mode 1 - pink lights
    else if (Mode == 1)
    {
     PinkLights(); 
    }

    //Mode 0 - flashing lights
    else if (Mode == 0)
    {
      FlashingLights();
    } //end mode 0
    else
    {
      Serial.println("WARNING, ILLEGITIMATE MODE SELECTED, REVERTING TO MODE 0 (make sure any new modes added follow the if/ else if structure");
      Mode = 0;
    }
    
  }   //end if(ON)
} //end loop

bool CheckForModeChange(int t_delay)
{
  //check for a mode change while delaying
  for (int i = 0; i < t_delay; i++)
  {
    if (irrecv.decode(&results)) // have we received an IR signal?
    {
      translateIR();
      irrecv.resume(); // receive the next value
      return true;
    }
    delay(1);
  }
  return false;
}

void PowerButton()
{
  ON ^= 1;
  Serial.println(ON);
  for (int i = 0; i <= NUM_LEDS; i++)
  {
    leds[i] = CRGB(0, 0, 0);
    FastLED.show();
  }
}

int Rainbow(){
  //TotalRequiredPositions = 1275 (5*255)
  bool modeChanged = false; 
  if (ModeAlreadyRan == 0)
      {
        for (int i = 0; i < NUM_LEDS; i++)
        {
          leds[i] = CRGB(0, 0, 0);
        }
        FastLED.show();
        modeChanged = CheckForModeChange(100);
        if(modeChanged){return 0;}
        ModeAlreadyRan = 1;
      }
      int colourCal = 20; //add artificial scaling for tweaking
      int lightDelta = 1275 / NUM_LEDS;
      int leds_stages = NUM_LEDS / 5;
      leds[0] = CRGB(255, 0, 0);
      //follow colour picker hue, discretized
      for (int i = 1; i < NUM_LEDS; i++)
      {
        if (i < leds_stages * 1 && !(leds[i - 1].g > 255 - lightDelta)) //increase green
        {                                                               //dont overflow colour mutation
          leds[i] = CRGB(leds[i - 1].r, leds[i - 1].g + lightDelta, leds[i - 1].b);
        }
        else if (i < leds_stages * 2 && !(leds[i - 1].r < lightDelta)) //decrease red
        {
          leds[i] = CRGB(leds[i - 1].r - lightDelta, leds[i - 1].g, leds[i - 1].b);
        }
        else if (i < leds_stages * 3 && !(leds[i - 1].b > 255 - lightDelta)) //increase blue
        {
          leds[i] = CRGB(leds[i - 1].r, leds[i - 1].g, leds[i - 1].b + lightDelta);
        }
        else if (i < leds_stages * 4 && !(leds[i - 1].g < lightDelta)) //decrease green
        {
          leds[i] = CRGB(leds[i - 1].r, leds[i - 1].g - lightDelta, leds[i - 1].b);
        }
        else if (i < leds_stages * 7 && !(leds[i - 1].r > 255 - lightDelta)) //increase red
        {
          leds[i] = CRGB(leds[i - 1].r + lightDelta, leds[i - 1].g, leds[i - 1].b);
        }
      }
      FastLED.show();
      modeChanged =  CheckForModeChange(100);
      if(modeChanged){return 0;}
}

int Cars()
{
  //make 3 cars on starting line
  finishLine.Move(leds, 33);
  mqueen.Move(leds, finishLine.position - 1);
  chick.Move(leds, finishLine.position - 2);
  king.Move(leds, finishLine.position - 3);
  LightGroup::RedrawGroups(leds);
  bool modeChanged = CheckForModeChange(300);
  if(modeChanged){return 0;}

  //count down to race start
  int countdown_i = finishLine.position + 4;
  leds[countdown_i] = CRGB(255, 150, 0);
  leds[countdown_i + 1] = CRGB(255, 150, 0);
  leds[countdown_i + 2] = CRGB(255, 150, 0);
  FastLED.show();
  leds[countdown_i] = CRGB(0, 0, 0);
  FastLED.show();
  CheckForModeChange(1000);
  leds[countdown_i + 1] = CRGB(0, 0, 0);
  FastLED.show();
  CheckForModeChange(1000);
  leds[countdown_i + 2] = CRGB(0, 0, 0);
  FastLED.show();
  CheckForModeChange(1000);

  //race starts, loop for 200 laps
  for (int j = 0; j < 10; j++)
  {
    for (int i = 3; i < NUM_LEDS; i++)
    {
      if (mqueen.position < NUM_LEDS - 1) //make cars wrap around led strip
      {
        mqueen.Move(leds, mqueen.position + 1);
      }
      else
      {
        mqueen.Move(leds, 0);
      }
      if (chick.position < NUM_LEDS - 1) //make cars wrap around led strip
      {
        chick.Move(leds, chick.position + 1);
      }
      else
      {
        chick.Move(leds, 0);
      }
      if (king.position < NUM_LEDS - 1) //make cars wrap around led strip
      {
        king.Move(leds, king.position + 1);
      }
      else
      {
        king.Move(leds, 0);
      }
      LightGroup::RedrawGroups(leds);
      modeChanged = CheckForModeChange(20);
      if(modeChanged){return 0;}
    }
  }
  //loop one last time, but slow
  for (int i = 10; i < 30; i++)
  {
    mqueen.Move(leds, i);
    king.Move(leds, i - 4);
    chick.Move(leds, i - 5);
    LightGroup::RedrawGroups(leds);
    modeChanged = CheckForModeChange(100);
    if(modeChanged){return 0;}
  }
  //chick hits king
  chick.Move(leds, king.position - 1);
  LightGroup::RedrawGroups(leds);
  modeChanged = CheckForModeChange(300);
  if(modeChanged){return 0;}
  for (int i = 0; i < 8; i++) //flash king red
  {
    king.ChangeColour(0, CRGB(255, 0, 0));
    LightGroup::RedrawGroups(leds);
    CheckForModeChange(100);
    king.ChangeColour(0, CRGB(10, 7, 15));
    LightGroup::RedrawGroups(leds);
    modeChanged = CheckForModeChange(100);
    if (modeChanged){return 0;}
  }
  king.ChangeColour(0, CRGB(10, 7, 15));
  LightGroup::RedrawGroups(leds);
  modeChanged = CheckForModeChange(1000);
  if (modeChanged){return 0;}
  //lightning stops
  for (int i = 0; i < 2; i++)
  {
    mqueen.Move(leds, mqueen.position + 1);
    chick.Move(leds, chick.position + 1);
    LightGroup::RedrawGroups(leds);
    if (CheckForModeChange(300))
    {
      return NULL;
    }
  }
  //chick finishes
  int tempFlash = 100;
  for (int i = 0; i < abs(chick.position - finishLine.position) + 7; i++)
  {
    chick.Move(leds, chick.position + 1);
    LightGroup::RedrawGroups(leds);
    modeChanged = CheckForModeChange(tempFlash * 2);
    if(modeChanged){return 0;}
  }
  for (int i = 0; i < 10; i++) //flash finish line gold
  {
    if (chick.position < NUM_LEDS - 2)
    {
      finishLine.ChangeColour(0, CRGB(255, 150, 0));
      finishLine.ChangeColour(2, CRGB(255, 150, 0));
      LightGroup::RedrawGroups(leds);
      modeChanged = CheckForModeChange(tempFlash);
      if(modeChanged){return 0;}
      finishLine.ChangeColour(0, CRGB(255, 255, 255));
      finishLine.ChangeColour(2, CRGB(255, 255, 255));
      LightGroup::RedrawGroups(leds);
      modeChanged = CheckForModeChange(tempFlash);
      if(modeChanged){return 0;}
      finishLine.ChangeColour(0, CRGB(255, 150, 0));
      finishLine.ChangeColour(2, CRGB(255, 150, 0));
      chick.Move(leds, chick.position + 1);
      LightGroup::RedrawGroups(leds);
      modeChanged = CheckForModeChange(tempFlash);
      if(modeChanged){return 0;}
      finishLine.ChangeColour(0, CRGB(255, 150, 0));
      finishLine.ChangeColour(2, CRGB(255, 150, 0));
      LightGroup::RedrawGroups(leds);
    }
  }
  finishLine.ChangeColour(0, CRGB(100, 100, 100));
  finishLine.ChangeColour(2, CRGB(100, 100, 100));
  LightGroup::RedrawGroups(leds);
  modeChanged =CheckForModeChange(1000);
  if(modeChanged){return 0;}
  //mqueen pushes king
  for (int i = 0; mqueen.position != (king.position - 1); i++) //back up to get king
  {
    mqueen.Move(leds, mqueen.position - 1);
    LightGroup::RedrawGroups(leds);
    CheckForModeChange(100);
  }
  CheckForModeChange(1000);
  for (int i = 0; king.position != (finishLine.position + 6); i++) //push king forward
  {
    mqueen.Move(leds, mqueen.position + 1);
    king.Move(leds, king.position + 1);
    LightGroup::RedrawGroups(leds);
    CheckForModeChange(100);
  }
  king.ChangeColour(0, CRGB(0, 0, 255));
  if (CheckForModeChange(100))
  {
    return NULL;
  }
}

int CanadaDay(){
  bool modeChanged = false;
  if (ModeAlreadyRan == 0)
      {
        //go to center
        for (int i = 0; i <= 49 / 2; i++)
        {
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
          modeChanged = CheckForModeChange(200);
          if(modeChanged){return 0;}
        }
        CheckForModeChange(2000);
        ModeAlreadyRan = 1; //do not loop

        //colour explosion
        for (int i = 49 / 2; i <= 49; i++)
        {
          if (i % 2 == 1)
          {
            leds[i] = CRGB::Red;
            leds[49 - i] = CRGB::Red;
            FastLED.show();
            modeChanged = CheckForModeChange(20);
            if(modeChanged){return 0;}
          }
          else
          {
            leds[i] = CRGB::White;
            leds[49 - i] = CRGB::White;
            FastLED.show();
            modeChanged = CheckForModeChange(20);
            if(modeChanged){return 0;}
          }
        }
      }
}

int DimMode()
{
  bool modeChanged = false;
      if(!ModeAlreadyRan)
      {
        for (int i = 0; i <= NUM_LEDS; i++)
        {
          leds[i] = CRGB(0,0,0);
        }
      }

      for (int i = 0; i <= NUM_LEDS; i += 9)
      {
        leds[i] = CRGB(65, 15, 0);
        FastLED.show();
      }
      modeChanged = CheckForModeChange(100);
      if(modeChanged){return 0;}
}

int FlashingLights(){
  leds[40] = CRGB(0, 0, 0);
      leds[39] = CRGB(0, 0, 0);
      FastLED.show();
      r = 150, g = 40, b = 0;
      for (int i = 3; i <= 49; i++)
      {
        //create pellets
        for (int i = 3; i <= 40; i += 3)
        {
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
}

int PinkLights()
{
  bool modeChanged = CheckForModeChange(0);
  if(modeChanged){return 0;}
      for (int i = 0; i <= 49; i++)
      {
        leds[i] = CRGB::Fuchsia;
        FastLED.show();
        modeChanged = CheckForModeChange(100);
        if(modeChanged){return 0;}
      }
}


