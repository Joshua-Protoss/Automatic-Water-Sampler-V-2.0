#ifndef PUMP_H
#define PUMP_H
#include "timer.h"
#include "servoAng.h"

// use pin ENA to use PWM
int servoPin  = 13;
ServoAng servoAng(servoPin);

class Pump{
  int _IN1;
  int _IN2;
  int _pumpTime = 5000;
  int _dataFiltered;
  int _timeSet;
  int _pumpCounter = 0;
  int _distanceSet;
  int _initialHeight;
  Timer timer;
  Timer pumpTimer;

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
  
  void runSampling(int timeSet){
    _currentState = RUNNING;
    timer.startTimerMin(timeSet);
  }

  int pumpCounter(){
    _pumpCounter += 1;
    if (_pumpCounter == 6){
      return _pumpCounter;
      _pumpCounter = 0;
    }
    return _pumpCounter;
  }

  void sensorSampling(int distanceSet, int initialHeight, int dataFiltered){
   
    _dataFiltered = dataFiltered;
    _distanceSet = distanceSet;
    _initialHeight = initialHeight;

    if (_dataFiltered < (_initialHeight - _distanceSet)){
      startPump();
      _initialHeight = _initialHeight - _distanceSet;
    }

    if (_pumpState == RUNNING){
        
      if (pumpTimer.isTimerReady()){
        stopPump();
        pumpCounter();
        servoAng.servoAngleSet(_pumpCounter);
        }
        
      }
    
  }

  void timerSampling(int timeSet){
    _timeSet = timeSet;
    
    if (_currentState == RUNNING){

      if (timer.isTimerReady()){
         startPump();
         timer.startTimerMin(_timeSet);
    }
    }
    
    if (_pumpState == RUNNING){
        
      if (pumpTimer.isTimerReady()){
        stopPump();
        pumpCounter();
        servoAng.servoAngleSet(_pumpCounter);
        }
        
      }
    
  }

  void standbyMode(){
    if (_currentState == STANDBY){
      stopPump();
    }
  }

  private:

  void startPump(){
    if (_pumpState == STANDBY){
    digitalWrite(_IN1, LOW);
    digitalWrite(_IN2, HIGH);
    _pumpState = RUNNING;
    pumpTimer.startTimerMil(_pumpTime);
    }
  }

  void stopPump(){
    
    digitalWrite(_IN1, LOW);
    digitalWrite(_IN2, LOW);
    _pumpState = STANDBY;
  }
  
};

#endif
