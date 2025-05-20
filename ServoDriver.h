#include <sys/_types.h>
#ifndef ServoDriver_h
#define ServoDriver_h

#include <Servo.h>

#ifndef DEG2RAD
  #define DEG2RAD(g) ((g)*M_PI)/180
#endif

class ServoDriver
{
  public:
    void attach(int pin);
    void detach();
    void setPosition(int position);
    void refresh();
    void setSinPeriod(unsigned int sinPeriod);
    void setAmplitude(unsigned int amplitude) { _amplitude=amplitude; };
    void setPhase(double phase) { _phase=phase; };
    int getPosition() { return _pos; };
  private:
    bool nextSample();
    void write(int position);
  private:
    Servo _servo;
    int _pos;
    long _previousServoCommandMillis;

    unsigned int _amplitude; //角度范围
    double _phase;

    unsigned int _samplingPeriod;
    unsigned int _sinPeriod; //正弦波周期时间
    double _numberSamples; //一个正弦周期的采样数
    double _currentPhase;//当前相位值
    double _increment; //每一次采样的步进角度
    long _previousSamplingMillis;
    long _currentSamplingMillis;
};

#endif