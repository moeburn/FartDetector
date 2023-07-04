// Modified SPFD5408 Library by Joao Lopes

#include <SPFD5408_Adafruit_GFX.h>    // Core graphics library
#include <SPFD5408_Adafruit_TFTLCD.h> // Hardware-specific library
#include <SPFD5408_TouchScreen.h>     // Touch library

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin


#define SPEAKER   1  // The speaker is on Digital pin 1, which is unfortunately shared with USB/serial comms


// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GREY    0x7BEF

#define C7 2093
#define D7 2349
#define E7 2637
#define F7 2793
#define G7 3135
#define A7 3520
#define B7 3951
#define C8 4186

      /*beep (SPEAKER, 2093, 250);
      beep (SPEAKER, 2349, 250);
      beep (SPEAKER, 2637, 250);
      beep (SPEAKER, 2793, 250);
      beep (SPEAKER, 3135, 250);
      beep (SPEAKER, 3520, 250);
      beep (SPEAKER, 3951, 250);
      beep (SPEAKER, 4186, 250);*/

//Init LCD:
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET); 

// Calibration values for touch screen pressure
#define SENSIBILITY 300
#define MINPRESSURE 10
#define MAXPRESSURE 1000

//Pins the touch screen is connected to
#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

// Calibration values for touch screen position
#define TS_MINX 125
#define TS_MINY 85
#define TS_MAXX 965
#define TS_MAXY 905

// Init TouchScreen:
TouchScreen ts = TouchScreen(XP, YP, XM, YM, SENSIBILITY);

//All of Joey's declared variables for general display logic:
#define arraylength 250
int sensor;
int sensorlast;
int sensoravg;
int alarmval = 25;
int history = 2;
int historycount = history;
int drawHeightold = 64;
int scrmax = 0;
int scrmin = 0;
int xmax, xmid, xmin;
int labelheight;
int drawArray[arraylength] = { };
int tempArray[arraylength] = { };
int scaleddrawArray[arraylength] = { };
int xPos = 0;
int newpage = 1;
unsigned long lastmillis = 0;
int firstvalue = 1;
int ranged = 0;
int tempmax = -100000;
int tempmin = 100000;
int sensormax = 0;

//All of Joey's variables for touchscreen logic:
boolean firststart = true;
boolean outflag = false;
boolean page1first = true;
boolean firstfart = true;
int screenpage = 0;
int offset = 20;




//---- MAIN CODE - SETUP ------------------------------------------------------------------------------------------------------

//VOID SETUP - means do this on bootup:
void setup(void) 
{
  UCSR0B = 0; //Disable any USB comms to quiet the speaker
  pinMode (A5, INPUT); //Set pin A5 (the gas sensor pin) to input mode
  pinMode(SPEAKER,OUTPUT);  //Set Speaker pin to output mode
  tft.reset(); //Reset and begin the LCD:
  tft.begin(0x9341); // SDFP5408
  tft.setRotation(1); //Set the LCD's rotation to 1 (0, 1, 2, or 3 for 90deg rot)
}



//---- MAIN CODE - LOOP ------------------------------------------------------------------------------------------------------

//VOID LOOP - main code, do this all the time:
void loop(void) 
{
  sensor = analogRead(A5); //variable named "sensor" is assigned current gas sensor value
  
  //Draw the appropriate stuff on the screen depending on what "menu" we're in:
  if (screenpage == 0) 
    {
      page0(); //Main menu (graphing or alarm buttons)
    }
  
  if (screenpage == 1)
    {
      page1(); //ALARM MODE
    }
  
  if (screenpage == 2)
    {
      page2(); //GRAPHING MODE (yes these are backwards compared to menu buttons, tough beans.)
    }
  if (screenpage == 3)
    {
      page3(); //FART MODE (yes these are backwards compared to menu buttons, tough beans.)
    }
}




