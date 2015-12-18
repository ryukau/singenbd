#include "Oscillator.h"

#include <cmath>
#include <cassert>
#include "utils.h"


Oscillator::Oscillator()
    : type(OscillatorType::Sin)
    , shape(0.0)
    , phaseOffset(0.0)
{
}


Oscillator::~Oscillator()
{
}

Oscillator& Oscillator::operator=(Oscillator &osc)
{
    setType(osc.getType());
    setShape(osc.getShape());
    setPhaseOffset(osc.getPhaseOffset());

    return *this;
}

Oscillator::OscillatorType Oscillator::getType()
{
    return type;
}

float Oscillator::getShape()
{
    return shape;
}

float Oscillator::getPhaseOffset()
{
    return phaseOffset;
}

void Oscillator::setType(Oscillator::OscillatorType oscType)
{
    type = oscType;
}

void Oscillator::setShape(float shape_)
{
    shape = shape_;
}

void Oscillator::setPhaseOffset(float phase)
{
    phaseOffset = phase;
}


// t: time, f: frequency
float Oscillator::Oscillation(float phase)
{
    assert(0.0f <= shape && shape <= 1.0f);

    float out = 0.0f;

    switch (type)
    {
    case OscillatorType::Sin:
        out = Sin(phase);
        break;

    case OscillatorType::Saw:
        out = SimpleSaw(phase, shape);
        break;

    case OscillatorType::Square:
        out = SimpleSquare(phase, shape);
        break;
    }

    return out;
}


//
// private
//

// 全てのオシレータは入力が[0.0, 1.0]の範囲をとるように正規化されている。

float Oscillator::Sin(float phase)
{
    phase += phaseOffset;
    return sinf(2.0f * PI * phase);
}


float Oscillator::SimpleSaw(float phase, float pw)
{
    phase += phaseOffset;
    phase = fmod(fabs(phase), 1.0f);
    float output = 0.0f;

    if (pw <= 0.0f || 1.0f <= pw)
    {
        output = phase;
    }
    else
    {
        if (phase <= pw)
        {
            output = phase / pw;
        }
        else
        {
            output = 1.0f - (phase - pw) / (1.0f - pw);
        }
    }

    return 2.0f * output - 1.0f;
}


float Oscillator::SimpleSquare(float phase, float pw)
{
    return (fmod(phase, 1.0f) < pw) ? -1.0f : 1.0f;
}
