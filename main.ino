#include <SPI.h>

// ILI9341_due NEW lib by Marek Buriak http://marekburiak.github.io/ILI9341_due/
#include "ILI9341_due_config.h"
#include "ILI9341_due.h"
#include "SystemFont5x7.h"
// Domyślne ustawienia dla Shielda ADAFRUIT
#define TFT_RST 8
#define TFT_DC 9
#define TFT_CS 10
// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
ILI9341_due tft = ILI9341_due(TFT_CS, TFT_DC, TFT_RST);

char textBuff[20];

// Color set
#define	BLACK           0x0000
#define	BLUE            0x88E4
#define	BORDO           0x057C
#define WHITE           0xFFFF
#define YELLOW          0xFFE0
#define RED             0xF800

//Definicje stałych używanych do obliczeń
#define REDRAW_DELAY 32 // Minimalny czas odświeżania wyświetlacza
#define HOR 102    // Horizon circle outside radius (205 is corner to corner
#define XC 160 // Współżędna x środka horyzontu
#define YC 120 // Współżędna y środka horyzontu
#define ANGLE_INC 1 // Angle increment for arc segments, 1 will give finer resolution, 2 or more gives faster rotation
#define DEG2RAD 0.0174532925

//Definicje zmiennych
int roll_angle = 180; // These must be initialed to 180 so updateHorizon(0); in setup() draws
int last_roll = 0; // the whole horizon graphic
int last_pitch = 0;
int roll_delta = 90;  // This is used to set arc drawing direction, must be set to 90 here

//Definicje zmiennych do testów
int test_angle = 0;
int delta = ANGLE_INC;

unsigned long redrawTime = 0;

uint16_t  color;
uint16_t  colorBG = BLACK;

void setup()
{
	randomSeed(analogRead(0));					//Inicjacja generatora liczb pseudo-losowych
	//Inicjacja wyświetlacza
	Serial.begin(9600);
	tft.begin();
	tft.setRotation(iliRotation90);             // Rotacja wyswietlacza o 90 stopni
	tft.setFont(SystemFont5x7);
	//Inicjacja podstawowego widoku wyświetlacza
	tft.fillRect(0, 0,  320, 120, BLUE);                     
	tft.fillRect(0, 120,320, 120, BORDO); 
		drawHorizon(0, 0);
		drawInfo();
  delay(2000);
}

void loop()
{
    // Wymuszenie regularnego odświeżania wyświetlacza
  if (millis() > redrawTime) {
    redrawTime = millis() + REDRAW_DELAY;
    updateHorizon(random(101) - 50, random (101) - 40);
  }
}

void updateHorizon(int angle, int pitch)
{
  bool draw = 1;
  int delta_pitch = 0;
  int pitch_error = 0;
  int delta_roll  = 0;
  while ((last_pitch != pitch) || (last_roll != angle))
  {
    delta_pitch = 0;
    delta_roll  = 0;

    if (last_pitch < pitch) {
      delta_pitch = 1;
      pitch_error = pitch - last_pitch;
    }
    
    if (last_pitch > pitch) {
      delta_pitch = -1;
      pitch_error = last_pitch - pitch;
    }
    
    if (last_roll < angle) delta_roll  = 1;
    if (last_roll > angle) delta_roll  = -1;
    
    if (delta_roll == 0) {
      if (pitch_error > 1) delta_pitch *= 2;
    }
    
    drawHorizon(last_roll + delta_roll, last_pitch + delta_pitch);
    drawInfo();
  }
}

void drawHorizon(int angle, int pitch)
{
  // Fudge factor adjustment for this sketch (so horizon is horizontal when start angle is 0)
  // This is needed as we draw extra pixels to avoid leaving plotting artifacts

  // Calculate coordinates for line start
  float sx = cos(angle * DEG2RAD);
  float sy = sin(angle * DEG2RAD);

  int16_t x0 = sx * HOR + 100;
  int16_t y0 = sy * HOR;

  if ((angle != last_roll) && ((abs(angle) > 35)  || (pitch != last_pitch)))
  {
    tft.drawLine(XC - x0, YC - y0 - 3 + pitch, XC + x0, YC + y0 - 3 + pitch, BLUE);
    tft.drawLine(XC - x0, YC - y0 + 3 + pitch, XC + x0, YC + y0 + 3 + pitch, BORDO);
    tft.drawLine(XC - x0, YC - y0 - 4 + pitch, XC + x0, YC + y0 - 4 + pitch, BLUE);
    tft.drawLine(XC - x0, YC - y0 + 4 + pitch, XC + x0, YC + y0 + 4 + pitch, BORDO);
  }

  tft.drawLine(XC - x0, YC - y0 - 2 + pitch, XC + x0, YC + y0 - 2 + pitch, BLUE);
  tft.drawLine(XC - x0, YC - y0 + 2 + pitch, XC + x0, YC + y0 + 2 + pitch, BORDO);

  tft.drawLine(XC - x0, YC - y0 - 1 + pitch, XC + x0, YC + y0 - 1 + pitch, BLUE);
  tft.drawLine(XC - x0, YC - y0 + 1 + pitch, XC + x0, YC + y0 + 1 + pitch, BORDO);

  tft.drawLine(XC - x0, YC - y0 + pitch,   XC + x0, YC + y0 + pitch,   WHITE);

  last_roll = angle;
  last_pitch = pitch;  
}

