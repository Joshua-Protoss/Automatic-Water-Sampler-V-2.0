#ifndef PUMP_H
#define PUMP_H
//#include "timer.h"
//#include "servoAng.h"


class Pump{
  int _IN1;
  int _IN2;
  int _pumpTime = 5000;
  int _dataFiltered;
  int _timeSet;
  int _pumpCounter = 0;
  int _distanceSet;
  int _initialHeight;
//  Timer timer;
//  Timer pumpTimer;

  enum pumpState {
    STANDBY,
    RUNNING,
    
  };

  pumpState _currentState = STANDBY;
  pumpState _pumpState = STANDBY;
  
  public:
  Pump(int IN1, int IN2){
  _IN1 = IN1;
  _IN2 = IN2;

  pinMode(_IN1, OUTPUT);
  pinMode(_IN2, OUTPUT);

  digitalWrite(_IN1, LOW);
  digitalWrite(_IN2, LOW);
  }

  void standby(){
    _currentState = STANDBY;
  }
  

  int pumpCounter(){
    _pumpCounter += 1;
    if (_pumpCounter == 6){
      return _pumpCounter;
      _pumpCounter = 0;
    }
    return _pumpCounter;
  }





  //private:

  void startPump(){
   
    digitalWrite(_IN1, LOW);
    digitalWrite(_IN2, HIGH);
 
  }

  void stopPump(){
    
    digitalWrite(_IN1, LOW);
    digitalWrite(_IN2, LOW);

  }
  
};

#endif
