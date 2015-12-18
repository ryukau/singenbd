#include "Oscillator.h"

#include <cmath>
#include <cassert>
#include "Globals.h"


const float cc = 1.5;


Oscillator::Oscillator() :
    mt(0),
    rnd(-1.0, 1.0),
    type(OscillatorType::Simple),
    shape(0.0),
    phaseOffset(0.0),
    noiseRatio(0.0),
    harmonics(300)
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
    setNoiseRatio(osc.getNoiseRatio());
    setHarmonics(osc.getHarmonics());

    return *this;
}

Oscillator::OscillatorType Oscillator::getType()
{
    return type;
}

float Oscillator::getShape()
{
    return shape / 4.0f;
}

float Oscillator::getPhaseOffset()
{
    return phaseOffset;
}

float Oscillator::getNoiseRatio()
{
    return noiseRatio;
}

int Oscillator::getHarmonics()
{
    return harmonics;
}

void Oscillator::setType(Oscillator::OscillatorType oscType)
{
    type = oscType;
}

void Oscillator::setShape(float shape_)
{
    shape = shape_ * 4.0f;
}

void Oscillator::setPhaseOffset(float phase)
{
    phaseOffset = phase;
}

void Oscillator::setNoiseRatio(float ratio)
{
    noiseRatio = ratio;
}

void Oscillator::setHarmonics(int harmonics)
{
    this->harmonics = harmonics;
}


// t: time, f: frequency
float Oscillator::Oscillation(float phase)
{
    assert(0.0f <= shape && shape <= 4.0f);

    float out = 0.0f;
    switch (type)
    {
    case OscillatorType::Simple:
        out = simpleOsc(phase);
        break;

    case OscillatorType::Additive:
        out = additiveOsc(phase);
        break;
    }

    return (1.0f - noiseRatio)*out + noiseRatio*Noise();
}


float Oscillator::simpleOsc(float phase)
{
    if (shape < 1.0f)
    {
        float ratio = shape;
        return ratio*SimpleSaw(phase, 0.5f) - (1.0f - ratio)*Cos(phase);
    }
    else if (shape < 2.0f)
    {
        return SimpleSaw(phase, shape / 2.0f);
    }
    else if (shape < 3.0f)
    {
        float ratio = shape - 2.0f;
        return (1.0f - ratio)*SimpleSaw(phase, 1.0f) + ratio*SimpleSquare(phase, 0.5f);
    }
    else
    {
        return SimpleSquare(phase, (shape - 2.1f) / 2.0f);
    }
}


float Oscillator::additiveOsc(float phase)
{
    if (shape <= 0.0f)
    {
        return Sin(phase);
    }
    else if (shape < 1.0f)
    {
        float ratio = shape;
        return ratio*AdditiveSaw(phase, 0.5f) - (1.0f - ratio)*Sin(phase);
    }
    else if (shape < 2.0f)
    {
        return AdditiveSaw(phase, shape / 2.0f);
    }
    else if (shape < 3.0f)
    {
        float ratio = shape - 2.0f;
        return (1.0f - ratio)*AdditiveSaw(phase, 1.0f) + ratio*AdditiveSquare(phase, 0.5f);
    }
    else
    {
        return AdditiveSquare(phase, (shape - 2.1f) / 2.0f);
    }
}


float Oscillator::Sin(float phase)
{
    phase += phaseOffset;
    return sinf(2.0f * PI * phase);
}


float Oscillator::Cos(float phase)
{
    phase += phaseOffset;
    return cosf(2.0f * PI * phase);
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


float Oscillator::AdditiveSaw(float phase, float pw)
{
    phase += phaseOffset + pw * 0.5f; // 2 * M_PI を掛け合わせていないので、位相は 0.0 から 1.0 の範囲で回転する。d * 0.5 を足し合わせることで、波が 0.0 から始まる。
    float output = 0.0f;


    // 0 除算を防ぐために if で場合分け
    if (pw <= 0.0)
    {
        float tp = 2.0f * PI;
        for (float n = 1.0; n <= harmonics; ++n)
        {
            output -= sin(tp * n * phase) / n / PI;
        }

        return 2.0f * output;
    }
    else if (1.0f <= pw)
    {
        float tp = 2.0f * PI;
        for (int n = 1; n <= harmonics; ++n)
        {
            output += sin(tp * n * phase) / n / PI;
        }

        return 2.0f * output;
    }
    else
    {
        float tpn; // 2 * pi * n
        float a_n, b_n; // フーリエ級数

        for (int n = 1; n <= harmonics; ++n)
        {
            tpn = 2.0f * PI * (float)n + cc;
            a_n = -(cosf(tpn * pw) / pw - cosf(tpn * (1.0f - pw)) / (pw - 1.0f) + 1.0f / (pw*(pw - 1.0f))) / pow(tpn, 2.0f);
            b_n = (sinf(tpn * pw) / pw - sinf(tpn * (1.0f - pw)) / (1.0f - pw)) / pow(tpn, 2.0f);
            output += a_n * cosf(tpn * phase) - b_n * sinf(tpn * phase);
        }

        return 4.0f * output;
    }
}


float Oscillator::AdditiveSquare(float phase, float pw)
{
    phase += pw * 0.5f;
    float output = pw;

    for (int n = 1; n <= harmonics; ++n)
    {
        output += sinf(PI * (float)n * pw + cc) * cosf(2.0f * PI * phase * (float)n + cc) * (2.0f / (float)n / PI);
    }

    return 2.0f * output - 1.0f;
}


float Oscillator::Noise()
{
    return rnd(mt);
}
