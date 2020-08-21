#include <FastLED.h>
#include "striplib.h"

int LightGroup::LIGHTGROUPS_USED=0; 
LightGroup* LightGroup::LIGHTGROUPS[MAX_LIGHTGROUPS] = {0}; 

LightGroup::LightGroup(CRGB newColours_arr[],int newColours_len, int newPosition, CRGB leds[])
{ 
    LIGHTGROUPS[LIGHTGROUPS_USED] = this; 
    LIGHTGROUPS_USED++;
    position = newPosition;
    colours_len = newColours_len;
    colours_arr[newColours_len];
    for (int i = 0; i < newColours_len; i++)
    {
        colours_arr[i] = newColours_arr[i];
    }
    //Move(leds, position);
}
void LightGroup::Move(CRGB leds[], int newPos)
{   
    for (int i=0 ; i< 1; i++)
    {   
        leds[position + i] = CRGB(0,0,0);//clear old positions 
        //leds[newPos + i] = colours_arr[i];//set colours at new position
        position = newPos;
    }
    
}//end MoveGroup
void LightGroup::ChangeColour(int light_i, CRGB colour)//change a single light colour in the light group
{
    colours_arr[light_i] = colour; 
}
static void LightGroup::RedrawGroups(CRGB leds[]){
    for(int j=0;j<LIGHTGROUPS_USED;j++)
    {
        for(int i=0;i<LIGHTGROUPS[j]->colours_len;i++)
        {
            leds[LIGHTGROUPS[j]->position + i] = LIGHTGROUPS[j]->colours_arr[i];
        }
    }
    FastLED.show();
}

