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
#include "filter.h"
#include "pump.h"
#include <Servo.h>
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
int servoPin = 13;

// Filtered data output
float dataFiltered = 0;
float initialHeight = 0;

//Initiate the servo
Servo servo;

//Initiate LCD pins
UTFT    myGLCD(ILI9341_16,38,39,40,41);
URTouch  myTouch( 6, 5, 4, 3, 2);

// Declare which fonts type
extern uint8_t BigFont[];
extern uint8_t SmallFont[];
extern uint8_t SevenSegNumFont[];

//Initiate timer variables
unsigned long previousMillis = 0;  
unsigned long currentMillis ;

// Define global variable and user defined variables
int x, y;                                   // Touch coordinates
int bottleCounter = 0;                      // start at 0
unsigned long timeSet = 5;                  // timer based parameter (minute)
float distanceSet = 5;                      // distance set by the user
float distanceInc = 5;                      // for incrementing the distanceSet
char currentPage, currentMethod;            // page navigation parameters
int start = 0;                              // for toggling start/stop button
int pumpVol = 22;                           // for pump calibration
int intervalVol = 10000;
int multiplierVol = (1000*pumpVol);

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
  currentMethod = '1';

  //Set the servo pin at pin 13 and set the value to zero
  servo.attach(13);
  servo.write(36);
  delay(10);

}

void loop(){

pageCheck();

if(start == 1 && currentMethod == '1'){                               //START SAMPLING WITH TIMER METHOD
      previousMillis = millis();
      
     while(bottleCounter < 6){
        currentMillis = millis();                                       //activate timer
        
        if(currentMillis - previousMillis >= ((timeSet*1000UL*60UL))){    //+((unsigned long)(intervalVol))
          pump.startPump();
          currentMillis = millis();
          previousMillis = currentMillis;
          
          while(currentMillis - previousMillis < (unsigned long) (multiplierVol)){
              currentMillis = millis();
              //myGLCD.print("pumpin", 105, 195);
                }
        pump.stopPump();                                             //Turn off the pump
        bottleCounter += 1;
        servoAngleSet(); 
        myGLCD.clrScr();
        drawMulaiSampling();
        }
      if (myTouch.dataAvailable()) {
      myTouch.read();
      x=myTouch.getX();                                                 // X coordinate where the screen has been pressed
      y=myTouch.getY();                                                 // Y coordinates where the screen has been pressed

      //Stop and Start Button
      if ((x>=205) && (x<=305) && (y>=185) && (y<=225)) 
      {
        drawFrame(205, 185, 305, 225);
        myGLCD.clrScr();
        start ^= 1;
        drawMulaiSampling();
        break;
      }
      }
    }

  }else if(start == 1 && currentMethod == '0'){                               //START SAMPLING WITH SENSOR METHOD
     previousMillis = millis();
      
     while(bottleCounter < 6){
      
      for (uint8_t i=0; i<40; i++){
      dataFiltered = lpfilter.filterUpdate(sensor.measurement());
      }
      if(dataFiltered <= (((initialHeight+distanceSet) - distanceInc))){
        //initialHeight = dataFiltered;
        distanceInc = (distanceInc+distanceSet);

        pump.startPump();
        currentMillis = millis();
        previousMillis = currentMillis;

        while(currentMillis - previousMillis < (unsigned long)(multiplierVol)){
          currentMillis = millis();
          //myGLCD.print("pumpin", 105, 195);
        }
      pump.stopPump();                                              //Turn off the pump
      bottleCounter += 1;
      servoAngleSet();
      myGLCD.clrScr();
      drawMulaiSampling();
      }
      
      if (myTouch.dataAvailable()) {
      myTouch.read();
      x=myTouch.getX();                                                 // X coordinate where the screen has been pressed
      y=myTouch.getY();                                                 // Y coordinates where the screen has been pressed

      //Stop and Start Button
      if ((x>=205) && (x<=305) && (y>=185) && (y<=225)) 
      {
        drawFrame(205, 185, 305, 225);
        myGLCD.clrScr();
        start ^= 1;
        drawMulaiSampling();
        break;
      }
      }
    }

  }


  
}

