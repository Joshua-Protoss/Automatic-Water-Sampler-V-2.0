#ifndef SERVOANG_H
#define SERVOANG_H

#include <Servo.h>

class ServoAng{
  Servo _servo;
  int _servoPin;
  
  public:
  ServoAng(int servoPin){
  _servoPin = servoPin;
  _servo.attach(_servoPin);
  _servo.write(38);
  }
  
  void servoAngleSet(int _bottleCounter){
  switch (_bottleCounter){
  case 1:
      _servo.write(53);
      delay(1000);
      break;
  case 2:
      _servo.write(72);
      delay(1000);
      break;
  case 3:
      _servo.write(90);
      delay(1000);
      break;
  case 4:
      _servo.write(110);
      delay(1000);
      break;
  case 5:
      _servo.write(128);
      delay(1000);
      break;
  default:                               //Original Position First Bottle
      _servo.write(38);
      delay(1000);
      break;
}
}                                       // end of servoAngleSet() function
  
};

#endif