void page0() //Main menu
{
  if (firststart) //If this is the first time 
    {
      drawBorder(); //draw a red border that probably can't be seen cause of cropped edges
      tft.drawRect (30, 30, 250, 70, BLUE); //draw graphing button rectangle
      tft.drawRect (30, 140, 110, 70, BLUE); //draw alarm button rectangle
      tft.drawRect (170, 140, 110, 70, BLUE); //draw fart button rectangle
      tft.setTextSize (3); //large text size (1-4)
      tft.setTextColor(RED); //if you can't figure out what that does, coding isn't for you.
      tft.setCursor (40, 50); //sets the text cursor position to 40x, 50y, inside of the rectangle
      tft.print("GRAPHING MODE");
      tft.setCursor (40, 160);
      tft.print("ALARM");
      tft.setCursor (190, 160);
      tft.print("FART");
      firststart = false; //It's no longer the first time
    }
  digitalWrite(13, HIGH); //Touchscreen stuff
  TSPoint p = ts.getPoint(); 
  digitalWrite(13, LOW); 
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.height()); //Map raw touchscreen values to actual x,y dimensions of LCD
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.width());;

    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) 
    { //If the screen is being touched
          /*tft.setTextSize (1);  //commented out code for printing raw touchscreen values for debugging
          tft.setCursor (20,20);
          tft.fillRect(10,10,50,50,WHITE);
          tft.print(p.x);
          tft.print(" ");
          tft.print(p.y);*/
       if (p.x > 20 && p.x < 80 && p.y > 50 && p.y < 140) { //If we're touching ALARM button
            tft.drawRect (30, 140, 110, 70, YELLOW); //Make button briefly flash yellow for haptic feedback
            beep(SPEAKER, 6000, 250); //Beep the speaker for double haptic feedback
            delay(250); //Wait 250ms to really let that sweet haptic feedback sink in
            screenpage = 1; //Goto ALARM menu
       }
       if (p.x > 130 && p.x < 180 && p.y > 50 && p.y < 290) { //If we're touching GRAPHING button
            tft.drawRect (30, 30, 250, 70, YELLOW);
            beep(SPEAKER, 5000, 250);
            delay(250);
            screenpage = 2; //Goto GRAPHING menu
       }
       if (p.x > 20 && p.x < 80 && p.y > 180 && p.y < 290) { //If we're touching FART button
            tft.drawRect (170, 140, 110, 70, YELLOW);
            beep(SPEAKER, 5000, 250);
            delay(250);
            screenpage = 3; //Goto FART menu
       }
    }
}




