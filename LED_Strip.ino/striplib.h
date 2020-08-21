/*
striplib is an arduino library created by jakadoes for use with the LED_Strip.ino file 
a link to the jakadoes github can be found here: https://github.com/Jakadoes/strip-lights/blob/master/LED_Strip.ino/LED_Strip.ino.ino
*/

#include <FastLED.h>
#ifndef striplib_h
#define striplib_h
#define MAX_LIGHTGROUPS 10


class LightGroup{
    public: 
        static void RedrawGroups(CRGB leds[]);
        static LightGroup* LIGHTGROUPS[MAX_LIGHTGROUPS]; 
        int static LIGHTGROUPS_USED;
         //tracks how many lightgroups have been instantiated 
        LightGroup(CRGB newColours_arr[],int newColours_len,int newPosition, CRGB leds[]);
        int position;
        void Move(CRGB leds[], int newPos);
        void ChangeColour(int light_i, CRGB colour);
        int colours_len;
        CRGB colours_arr[];//defines colours of each light in the light group 
};//end LightGroup

#endif