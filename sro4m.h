#ifndef SRO4M_H
#define SRO4M_H

class Sro4m{
  int _TRIG;
  int _ECHO;
  long _duration;
  float _distance;
  
  public:
  Sro4m(int TRIG, int ECHO){
  _TRIG = TRIG;
  _ECHO = ECHO;
  pinMode(_TRIG, OUTPUT);
  pinMode(_ECHO, INPUT);
  };

  float measurement(){
  digitalWrite(_TRIG, LOW);
  delayMicroseconds(5);

  digitalWrite(_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(_TRIG, LOW);

  _duration = pulseIn(_ECHO, HIGH);
  _distance = _duration*0.034/2;
  //delay(10);

  return _distance;
  }
  
};

#endif SRO4M_H
