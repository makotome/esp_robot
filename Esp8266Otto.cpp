#include <sys/_intsup.h>
#include "core_esp8266_features.h"
#include "Esp8266Otto.h"

#define LeftLeg 12
#define RightLeg 13
#define LeftFoot 14
#define RightFoot 15

void EspOtto::init()
{
  _servoPins[0] = LeftLeg;
  _servoPins[1] = RightLeg;
  _servoPins[2] = LeftFoot;
  _servoPins[3] = RightFoot;

  attachServos();
}

void EspOtto::attachServos()
{
  for (int i = 0; i < 4; i++)
  {
    _servoDrivers[i].attach(_servoPins[i]);
  }
}

void EspOtto::detachServos()
{
  for (int i = 0; i < 4; i++)
  {
    _servoDrivers[i].detach();
  }
}

void EspOtto::walk(float steps, int direction)
{
  int a[4] = {20, 20, 20, 20};
  double phase[4] = {0, 0, DEG2RAD(direction * -90), DEG2RAD(direction * -90)};

  execute(a, phase, steps);
}

void EspOtto::turn(float steps, int direction)
{
  int a[4] = {20, 20, 20, 20};
  double phase[4] = {0, 0, DEG2RAD(-90), DEG2RAD(-90)};

  if (direction == LEFT)
  {
    a[0] = 30;
    a[1] = 10;
  }
  else
  {
    a[0] = 10;
    a[1] = 30;
  }

  execute(a, phase, steps);
}

void EspOtto::execute(int a[4], double phase[4], float steps = 1.0)
{
  attachServos();

  int cycles = (int)steps;

  if (cycles >= 1)
  {
    for (int i = 0; i < cycles; i++)
    {
      driveServos(a, phase, 1.0);
    }
  }

  driveServos(a, phase, 1.0);
}

void EspOtto::driveServos(int a[4], double phase[4], float cycle = 1.0)
{
  unsigned int period = 3000;
  for (int i = 0; i < 4; i++)
  {
    _servoDrivers[i].setAmplitude(a[i]);
    _servoDrivers[i].setPhase(phase[i]);
    _servoDrivers[i].setSinPeriod(period);
  }

  double ref = millis();
  for (double x = ref; x <= period * cycle + ref; x = millis())
  {
    for (int i = 0; i < 4; i++)
    {
      _servoDrivers[i].refresh();
    }
  }
}

void EspOtto::moveServos(int time, int targets[])
{
  attachServos();

  _finalTime = millis() + time;
  if (time > 10)
  {
    for (int i = 0; i < 4; i++)
    {
      int target = targets[i];
      int pos = _servoDrivers[i].getPosition();
      int count = time / 10;
      _increment[i] = (target - pos) / count;
    }

    for (int iteration = 1; millis() < _finalTime; iteration++)
    {
      _partialTime = millis() + 10;
      for (int i = 0; i < 4; i++)
      {
        int nextPos = _servoDrivers[i].getPosition() + _increment[i];
        _servoDrivers[i].setPosition(nextPos);
        while (millis() < _partialTime)
          ;
      }
    }
  }
  else
  {
    for (int i = 0; i < 4; i++)
    {
      _servoDrivers[i].setPosition(targets[i]);
      while (millis() < _finalTime)
        ;
    }
  }

  bool finished = true;
  while (finished)
  {
    finished = false;
    for (int i = 0; i < 4; i++)
    {
      if (targets[i] != _servoDrivers[i].getPosition())
      {
        finished = true;
        break;
      }
    }

    if (finished)
    {
      for (int i = 0; i < 4; i++)
      {
        _servoDrivers[i].setPosition(targets[i]);
      }

      _partialTime = millis() + 10;
      while (millis() < _partialTime)
        ;
    }
  }
}

void EspOtto::home()
{
  int homes[4] = {90, 90, 90, 90};
  moveServos(2000, homes);
  detachServos();
}
