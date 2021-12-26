#ifndef FILTER_H
#define FILTER_H
//#include "sro4m.h"

typedef struct {
  float alpha;

  float out;
  
}FirstOrderIIR;

class LPFilter{
  float _sensorData;
  //int _outputData;
  //float _alpha;
  FirstOrderIIR filt;
  FirstOrderIIR (*pfilt) = &filt;

  public:
  
  LPFilter(float alpha){
  if (alpha < 0.0f) {
    pfilt->alpha = 0.0f;
  }else if (alpha > 1.0f){
    pfilt->alpha = 1.0f;
  }else {
    pfilt->alpha = alpha;
  }
  
  pfilt->out = 0;
  }

  float filterUpdate(float sensorData){
  _sensorData = sensorData;
  pfilt->out = (1.0f - pfilt->alpha) * _sensorData + pfilt->alpha * pfilt->out;

  return pfilt->out;
  }
  
  
};


#endif