void servoAngleSet(){
  switch (bottleCounter){
  case 1:
      servo.write(53);
      delay(1000);
      break;
  case 2:
      servo.write(72);
      delay(1000);
      break;
  case 3:
      servo.write(90);
      delay(1000);
      break;
  case 4:
      servo.write(110);
      delay(1000);
      break;
  case 5:
      servo.write(128);
      delay(1000);
      break;
  default:                               //Original Position First Bottle
      servo.write(36);
      delay(1000);
      break;
}
}                                       // end of servoAngleSet() function

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
  case '3':
      touchSettings();
      break;
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
      // If We Chose move servo menu
      if ((x>=35) && (x<=285) && (y>=190) && (y<=230)) {
        drawFrame(35, 190, 285, 230);                 //Button Highlight Function
        currentPage = '3';                            //Current page number for Parameter Menu
        myGLCD.clrScr();                              // Clears the screen
        drawSettings();
//        servo.write(110);
//        delay(1000);
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
      for (uint8_t i=0; i<120; i++){
      dataFiltered = lpfilter.filterUpdate(sensor.measurement());
      }
      initialHeight = dataFiltered;
      
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
          if (distanceSet <= 5)
          {
            distanceSet = 5;
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
          if (distanceSet >= 50)
          {
            distanceSet = 50;
          }
        }
        else if (currentMethod == '1')
        {
        timeSet=timeSet+1;
          if (timeSet >= 99)
          {
            timeSet = 99;
          }
        }
        drawIncDecMetode();
      }
    }
    distanceInc = distanceSet;
}

void touchSettings(){
                
    if (myTouch.dataAvailable()) {        // Touch mechanics for Atur Metode menu
      myTouch.read();
      x=myTouch.getX();                   // X coordinate where the screen has been pressed
      y=myTouch.getY();                   // Y coordinates where the screen has been pressed
      drawIncDecVol();                    //Call the drawIncDecMetode() function
      
      
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
      
      
      if ((x>=35) && (x<=135) && (y>=35) && (y<=75))                        //Servo1 Option Button
      {
        drawFrame(35, 35, 135, 75);
        servo.write(36);
        delay(1000);
      }

      
      if ((x>=175) && (x<=285) && (y>=35) && (y<=75))                       //Servo2 Option Button
      {
        drawFrame(175, 35, 285, 75);
        servo.write(110);
        delay(1000);
      }
   // Setting Increment - Decrement Navigation Buttons
   

    if ((x>=95) && (x<=135) && (y>=120) && (y<=165))          // If User push the LEFT arrow
      {
        drawFrame(95, 120, 135, 165);
        pumpVol = pumpVol - 1;
        if (pumpVol <= 10)
          {
            pumpVol = 10;
          }
        drawIncDecVol();
        multiplierVol = (1000*pumpVol);
      }
      

      if ((x>=205) && (x<=245) && (y>=120) && (y<=165))      // if User push the RIGHT arrow
      {
        drawFrame(205, 120, 245, 165);
          pumpVol = pumpVol + 1;
          if (pumpVol >= 100)
            {
              pumpVol = 100;
            }
          drawIncDecVol();
          multiplierVol = (1000*pumpVol);
        }
     


    }
}


//*** DRAW SCREEN AND UPDATE VARIABLES FUNCTIONS ***
void drawUpdateValue(){

  myGLCD.print("sec", 17, 95);                    // print ml for the volume
  myGLCD.setBackColor(0, 0, 0);

  if (currentMethod == '0')                       // Check wether the user has chosen the "sensor" method or the "timer" method
  {
  myGLCD.print("Sensor", 100, 55);                // print "cm" for sensor method
  myGLCD.print("cm", 125, 95);     
  }
  else
  {
  myGLCD.print("Timer ", 100, 55);                // print "menit" for timer method
  myGLCD.print("min", 125, 95);   
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
  
  myGLCD.printNumI(pumpVol, 5, 115, 2, '0');
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
  myGLCD.print("  cm  ", 125, 165);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setColor(0, 255, 0);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.printNumI(distanceSet, 140, 115, 2, '0');
  }
  
  else if (currentMethod == '1')
  {
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.print("minute", 125, 165);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setColor(0, 255, 0);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.printNumI(timeSet, 140, 115, 2, '0');
  
  }
}

void drawIncDecVol(){
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

  myGLCD.setBackColor(0, 0, 0);
  myGLCD.print("  sec ", 125, 165);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setColor(0, 255, 0);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.printNumI(pumpVol, 140, 115, 2, '0');
}

