// ***************************************************************************
// *                   PROTOSS AUTOMATIC WATER SAMPLER                       *
// * Copyright (C)2020 PT PROTOSS TEKNO CIPTA, TANGERANG. All right reserved *
// *                    web: http://www.protoss.id/                          *
// ***************************************************************************
//
// This program was made to run on serial modules with a screen 
// resolution of 320x240 pixels.
// 
// This program requires the UTFT, URTouch, and Servo library 
// NewPing Ultrasonic library no longer used in version 2.0
// IMPORTANT: Read the comments in the setup()function and global variables.

#include "sro4m.h"
#include "timer.h"
//#include "servoAng.h"
#include "filter.h"
#include "pump.h"
#include <UTFT.h>
#include <URTouch.h>

// Define Ultrasonic Sensor Pins and Parameters
#define TRIG 12
#define ECHO 11

// Define L298N pins
#define IN1 9
#define IN2 8

// Define filter alpha value
#define IIR_FILTER_ALPHA 0.9f

// Define Servo Pin
//int servoPin = 13;

// Filtered data output
int dataFiltered = 0;
int initialHeight = 0;

//Initiate LCD pins
UTFT    myGLCD(ILI9341_16,38,39,40,41);
URTouch  myTouch( 6, 5, 4, 3, 2);

// Declare which fonts type
extern uint8_t BigFont[];
extern uint8_t SmallFont[];
extern uint8_t SevenSegNumFont[];

// Define global variable and user defined variables
int x, y;                                   // Touch coordinates
int bottleCounter = 0;                      // start at 0
unsigned long timeSet = 5;                  // timer based parameter (minute)
unsigned long distanceSet = 5;              // distance set by the user
unsigned long distanceInc = 5;              // for incrementing the distanceSet
char currentPage, currentMethod;            // page navigation parameters
int start = 0;                              // for toggling start/stop button

// Create objects
Sro4m sensor(TRIG, ECHO);
//ServoAng servoAng(servoPin);
Timer pumpTimer;
LPFilter lpfilter(IIR_FILTER_ALPHA);
Pump pump(IN1, IN2);

void setup(){
  
  // Initialize LCD touchscreen
  myGLCD.InitLCD();
  myGLCD.clrScr();
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);

  // initialize LCD touchscreen Font and Font Color
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(0, 0, 255);

  // Call the draw HomeScreen function and set the current and method page to zero
  drawHomeScreen();
  currentPage = '0';
  currentMethod = '0';
  for (uint8_t i=0; i<15; i++){
      dataFiltered = lpfilter.filterUpdate(sensor.measurement());
  }
  initialHeight = dataFiltered;

}

void loop(){

pageCheck();

if (start == 1){
  pumpManager(currentMethod);
  bottleCounter = pump.pumpCounter();
}else {
  pump.standbyMode();
}



if (pump.pumpCounter() == 6){
  start = 0;
  bottleCounter = 0;
}

  
}

void pageCheck(){
  switch (currentPage){
  case '0':
      touchHomeMenu();
      break;
  case '1':
      touchMulaiSampling();
      break;
  case '2':
      touchAturMetode();
      break;
}
}

void pumpManager(char currentMethod){
    int i;
    if (currentMethod == '1'){
      for (i=0; i<11; i++){
      dataFiltered = lpfilter.filterUpdate(sensor.measurement());
      }
      pump.sensorSampling(distanceSet,initialHeight,dataFiltered);
    }else if (currentMethod == '0'){
      pump.timerSampling(timeSet);
    }
  }

// ***      Touch Screen Mechanics Functions      ***

void touchHomeMenu(){                               // Touch mechanics for the Home Menu

    if (myTouch.dataAvailable()) {
      myTouch.read();                               // Read the touch data
      x=myTouch.getX();                             // X coordinate where the screen has been pressed
      y=myTouch.getY();                             // Y coordinates where the screen has been pressed
      
      // If We Chose the Mulai Sampling menu 
      if ((x>=35) && (x<=285) && (y>=90) && (y<=130)) {
        drawFrame(35, 90, 285, 130);                 // Custom Function - Highlighs the buttons when it's pressed
        currentPage = '1';                           // Current page number for Sampling Menu
        myGLCD.clrScr();                             // Clears the screen
        drawMulaiSampling();                         // Call the sampling() function
      }
      
      // If We Chose the Atur Metode menu
      if ((x>=35) && (x<=285) && (y>=140) && (y<=180)) { 
        drawFrame(35, 140, 285, 180);                 //Button Highlight Function
        currentPage = '2';                            //Current page number for Parameter Menu
        myGLCD.clrScr();                              // Clears the screen
        drawAturMetode();                             // Call the parameter() function
      }  

    }
}           // end of touchHomeMenu() function

