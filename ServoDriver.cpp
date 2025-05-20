#include <cmath>
#include "core_esp8266_features.h"
#include "ServoDriver.h"


bool ServoDriver::nextSample()
{
  _currentSamplingMillis = millis();

  if(_currentSamplingMillis - _previousSamplingMillis > _samplingPeriod){
    _previousSamplingMillis = _currentSamplingMillis;
    return true;
  }

  return false;
}

void ServoDriver::attach(int pin)
{
  if(!_servo.attached()){
    _servo.attach(pin, 500, 2500);
    _pos = 90;
    _servo.write(90);

      //正弦波函数运动轨迹参数
    _samplingPeriod = 30; //每30毫秒采样一次
    _sinPeriod = 2000; //整个周期为2秒
    _numberSamples = _sinPeriod / _samplingPeriod;//一个周期的采样次数
    _increment = 2*M_PI/_numberSamples;//每一次采样步进的角度

    _previousSamplingMillis = 0;
  }
}

void ServoDriver::detach()
{
  if(_servo.attached())
    _servo.detach();
}

void ServoDriver::setPosition(int position)
{
  write(position);
}

void ServoDriver::refresh()
{
  int pos = round(_amplitude * sin(_currentPhase + _phase));
  write(pos + 90);

  _currentPhase = _currentPhase + _increment;
}

void ServoDriver::setSinPeriod(unsigned int sinPeriod)
{
  _sinPeriod = sinPeriod;

  _numberSamples = _sinPeriod/_samplingPeriod;
  _increment = 2*M_PI/_numberSamples;
}

void ServoDriver::write(int position)
{
  long currentMillis = millis();
  _pos = position;
  _previousServoCommandMillis = currentMillis;
  _servo.write(_pos);
}



