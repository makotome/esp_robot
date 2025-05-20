#ifndef SerialLogger_h
#define SerialLogger_h

class SerialLogger
{
private:
public:
    void init();
    void log(const char *message);
    void log(int value);
    void log(float value);
    void log(double value);
    void log(const char *message, int value);
    void log(const char *message, float value);
    void log(const char *message, double value);
    void log(const char *message, const char *value);
};

#endif