void touchMulaiSampling(){
  // Touch mechanics for Sampling menu (Mulai Sampling)
  
    if (myTouch.dataAvailable()) {
      myTouch.read();
      x=myTouch.getX();                               // X coordinate where the screen has been pressed
      y=myTouch.getY();                               // Y coordinates where the screen has been pressed

      //Stop and Start Button
      if ((x>=205) && (x<=305) && (y>=185) && (y<=225)) 
      {
        drawFrame(205, 185, 305, 225);
        myGLCD.clrScr();
        start ^= 1;
    
        if (start == 0){
            pump.standby();
        }else if (start == 1){
            pump.runSampling(timeSet);
        }
        
        drawMulaiSampling();
      }
      //Back to Home Menu (Menu Button)
      if ((x>=5) && (x<=105) && (y>=185) && (y<=225)) 
      {
        drawFrame(5, 185, 105, 225);
        currentPage = '0';
        myGLCD.clrScr();
        drawHomeScreen();
      }
    }
}

void touchAturMetode(){
                
    if (myTouch.dataAvailable()) {        // Touch mechanics for Atur Metode menu
      myTouch.read();
      x=myTouch.getX();                   // X coordinate where the screen has been pressed
      y=myTouch.getY();                   // Y coordinates where the screen has been pressed
      drawIncDecMetode();                 //Call the drawIncDecMetode() function
      
      
      if ((x>=5) && (x<=105) && (y>=185) && (y<=225))       //Back to Home Menu (Menu Button)
      {
        drawFrame(5, 185, 105, 225);
        currentPage = '0';
        myGLCD.clrScr();
        drawHomeScreen();
      }

     
      if ((x>=205) && (x<=305) && (y>=185) && (y<=225))                          //OKE Button 
      {
        drawFrame(205, 185, 305, 225);
        currentPage = '1';
        myGLCD.clrScr();
        drawMulaiSampling();
      }
      
      
      if ((x>=35) && (x<=135) && (y>=35) && (y<=75))                        //Sensor Option Button
      {
        drawFrame(35, 35, 135, 75);
        currentMethod = '0';
        drawIncDecMetode();
      }

      
      if ((x>=175) && (x<=275) && (y>=35) && (y<=75))                       //Time Option Button
      {
        drawFrame(175, 35, 275, 75);
        currentMethod = '1';
        drawIncDecMetode();
      }


   // Setting Increment - Decrement Navigation Buttons
   

       if ((x>=95) && (x<=135) && (y>=120) && (y<=165))       // If User push the LEFT arrow
      {
        drawFrame(95, 120, 135, 165);
        if (currentMethod == '0')
        {
          distanceSet=distanceSet-1;
          if (distanceSet <= 1)
          {
            distanceSet = 1;
          }
        }
        else if (currentMethod == '1')
        {
          timeSet=timeSet-1;
          if (timeSet <= 1)
          {
            timeSet = 1;
          }
        }
        drawIncDecMetode();
      }
     //RIGHT ARROW
      if ((x>=205) && (x<=245) && (y>=120) && (y<=165))       // If User push the RIGHT arrow
      {
        drawFrame(205, 120, 245, 165);
        if (currentMethod == '0')
        {
          distanceSet=distanceSet+1;
          if (distanceSet >= 20)
          {
            distanceSet = 20;
          }
        }
        else if (currentMethod == '1')
        {
        timeSet=timeSet+1;
          if (timeSet >= 30)
          {
            timeSet = 30;
          }
        }
        drawIncDecMetode();
      }
    }
      //distance_set = z_max;            please remove this later
}



