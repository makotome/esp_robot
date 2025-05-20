#include "SerialLogger.h"
#include <Arduino.h>

void SerialLogger::init()
{
    Serial.begin(115200);
    Serial.println("Serial Logger Initialized");
}

void SerialLogger::log(const char *message)
{
    Serial.println(message);
}

void SerialLogger::log(int value)
{
    Serial.println(value);
}

void SerialLogger::log(float value)
{
    Serial.println(value);
}

void SerialLogger::log(double value)
{
    Serial.println(value);
}

void SerialLogger::log(const char *message, int value)
{
    Serial.print(message);
    Serial.println(value);
}

void SerialLogger::log(const char *message, float value)
{
    Serial.print(message);
    Serial.println(value);
}

void SerialLogger::log(const char *message, double value)
{
    Serial.print(message);
    Serial.println(value);
}

void SerialLogger::log(const char *message, const char *value)
{
    Serial.print(message);
    Serial.println(value);
}