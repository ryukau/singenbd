#ifndef UTILS_H
#define UTILS_H

const float PI = 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214f;
const float TWOPI = PI * 2;
const float E = 2.71828182845904523536028747135266249775724709369995957496696762772407663035354759457138217852516642742746f;
const float EPS = 1e-50f;

class SampleRate
{
public:
    static unsigned int get() { return sampleRate; }
    static void set(unsigned int rate)
    {
        sampleRate = rate;
    }

private:
    static unsigned int sampleRate;
};

#endif // UTILS_H