void page1() //ALARM MODE
{
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.height());
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.width());;
        
  if (page1first) //if it's the first moment we've entered ALARM mode
    {
      tft.fillScreen(BLACK); //blank the entire screen, very slow, takes about 250ms
      tft.drawRect (190, 42, 83, 71, BLUE); //draw the up and down buttons
      tft.drawRect (190, 137, 83, 71, BLUE);
      tft.setTextSize (3);
      tft.setTextColor(WHITE);
      tft.setCursor (217, 67);
      tft.print("UP");
      tft.setCursor (197, 162);
      tft.print("DOWN");
      tft.setCursor(25, 25);
      tft.setTextColor(YELLOW);
      tft.print("Val: "); //draw the text
      tft.setCursor(25, 55);
      tft.print("Max: ");
      tft.setCursor(25, 105);
      tft.setTextColor(RED);
      tft.print("ALARM: ");
      page1first = false; //no longer the first moment
    }
  if (millis() - lastmillis >= 1000) //Only do this stuff once per second
    {
      tft.setTextSize (3);
      tft.setTextColor(BLACK); //Set text to black and print same values on top of old - faster way of blanking just what we need to, to prevent letter overstrikes
      tft.setCursor(25, 25);
      tft.print("     ");
      tft.print(sensorlast); //Erase the last value
      tft.setCursor(25, 25);
      tft.setTextColor(YELLOW); //Text color back to yellow so we're printing visible text again
      tft.print("Val: ");
      tft.print(sensor); //print the new value
      sensorlast = sensor; //remember what value to use to erase the last one
      if (sensor > sensormax) //If the current sensor value is higher than we've ever seen
        {
          tft.setCursor(25, 55);
          tft.setTextColor(BLACK); //fast char blank trick
          tft.print("     ");
          tft.print(sensormax);
          sensormax = sensor;
          tft.setCursor(25, 55);
          tft.setTextColor(YELLOW);
          tft.print("Max: ");
          tft.print(sensormax); //Display the maximum sensor value ever recorded
        }
      if (sensor > alarmval) //THIS IS THE ALARM CODE - if current value is higher than set alarm point
        {
          beep (SPEAKER, 2000, 500); //do the beep (2000hz sorta, for 500ms)
        }
      lastmillis = millis(); //Reset "once per second" counter
    }

      if (p.z > MINPRESSURE && p.z < MAXPRESSURE) //if the screen is being touched
      { 
          /*tft.setTextSize (1);  //commented out old debug code
          tft.setCursor (25,85);
          tft.setTextColor(BLACK);
          tft.fillRect(25,85,50,50,WHITE);
          tft.print(p.x);
          tft.print(" ");
          tft.print(p.y);*/
       if (p.x > 110 && p.x < 176 && p.y > 200 && p.y < 270) { //if we're pressing the UP button
            tft.drawRect (190, 42, 83, 71, YELLOW); //more of that sweet sweet haptic feedback stuff
            tft.setTextColor(BLACK);
            tft.setCursor(25, 105);
            tft.print("       ");
            tft.print(alarmval);
            tft.setTextColor(RED);
            tft.setCursor(25, 105);
            tft.print("ALARM: ");
            alarmval += 1; //increase the ALARM point value
            tft.setTextColor(CYAN);
            tft.print(alarmval);
            delay(150); //debounce the button by 150ms (otherwise a quick tap of the button could increase it by like 500 times)
            tft.drawRect (190, 42, 83, 71, BLUE);
       }
       if (p.x > 23 && p.x < 86 && p.y > 200 && p.y < 270) { //if we're pressing the DOWN button
            tft.drawRect (190, 137, 83, 71, YELLOW);
            tft.setTextColor(BLACK);
            tft.setCursor(25, 105);
            tft.print("       ");
            tft.print(alarmval);
            tft.setTextColor(RED);
            tft.setCursor(25, 105);
            tft.print("ALARM: ");
            alarmval -= 1; //DEcrease the ALARM point value
            tft.setTextColor(CYAN);
            tft.print(alarmval);
            delay(150);
            tft.drawRect (190, 137, 83, 71, BLUE);
       }
    }
}

void page2() //GRAPHING MODE
{
      tft.fillScreen(BLACK); //uber slow screen blank, unfortunately it's the only way
      graphhandler(); //run the graphhandler function to shift the arrays one to the left for the line graph data
      
        if (millis() - lastmillis >= 1000) //do this stuff once every second
        {
          if (newpage == 1) //if this is the first moment we've been in graphing mode
          {
            newpage = 0; //no longer the first time
            if (scrmax == 0) {scrmax = sensor + 50;} //old auto ranging code
            if (scrmin == 0) {scrmin = sensor - 50;}
            if (sensor > scrmax) { scrmax = sensor; }
            if (sensor < scrmin) { scrmin = sensor; }
            //xmax = scrmax;
            //xmin = scrmin;
            scrmax = 40; //overriding the auto ranging code since it is not likely we will need it with this gas sensor
            scrmin = 0;
            xmax = 40;
            xmin = 0;
            xmid = (xmax + xmin) / 2;
          }
      
              lastmillis = millis(); // Update lastmillis
      
              tft.setTextSize(2);  //print the graph axis labels in grey
              tft.setTextColor(GREY); 
              tft.setCursor(arraylength, 0+offset);
              tft.print(xmax);
              tft.setCursor(arraylength,116);
              tft.print(xmid);
              tft.setCursor(arraylength,222-offset);
              tft.print(xmin);
      
              tft.setTextColor(RED);
              labelheight = 222-map(drawArray[arraylength-2], scrmin, scrmax, 0, 232-offset); //We have a little floating number that rides the y axis, figure out how high up it should go
              if (labelheight < 0) {labelheight = 0;}
              if (labelheight > 222-offset) {labelheight = 222-offset;}
              tft.setCursor(arraylength, labelheight); 
              tft.print((drawArray[arraylength-2])); //draw that floating number
              tft.setCursor(25, 3+offset);
              tft.print("Val: ");  //draw the plain text values at the top left
              tft.print(sensor);
              tft.print(", Max: ");
              if (sensor > sensormax) {sensormax = sensor;}
              tft.print(sensormax);
      
      
              tft.drawLine(0,120,arraylength-2,120,GREY); //draw the graph axis lines/grid
              tft.drawLine(0,0+offset,arraylength-2,0+offset,GREY);
              tft.drawLine(0,239-offset,arraylength-2,239-offset,GREY);
      
          for (int i = 0; i<(arraylength-1); i++) //Map the raw sensor values to the dimensions of the LCD so we can draw the red graph line
          {
              scaleddrawArray[i] = map(drawArray[i], scrmin, scrmax, 0+offset, 239-offset);
      
          }
          for (int i = 1; i<(arraylength-1); i++)
          {
              tft.drawLine (i-1, 239-scaleddrawArray[i-1], i, 239-scaleddrawArray[i], RED ); //DRAW THAT FUCKER, WOOT
          }
        }
                
        delay(1000); //wait why the hell is this here? I already have a "once per second code", I can't remember.  I'm leaving it in so I don't break anything.
}