//*** DRAW SCREEN AND UPDATE VARIABLES FUNCTIONS ***
void drawUpdateValue(){

  myGLCD.print("ml", 20, 95);                    // print ml for the volume
  myGLCD.setBackColor(0, 0, 0);

  if (currentMethod == '0')                       // Check wether the user has chosen the "sensor" method or the "timer" method
  {
  myGLCD.print("Sensor", 100, 55);                // print "cm" for sensor method
  myGLCD.print("cm", 125, 95);     
  }
  else
  {
  myGLCD.print("Waktu ", 115, 55);                // print "menit" for timer method
  myGLCD.print("menit", 125, 95);   
  }

  // Setting up seven segment font
  myGLCD.setColor(0, 255, 0);
  myGLCD.setFont(SevenSegNumFont);
 

if (currentMethod == '0')                                     // Check wether the user has chosen the "sensor" method or the "timer" method
  {
    myGLCD.printNumI(distanceSet, 110, 115, 2, '0');          // print the ultrasonic sensor value for sensor method
  }
  else
  {
    myGLCD.printNumI(timeSet, 110, 115, 2, '0');             // print the timer value for timer method
  }
  
  myGLCD.printNumI(420, 5, 115, 2, '0');
  myGLCD.printNumI(bottleCounter, 220, 115, 2, '0');

}

//****      DRAW LEFT-RIGHT NAVIGATION MENU FUNCTION      ***

//Atur Metode Increment-Decrement Buttons
void drawIncDecMetode()                                               // Equivalent to drawIncDecVolume() for "ATUR VOLUME" 
{
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (95, 120, 135, 165);
  myGLCD.fillRoundRect (205, 120, 245, 165);
  myGLCD.setColor(0, 255, 0);
  myGLCD.drawRoundRect (95, 120, 135, 165);
  myGLCD.drawRoundRect (205, 120, 245, 165);
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(0, 0, 255);
  myGLCD.print("<", 110, 135);
  myGLCD.print(">", 210, 135);

  if (currentMethod == '0')
  {
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.print("  cm ", 125, 165);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setColor(0, 255, 0);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.printNumI(distanceSet, 140, 115, 2, '0');
  }
  
  else if (currentMethod == '1')
  {
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.print("menit", 125, 165);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setColor(0, 255, 0);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.printNumI(timeSet, 140, 115, 2, '0');
  
  }
}

//****      DRAW HOME SCREEN MENU FUNCTION      ***
void drawHomeScreen() {
  // Clear Previous Screen
  myGLCD.clrScr();
  // Title - PROTOSS WATER SAMPLER
  myGLCD.setBackColor(0,0,0); // Sets the background color of the area where the text will be printed to black
  myGLCD.setColor(255, 255, 255); // Sets color to white
  myGLCD.setFont(BigFont); // Sets font to big
  myGLCD.print("PROTOSS WATER SAMPLE", CENTER, 10); // Prints the string on the screen
  myGLCD.setColor(0, 0, 255); // Sets color to blue
  myGLCD.drawLine(0,32,319,32); // Draws the BLUE line
  myGLCD.setColor(255, 255, 255); // Sets color to white
  myGLCD.setFont(SmallFont); // Sets the font to small
  myGLCD.print("Your trusted partner for water sampling", CENTER, 41); // Prints the string
  myGLCD.setFont(BigFont);
  myGLCD.print("Pilih Menu", CENTER, 64);
  
  // Button - MULAI SAMPLING
  myGLCD.setColor(16, 107, 163); // Sets green color
  myGLCD.fillRoundRect (35, 90, 285, 130); // Draws filled rounded rectangle
  myGLCD.setColor(0, 0, 255); // Sets color to blue
  myGLCD.drawRoundRect (35, 90, 285, 130); // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
  myGLCD.setColor(255, 255, 255);
  myGLCD.setFont(BigFont); // Sets the font to big
  myGLCD.setBackColor(16, 107, 163); // Sets the background color of the area where the text will be printed to green, same as the button 
  myGLCD.print("MULAI SAMPLING", CENTER, 102); // Prints the string
  
  // Button - ATUR METODE
  myGLCD.setColor(16, 107, 163);
  myGLCD.fillRoundRect (35, 140, 285, 180);
  myGLCD.setColor(0, 0, 255);
  myGLCD.drawRoundRect (35, 140, 285, 180);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(16, 107, 163);
  myGLCD.print("ATUR METODE", CENTER, 152);
  
}

//****      DRAW MULAI SAMPLING USER INTERFACE      ***