//****      DRAW HOME SCREEN MENU FUNCTION      ***
void drawHomeScreen() {
  // Clear Previous Screen
  myGLCD.clrScr();
  // Title - PROTOSS WATER SAMPLER
  myGLCD.setBackColor(0,0,0); // Sets the background color of the area where the text will be printed to black
  myGLCD.setColor(255, 255, 255); // Sets color to white
  myGLCD.setFont(BigFont); // Sets font to big
  myGLCD.print("SISTERA", CENTER, 10); // Prints the string on the screen
  myGLCD.setColor(0, 0, 255); // Sets color to blue
  myGLCD.drawLine(0,32,319,32); // Draws the BLUE line
  myGLCD.setColor(255, 255, 255); // Sets color to white
  myGLCD.setFont(SmallFont); // Sets the font to small
  myGLCD.print("Powered by Protoss Technology", CENTER, 41); // Prints the string    Your trusted partner for water sampling
  myGLCD.setFont(BigFont);
  myGLCD.print("MAIN MENU", CENTER, 64);
  
  // Button - MULAI SAMPLING
  myGLCD.setColor(16, 107, 163); // Sets green color
  myGLCD.fillRoundRect (35, 90, 285, 130); // Draws filled rounded rectangle
  myGLCD.setColor(0, 0, 255); // Sets color to blue
  myGLCD.drawRoundRect (35, 90, 285, 130); // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
  myGLCD.setColor(255, 255, 255);
  myGLCD.setFont(BigFont); // Sets the font to big
  myGLCD.setBackColor(16, 107, 163); // Sets the background color of the area where the text will be printed to green, same as the button 
  myGLCD.print("START SAMPLING", CENTER, 102); // Prints the string
  
  // Button - ATUR METODE
  myGLCD.setColor(16, 107, 163);
  myGLCD.fillRoundRect (35, 140, 285, 180);
  myGLCD.setColor(0, 0, 255);
  myGLCD.drawRoundRect (35, 140, 285, 180);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(16, 107, 163);
  myGLCD.print("SAMPLING METHOD", CENTER, 152);
 
  // Button - ATUR VOLUME
  myGLCD.setColor(16, 107, 163);
  myGLCD.fillRoundRect (35, 190, 285, 230);
  myGLCD.setColor(0, 0, 255);
  myGLCD.drawRoundRect (35, 190, 285, 230);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(16, 107, 163);
  myGLCD.print("SETTINGS", CENTER, 202);
  
}

//****      DRAW MULAI SAMPLING USER INTERFACE      ***

void drawMulaiSampling(){
  // Clear Previous Screen
  myGLCD.clrScr();
  
  //Write Title
  myGLCD.setBackColor(0,0,0);                             // Sets the background color of the area where the text will be printed to black
  myGLCD.setColor(255, 255, 255);                         // Sets color to white
  myGLCD.setFont(BigFont);                                // Sets font to big
  myGLCD.print("SISTERA WATERSAMPLER", CENTER, 10);       // Prints the string on the screen
  myGLCD.setColor(0, 0, 255);                             // Sets color to blue
  myGLCD.drawLine(0,32,319,32);                           // Draws the blue line
  
  //Write Column Identifiers
  myGLCD.setColor(255, 255, 255);
  myGLCD.print("PUMP", 12, 35);
  myGLCD.print("Timer", 5, 55);
  myGLCD.print("Bottle", 215, 35);
  myGLCD.print("Counter", 205, 55);
  myGLCD.print("Method", 100, 35);

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
  myGLCD.print("Start", 215, 195);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.print("Idle..", 105, 195);
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
  myGLCD.print("Choose Sampling Method", 0, 0);
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
  myGLCD.print("OK", 215, 195);
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
  myGLCD.print("Timer", 185, 45);
  myGLCD.setBackColor(0, 0, 0);
}

void drawSettings(){
  // Clear Previous Screen
  myGLCD.clrScr();
  
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print("Set Pump Timer", 0, 0);
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
  myGLCD.print("OK", 215, 195);
  myGLCD.setBackColor(0, 0, 0);

  myGLCD.setColor(16, 167, 103);
  myGLCD.fillRoundRect (35, 35, 135, 75);
  myGLCD.setColor(0, 0, 255);
  myGLCD.drawRoundRect (35, 35, 135, 75);
  myGLCD.setBackColor(16, 167, 103);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print("Servo1", 38, 45);
  myGLCD.setBackColor(0, 0, 0);

  myGLCD.setColor(16, 167, 103);
  myGLCD.fillRoundRect (175, 35, 285, 75);
  myGLCD.setColor(0, 0, 255);
  myGLCD.drawRoundRect (175, 35, 285, 75);
  myGLCD.setBackColor(16, 167, 103);
  myGLCD.setColor(255, 255, 255);
  myGLCD.print("Servo2", 185, 45);
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