void page3() //FART GAME MODE
{
  digitalWrite(13, HIGH);  //do the touchscreen stuff
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.height());
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.width());;

      if (p.z > MINPRESSURE && p.z < MAXPRESSURE) //if the screen is being touched
      { 
        if (p.x > 23 && p.x < 86 && p.y > 200 && p.y < 270) //if we're pressing the RESET button
        { 
            tft.drawRect (190, 137, 83, 71, YELLOW);  //haptic
            tft.setTextSize (3);
            tft.setCursor(25, 55);
            tft.setTextColor(BLACK); //fast char blank trick
            tft.print("          ");
            tft.print(sensormax);
            sensormax = sensor;  //Reset the high score to the current measured value
            tft.setCursor(25, 55);
            tft.setTextColor(YELLOW);
            tft.print("HI SCORE: ");
            tft.print(sensormax); 
            sensormax = sensor;
            delay(250);
            tft.drawRect (190, 137, 83, 71, BLUE);
        }
      }

  if (firstfart)  //if we're just loading the fart screen
  {
    tft.fillScreen(BLACK);
    firstfart = false;
    tft.setTextSize (3);
    tft.setCursor(25, 25);
    tft.setTextColor(YELLOW);
    tft.print("Val: "); //draw the text
    tft.drawRect (190, 137, 83, 71, BLUE);
    tft.setTextColor(WHITE);
    tft.setCursor (203, 162);
    tft.setTextSize (2);
    tft.print("RESET");
  }

  if (millis() - lastmillis >= 1000) //do this stuff once every second
        {
          tft.setTextSize (3);
          tft.setTextColor(BLACK); //Set text to black and print same values on top of old - faster way of blanking just what we need to, to prevent letter overstrikes
          tft.setCursor(25, 25);
          tft.print("     ");
          tft.print(sensorlast); //Erase the last value
          tft.setCursor(25, 25);
          tft.setTextColor(YELLOW); //Text color back to yellow so we're printing visible text again
          tft.print("Val: ");
          tft.print(sensor); //print the new value
          sensorlast = sensor; //remember what value to use to erase the last one
          lastmillis = millis();
        }

  if (sensor > sensormax) //If you broke the high score record
    {
      tft.setTextSize (3);
      tft.setTextColor(BLACK); //Set text to black and print same values on top of old - faster way of blanking just what we need to, to prevent letter overstrikes
      tft.setCursor(25, 25);
      tft.print("     ");
      tft.print(sensorlast); //Erase the last value
      tft.setCursor(25, 25);
      tft.setTextColor(YELLOW); //Text color back to yellow so we're printing visible text again
      tft.print("Val: ");
      tft.print(sensor); //print the new value
      sensorlast = sensor; //remember what value to use to erase the last one
      tft.setTextSize (3);
      tft.setCursor(25, 55);
      tft.setTextColor(BLACK); //fast char blank trick
      tft.print("          ");
      tft.print(sensormax);
      sensormax = sensor;
      tft.setCursor(25, 55);
      tft.setTextColor(YELLOW);
      tft.print("HI SCORE: ");
      tft.print(sensormax); //Display the maximum sensor value ever recorded
      tft.setCursor(25, 85);
      tft.setTextColor(CYAN);
      tft.setTextSize (2);
      tft.print("CONGRATULATIONS!");
      tft.setCursor(25, 105);
      tft.setTextSize (1);
      tft.print("You have the biggest fart!");
      shaveandahaircut();
      delay (3000);
      tft.setCursor(25, 85);
      tft.setTextColor(BLACK);
      tft.setTextSize (2);
      tft.print("CONGRATULATIONS!");
      tft.setCursor(25, 105);
      tft.setTextSize (1);
      tft.print("You have the biggest fart!");
    }
}