void drawMulaiSampling(){
  // Clear Previous Screen
  myGLCD.clrScr();
  
  //Write Title
  myGLCD.setBackColor(0,0,0);                             // Sets the background color of the area where the text will be printed to black
  myGLCD.setColor(255, 255, 255);                         // Sets color to white
  myGLCD.setFont(BigFont);                                // Sets font to big
  myGLCD.print("PROTOSS WATER SAMPLE", CENTER, 10);       // Prints the string on the screen
  myGLCD.setColor(0, 0, 255);                             // Sets color to blue
  myGLCD.drawLine(0,32,319,32);                           // Draws the blue line
  
  //Write Column Identifiers
  myGLCD.setColor(255, 255, 255);
  myGLCD.print("Volum", 5, 35);
  myGLCD.print("Air", 15, 55);
  myGLCD.print("Botol", 215, 35);
  myGLCD.print("Terisi", 205, 55);
  myGLCD.print("Metode", 100, 35);

  //Draw the Menu Button
  myGLCD.setColor(16, 167, 103);
  myGLCD.fillRoundRect (5, 185, 105, 225);
  myGLCD.setColor(0, 0, 255);
  myGLCD.drawRoundRect(5, 185, 105, 225);
  myGLCD.setBackColor(16, 167, 103);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print("Menu", 15, 195);
  myGLCD.setBackColor(0, 0, 0);

  //Draw the Start,Stop, or Pumping
  if(start == 1 && currentMethod == '0'){
      
  myGLCD.setColor(16, 167, 103);
  myGLCD.fillRoundRect (205, 185, 305, 225);
  myGLCD.setColor(0, 0, 255);
  myGLCD.drawRoundRect (205, 185, 305, 225);
  myGLCD.setBackColor(16, 167, 103);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print("Stop", 215, 195);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.print("run...", 105, 195);
  }
  else if (start == 1 && currentMethod == '1')                                              // Used to Check wether the User has pushed the "Mulai" Button or not
  {
  
  myGLCD.setColor(16, 167, 103);
  myGLCD.fillRoundRect (205, 185, 305, 225);
  myGLCD.setColor(0, 0, 255);
  myGLCD.drawRoundRect (205, 185, 305, 225);
  myGLCD.setBackColor(16, 167, 103);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print("Stop", 215, 195);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.print("run...", 105, 195);
}
  else
  {
  myGLCD.setColor(16, 167, 103);
  myGLCD.fillRoundRect(205, 185, 305, 225);
  myGLCD.setColor(0, 0, 255);
  myGLCD.drawRoundRect(205, 185, 305, 225);
  myGLCD.setBackColor(16, 167, 103);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print("Mulai", 215, 195);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.print("stoped", 105, 195);
  digitalWrite(10,HIGH);
  digitalWrite(9,LOW);
  digitalWrite(8,0);
  delay(1000);
  }

  drawUpdateValue();
}

//****      DRAW ATUR METODE USER INTERFACE      ***

void drawAturMetode()
{
  // Clear Previous Screen
  myGLCD.clrScr();
  
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print("Pilih Metode Sampling", 0, 0);
  myGLCD.setColor(0, 0, 255);
  myGLCD.drawLine(0,15,319,15);

  myGLCD.setColor(16, 167, 103);
  myGLCD.fillRoundRect (5, 185, 105, 225);
  myGLCD.setColor(0, 0, 255);
  myGLCD.drawRoundRect (5, 185, 105, 225);
  myGLCD.setBackColor(16, 167, 103);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print("Menu", 15, 195);
  myGLCD.setBackColor(0, 0, 0);

  myGLCD.setColor(16, 167, 103);
  myGLCD.fillRoundRect (205, 185, 305, 225);
  myGLCD.setColor(0, 0, 255);
  myGLCD.drawRoundRect (205, 185, 305, 225);
  myGLCD.setBackColor(16, 167, 103);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print("Oke", 215, 195);
  myGLCD.setBackColor(0, 0, 0);

  myGLCD.setColor(16, 167, 103);
  myGLCD.fillRoundRect (35, 35, 135, 75);
  myGLCD.setColor(0, 0, 255);
  myGLCD.drawRoundRect (35, 35, 135, 75);
  myGLCD.setBackColor(16, 167, 103);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print("Sensor", 38, 45);
  myGLCD.setBackColor(0, 0, 0);

  myGLCD.setColor(16, 167, 103);
  myGLCD.fillRoundRect (175, 35, 275, 75);
  myGLCD.setColor(0, 0, 255);
  myGLCD.drawRoundRect (175, 35, 275, 75);
  myGLCD.setBackColor(16, 167, 103);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print("Waktu", 185, 45);
  myGLCD.setBackColor(0, 0, 0);
}



// ***      DRAW FRAME FUNCTION     ***

void drawFrame(int x1, int y1, int x2, int y2)            // This function used to draw a frame on a button's border
{
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (x1, y1, x2, y2);       
  while (myTouch.dataAvailable())
  myTouch.read();
  myGLCD.setColor(0, 0, 255);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
}