void drawInfo(void)
{
    // Update things near middle of screen first (most likely to get obscured)

  // Level wings graphic
  tft.fillRect(160 - 1, 120 - 1, 3, 3, RED);
  tft.drawFastHLine(160 - 30,   120, 24, RED);
  tft.drawFastHLine(160 + 30 - 24, 120, 24, RED);
  tft.drawFastVLine(160 - 30 + 24, 120, 3, RED);
  tft.drawFastVLine(160 + 30 - 24, 120, 3, RED);

  tft.drawFastHLine(160 - 12,   120 - 40, 24, WHITE);
  tft.drawFastHLine(160 -  6,   120 - 30, 12, WHITE);
  tft.drawFastHLine(160 - 12,   120 - 20, 24, WHITE);
  tft.drawFastHLine(160 -  6,   120 - 10, 12, WHITE);

  tft.drawFastHLine(160 -  6,   120 + 10, 12, WHITE);
  tft.drawFastHLine(160 - 12,   120 + 20, 24, WHITE);
  tft.drawFastHLine(160 -  6,   120 + 30, 12, WHITE);
  tft.drawFastHLine(160 - 12,   120 + 40, 24, WHITE);


  tft.setTextColor(WHITE);
  tft.cursorToXY(160 - 12 - 13, 120 - 20 - 3);
  tft.print("10");
  tft.cursorToXY(160 + 12 + 1, 120 - 20 - 3);
  tft.print("10");
  tft.cursorToXY(160 - 12 - 13, 120 + 20 - 3);
  tft.print("10");
  tft.cursorToXY(160 + 12 + 1, 120 + 20 - 3);
  tft.print("10");

  tft.cursorToXY(160 - 12 - 13, 120 - 40 - 3);
  tft.print("20");
  tft.cursorToXY(160 + 12 + 1, 120 - 40 - 3);
  tft.print("20");
  tft.cursorToXY(160 - 12 - 13, 120 + 40 - 3);
  tft.print("20");
  tft.cursorToXY(160 + 12 + 1, 120 + 40 - 3);
  tft.print("20");

  // Display justified angle value near bottom of screen
  tft.setTextColor(YELLOW, BORDO); // Text with background
  tft.setTextArea(120, 180, 50, 10);
//  tft.printAt(last_roll, 120, 180);
//  tft.setTextPadding(24);                // Padding width to wipe previous number
}

int angleGenerator(int maxAngle)	//Generator przechyłu - do testu
{
  // Synthesize a smooth +/- 50 degree roll value for testing
  delta++; if (delta >= 360) test_angle = 0;
  test_angle = (maxAngle + 1) * sin((delta) * DEG2RAD);

  // Clip value so we hold angle near peak
  if (test_angle >  maxAngle) test_angle =  maxAngle;
  if (test_angle < -maxAngle) test_angle = -maxAngle;

  return test_angle;
}

void testRoll(void)					//Obrót
{
  tft.setTextColor(YELLOW, BLUE);
  tft.setTextArea(120, 180, 50, 10);
  tft.printAligned("Roll test", gTextAlignMiddleCenter);

  for (int a = 0; a < 360; a++) {
    //delay(REDRAW_DELAY / 2);
    updateHorizon(angleGenerator(50), 0);
  }
  tft.setTextColor(YELLOW, BLUE);
  tft.setTextArea(120, 180, 50, 10);
  tft.printAligned("                ", gTextAlignMiddleCenter);
}

void testPitch(void)				//Nachylenie
{
  tft.setTextColor(YELLOW, BLUE);
  tft.setTextArea(120, 180, 50, 10);            
  tft.printAligned("Pitch test", gTextAlignMiddleCenter);

  for (int p = 0; p > -50; p--) {
    delay(REDRAW_DELAY / 2);
    updateHorizon(0, p);
  }

  for (int p = -50; p < 50; p++) {
    delay(REDRAW_DELAY / 2);
    updateHorizon(0, p);
  }


  for (int p = 50; p > 0; p--) {
    delay(REDRAW_DELAY / 2);
    updateHorizon(0, p);
  }

  tft.setTextColor(YELLOW, BLUE);
  tft.setTextArea(120, 180, 50, 10);            
  tft.printAligned("                ", gTextAlignMiddleCenter);
}