void graphhandler() //where all the graphing magic happens
{
  sensoravg += sensor; //most of this is auto ranging stuff that became entirely unnecessary when I decided to force the graph's Y axis range
  if (historycount >= history)
  {
    historycount = 0;
        if (firstvalue == 1) {sensor = sensoravg;} else {sensor = sensoravg/history;}
        sensoravg = 0;
       
        if (scrmax == 0) {scrmax = sensor + 20;}
        if (scrmin == 0) {scrmin = sensor - 20;}
        if (sensor > scrmax) { scrmax = sensor; ranged = 1; }
        if (sensor < scrmin) { scrmin = sensor; ranged = 1; }

      if (drawArray[1] == 0 && drawArray[69] == 0 && drawArray[arraylength] == 0) //Fill up the array on the first time with sensor value so it can draw a flat line
      {
        for(int i(0); i < arraylength; ++i)
         {
           drawArray[i] = sensor;
         }
        firstvalue = 0;
      }

      drawArray[arraylength-1] = sensor; //set newest to newest

      for(int i(0); i < arraylength; ++i) //shift over one
      {
         tempArray[i-1] = drawArray[i];
       }
      for(int i(0); i < arraylength; ++i)
      {
         drawArray[i] = tempArray[i];
       }

       if (ranged == 1) //auto ranging
       {
         tempmax = -100000;
         tempmin = 100000;
         for(int i = 2; i<(arraylength-2); i++)
         {
              if(drawArray[i] > tempmax) {tempmax = drawArray[i];}
         }
         scrmax = tempmax;
         for(int i = 2; i<(arraylength-2); i++)
         {
              if(drawArray[i] < tempmin) {tempmin = drawArray[i];}
         }
         scrmin = tempmin;
       }

       //xmax = scrmax;
       //xmin = scrmin;
       scrmax = 40;
       scrmin = 0;
       xmax = 40; //oh yeah and they're called "xmax" even though it's really ymax.  Fun stuff.
       xmin = 0;
       xmid = (xmax + xmin) / 2;

  }
  historycount++;
}





void drawBorder () { //Initiate communication with Omicron Persei lifeform via binary microwave nah I'm just messing with ya.  Draw a border:
  uint16_t width = tft.width() - 1;
  uint16_t height = tft.height() - 1;
  uint8_t border = 10;
  tft.fillScreen(RED);
  tft.fillRect(border, border, (width - border * 2), (height - border * 2), WHITE);
}

void shaveandahaircut () //never thought I'd be writing a function like this
{
  beep (SPEAKER, C8, 250);
  beep (SPEAKER, G7, 125);
  beep (SPEAKER, G7, 125);
  beep (SPEAKER, A7, 250);
  beep (SPEAKER, G7, 250);
  delay (250);
  beep (SPEAKER, B7, 250);
  beep (SPEAKER, C8, 250);
}


void beep (unsigned char speakerPin, int frequencyInHertz, long timeInMilliseconds) //Not my beep function, someone else's
{   // http://web.media.mit.edu/~leah/LilyPad/07_sound_code.html
          int x;   
          long delayAmount = (long)(1000000/frequencyInHertz);
          long loopTime = (long)((timeInMilliseconds*1000)/(delayAmount*2));
          for (x=0;x<loopTime;x++)   
          {  
              digitalWrite(speakerPin,HIGH);
              delayMicroseconds(delayAmount);
              digitalWrite(speakerPin,LOW);
              delayMicroseconds(delayAmount);
          }  
}
