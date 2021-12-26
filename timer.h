#ifndef TIMER_H
#define TIMER_H

class Timer{
  long timerStart = 0;
  long timerTarget = 0;

  public:

  void startTimerMin(int minute) {
    timerStart = millis();
    timerTarget = minute*60*1000;
  }
  void startTimerMil(int miliseconds) {
    timerStart = millis();
    timerTarget = miliseconds;
  }
  
  bool isTimerReady(){
    return (millis() - timerStart) > timerTarget;
  }
};

#endif // TIMER_H

// for short delay use
// (millis() % 1000 > 500)
// it will work for delay shorter than 1 sec 
