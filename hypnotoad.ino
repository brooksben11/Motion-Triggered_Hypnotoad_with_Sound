#include "Particle.h"
#include "neopixel.h"
SYSTEM_MODE(AUTOMATIC);

#define PIXEL_PIN D8
#define PIXEL_COUNT 2
#define PIXEL_TYPE WS2812B
Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

int G_Max = 255;
int R_Max = 255;
int B_Max = 51; // 1/5 of others

//Started with 25/5
int G_Min = 75;
int R_Min = 75;
int B_Min = 15; // 1/5 of others

//Start Left Eye at full brightness
int Left_Eye_G =        G_Max;
int Left_Eye_R =        R_Max;
int Left_Eye_B =        B_Max;
int Left_Eye_Sign =     -1;

//Start Right Eye at lowest brightness
int Right_Eye_G =       G_Min;
int Right_Eye_R =       R_Min;
int Right_Eye_B =       B_Min;
int Right_Eye_Sign =    +1;

int G_Increment = 5;
int R_Increment = 5;
int B_Increment = 1;




SYSTEM_THREAD(ENABLED)
#include "SdFat.h"
#include "Adafruit_VS1053.h"
SdFat SD;

// These are the pins used for the music maker FeatherWing
const int  MP3_RESET        = -1;                 // VS1053 reset pin (unused!)
const int  SD_CS            = D2;                 // SD Card chip select pin
const int  MP3_CS           = D3;                 // VS1053 chip select pin (output)
const int  DREQ             = D4;                 // VS1053 Data request, ideally an Interrupt pin
const int  MP3_DCS          = D5;                 // VS1053 Data/command select pin (output)
Adafruit_VS1053_FilePlayer musicPlayer(MP3_RESET, MP3_CS, MP3_DCS, DREQ, SD_CS); 

bool Start_Music = false;
bool Eyes = false;
bool Update_Eyes = false;
const String Filename = "Hypnotoad.mp3";
const int Volume = 1; //Lower number is louder

Timer timer_Eyes_Off(11500, Eyes_Off, true); //Need to set the time to match the length of the sound clip
Timer timer_Eyes_Update(10, Eyes_Update, false); //extra important to use timer instead of delay, as it can cause it to intermittently crash




void setup() {
    Particle.subscribe("Motion", Motion, MY_DEVICES);
    Mesh.subscribe("Motion", Motion);
    SD.begin(SD_CS);
    musicPlayer.begin();
    musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);
    musicPlayer.setVolume(Volume, Volume);
    
    
    strip.begin();
    strip.setBrightness(255);
    strip.show(); // Initialize all pixels to 'off'
    
    
}

void loop() {
    if (Start_Music == true){
        musicPlayer.startPlayingFile(Filename);
        Start_Music = false;
    }
    
    if (Eyes == true and Update_Eyes == true){
        //Set lights to new values
        strip.setPixelColor(0, strip.Color(Left_Eye_G, Left_Eye_R, Left_Eye_B)); //strip.Color( greenVal, redVal, blueVal )
        strip.setPixelColor(1, strip.Color(Right_Eye_G, Right_Eye_R, Right_Eye_B));
        strip.show();
        
        //Change the signs when any of the values hits it's max/min
        if (Left_Eye_G >= G_Max or Left_Eye_R >= R_Max or Left_Eye_B >= B_Max or Right_Eye_G <= G_Min or Right_Eye_R <= R_Min or Right_Eye_B <= B_Min) {
            Left_Eye_Sign = -1;
            Right_Eye_Sign = +1;
        }
        else if (Right_Eye_G >= G_Max or Right_Eye_R >= R_Max or Right_Eye_B >= B_Max or Left_Eye_G <= G_Min or Left_Eye_R <= R_Min or Left_Eye_B <= B_Min) {
            Left_Eye_Sign = +1;
            Right_Eye_Sign = -1;
        }
        
        //Calculate new values
        Left_Eye_G = Left_Eye_G + (G_Increment * Left_Eye_Sign);
        Left_Eye_R = Left_Eye_R+ (R_Increment * Left_Eye_Sign);
        Left_Eye_B = Left_Eye_B + (B_Increment * Left_Eye_Sign);
        
        Right_Eye_G = Right_Eye_G + (G_Increment * Right_Eye_Sign);
        Right_Eye_R = Right_Eye_R + (R_Increment * Right_Eye_Sign);
        Right_Eye_B = Right_Eye_B + (B_Increment * Right_Eye_Sign);
        
        Update_Eyes = false;
    }
    
    else if (Eyes == false) {
        //code for shutting off the eyes when they're not on
        strip.setPixelColor(0, strip.Color(0,0,0)); //strip.Color( greenVal, redVal, blueVal )
        strip.setPixelColor(1, strip.Color(0,0,0));
        strip.show();
    }
}

void Motion(const char *event, const char *data)
{
        Start_Music = true;
        Eyes = true;
        Update_Eyes = true;
        timer_Eyes_Off.start();
        timer_Eyes_Update.start();
}

void Eyes_Off()
{
    Eyes = false;
    timer_Eyes_Off.stop();
}

void Eyes_Update()
{
    Update_Eyes = true;
}