/* 
Create f-18 battery gauge on GC9A01 round 1.28 inch TFT display driven by ESP32 development board by Tanarg
Gauge consists of background image and needle image (used twice)

Sketch uses the TFT_eSPI library, so you will need this loaded and configured, in partular ensure that the display, and pin out, is configured in User_Setup.h
*/

#define USE_HSPI_PORT          // ← Critical! Forces correct SPI2_HOST on ESP32-S3

const byte colorDepth = 16; // Increasing this beyond 8 may cause graphic image issues

#include "BatteryBackground.h" //Jpeg image array
#include "Needle.h" //Jpeg image array

#define DCSBIOS_DEFAULT_SERIAL
#define DCSBIOS_DISABLE_SERVO 
#include <DcsBios.h>


#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite gaugeBack = TFT_eSprite(&tft); // Sprite object for background
TFT_eSprite needleU = TFT_eSprite(&tft); // Sprite object for left needle
TFT_eSprite needleE = TFT_eSprite(&tft); // Sprite object for right needle

int angleMapU = 0; // Declare int value for needle angles
int angleMapE = 0;

void onVoltUChange(unsigned int newValue) {

  angleMapU = map (newValue, 0, 65535, -150, -30);   // Take input for left needle (battery identifier (U). 0-65535 is input value range from DCS. -150 to -30 is the range of the left needle in degrees
  plotGauge (angleMapU, angleMapE);                 //redraw both needles irrespective if only one input value has changed
}
DcsBios::IntegerBuffer voltUBuffer(0x753c, 0xffff, 0, onVoltUChange);


void onVoltEChange(unsigned int newValue) {

  angleMapE = map (newValue, 0, 65535, 150, 30); // Take input for right needle (E)
  plotGauge (angleMapU, angleMapE); //redraw both needles irrespective if only one input value has changed
}
DcsBios::IntegerBuffer voltEBuffer(0x753e, 0xffff, 0, onVoltEChange);

void setup() 
{
  DcsBios::setup();
  tft.begin();
  tft.fillScreen (TFT_BLACK);
  
  gaugeBack.setSwapBytes (true); // Depending on Sprite graphics byte order these may, or may not, be required
  needleU.setSwapBytes (true); 
  needleE.setSwapBytes (true); 
  bitTest ();  // This can be comented out if not required or removed completely, along with the function
}

void loop() 
{
  DcsBios::loop(); 
}

void bitTest () // Test full range of both needles
{ 
  for (int i = 0; i < 120; i += 5){             // Test range of both needles from min to max
    int angleMapU = map (i, 0, 120, -150, -30); // Up (0) is the default starting point for the needle. needleU will move clockwise on the left of the gauge
    int angleMapE = map (i, 0, 120, 150, 30);   // needleE will move anti-clockwise on the right of the gauge
    plotGauge (angleMapU, angleMapE);
  }

  for (int i = 119; i >= 0; i -= 5) // Test range of both needles from max to min
  { 
    int angleMapE = map (i, 0, 120, 150, 30);
    int angleMapU = map (i, 0, 120, -150, -30);
    plotGauge (angleMapU, angleMapE);
  }
}

void plotGauge (int16_t angleU, int16_t angleE) // Function takes both needle U & E input values, creates needle rotations and pushes them into background image (Sprite)
{ 
  createBackground (); // Create Sprites
  createNeedleU (); 
  createNeedleE (); 
  needleU.pushRotated (&gaugeBack, angleU, TFT_TRANSPARENT);
  needleE.pushRotated (&gaugeBack, angleE, TFT_TRANSPARENT);
  gaugeBack.pushSprite (0, 0, TFT_TRANSPARENT);
  gaugeBack.deleteSprite (); // Delete Sprites to free up memory
  needleU.deleteSprite();
  needleE.deleteSprite();
}

void createBackground () //Create battery dial background as full screen Sprite and set pivot point for needles 
{ 
  gaugeBack.setColorDepth (colorDepth);
  gaugeBack.createSprite (240, 240); // Size of the screen in pixels
  gaugeBack.setPivot (120, 120); // Pivot point in centre of screen (Gauge)
  gaugeBack.fillSprite (TFT_TRANSPARENT);
  gaugeBack.pushImage (0, 0, 240, 240, Battery); // (x, y, dwidth, dheight, image);
}

void createNeedleU () // Create needles as Sprites from single graphic and set pivot point
{ 
  needleU.setColorDepth (colorDepth);
  needleU.createSprite (15, 88); //Size of the needle (must be accurate)
  needleU.setPivot (7, 84);  // Pivot point of the needle  
  needleU.pushImage (0, 0, 15, 88, Needle); // (x, y, dwidth, dheight, image);
}

void createNeedleE () // Create needles as Sprites from single graphic and set pivot point
{ 
  needleE.setColorDepth (colorDepth);
  needleE.createSprite (15, 88); //Size of the needle (must be accurate)
  needleE.setPivot (7, 84);  // Pivot point of the needle 
  needleE.pushImage (0, 0, 15, 88, Needle); // (x, y, dwidth, dheight, image);
}
